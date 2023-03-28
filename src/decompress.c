#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "decompress.h"
#include "huffman.h"

static unsigned char cipher_key[] = CIPHER; // klucz szyfrowania
static int cipherPos = 0; // pozycja w szyfrze
static unsigned char buffer[4096];
static int buf_pos = 0; // aktualna pozycja w buforze
static int cur_buf_size = 4096; // aktualna wielkosc buforu
static unsigned char code_buf[4096]; // bufor dla kodow znakow
static int code_buf_pos = 0; // aktualna pozycja w buforze dla kodow
static mod_t mode = dictRoad; // zmienna przechowujaca aktualny tryb czytania pliku
static dnode_t *head = NULL; // pomocnicze drzewo dnode
static int currentBits = 0, tempCode = 0;

/**
Funkcja dekompresujaca dany plik pochodzacy z tego kompresora
    FILE *input - plik wejsciowy
    FILE *output - plik wyjsciowy
*/
void decompress(FILE *input, FILE *output) {
    int i, cipherLength = (int)strlen((char *)cipher_key);
    unsigned char c;
    listCodes *list = NULL; // lista na przechowanie odczytanego slownika
    head = malloc(sizeof(dnode_t));
    head->prev = NULL;
    head->left = NULL;
    head->right = NULL;
    dnode_t *iterator = head; // ustawienie pseudoiteratora po drzewie

    fseek(input, 0, SEEK_END); // odczytanie, gdzie jest koniec pliku
    int inputEOF = ftell(input);
    fseek(input, 2, SEEK_SET); // ustawienie kursora na trzeci znak w celu odczytania flag
    fread(&c, sizeof(char), 1, input);
    int compLevel = (c & 192) >> 6 ? 4 * (((c & 192) >> 6) + 1) : 0; // odczytanie poziomu kompresji (192 == 0b11000000)
    bool cipher = c & 32 ? true : false; // odczytanie szyfrowania (32 == 0b00100000)
    bool reduntantZero = c & 16 ? true : false; // sprawdzenie, czy konieczne bedzie odlaczenie nadmiarowego koncowego znaku '\0' (16 == 0b00010000)
    int reduntantBits = c & 7; // odczytanie ilosci bitow konczacych (7 == 0b00000111)
    fseek(input, 4, SEEK_SET);
#ifdef DEBUG
    // wyswietlenie odczytanych wartosci na stderr
    fprintf(stderr, "The following values have been read: "
        "compression level: %d, encrypted: %s, reduntant ending bits: %d, reduntant '\\0' symbol: %s\n",
        compLevel, cipher ? "true" : "false", reduntantBits, reduntantZero ? "true" : "false");
#endif

    // przypadek pliku nieskompresowanego, ale zaszyfrowanego
    if(compLevel == 0 && cipher) {
        for(i = 4; i < inputEOF; i++) {
            fread(&c, sizeof(char), 1, input);
            c -= cipher_key[cipherPos % cipherLength];
            cipherPos++;
            fprintf(output, "%c", c);
        }
        return;
    }
    for(i = 4; i < inputEOF; i++) {
        fread(&c, sizeof(char), 1, input);
        if(cipher) {
            c -= cipher_key[cipherPos % cipherLength];
            cipherPos++;
        }
        if(i != inputEOF - 1)
            analyzeBits(output, c, compLevel, &list, 0, false, &iterator);
    }
    analyzeBits(output, c, compLevel, &list, reduntantBits, reduntantZero, &iterator);
    fprintf(stderr, "File successfully decompressed!\n");
    int outputEOF = ftell(output);
#ifdef DEBUG
    if(input != stdin && output != stdout) {
        if(ftell(output) < inputEOF)
            fprintf(stderr, "File size reduced by %.2f%%\n", 100 - 100 * (double)outputEOF / inputEOF);
        else
            fprintf(stderr, "File size increased by %.2f%%\n", 100 * (double)outputEOF / inputEOF - 100);
        fprintf(stderr, "Input: %ld, output: %ld\n", inputEOF, outputEOF);
    }
#endif
    // zwalnianie pamieci
    freeListCodes(&list);
    freeDnodeTree(head);
}

/**
Funkcja do analizy kolejnych bitow danego chara z pliku skompresowanego
    FILE *output - plik wyjsciowy
    unsigned char c - znak analizowany
    int compLevel - poziom kompresji podany w bitach (dla compLevel == 0 - brak kompresji)
    short reduntantBits - ilosc koncowych bitow do porzucenia w tym znaku 
    bool reduntantZero - zmienna odpowiedzialna za sprawdzanie czy pominac nadmiarowy koncowy znak zerowy podczas zapisu (zazwyczaj false)
    dnode_t *iterator - pseudoiterator po pomocniczym drzewie dnode_t
*/
void analyzeBits(FILE *output, unsigned char c, int compLevel, listCodes **list, short reduntantBits, bool reduntantZero, dnode_t **iterator) {
    int i;
    short bits = 0; // ilosc przeanalizowanych bitow
    short cur_bit = 0; // wartosc obecnie analizowanego bitu
    short cur_code = 0; // obecny kod przejscia w sciezce
    while (bits != 8 - reduntantBits) {
        switch(mode) {
            case dictRoad: {
                cur_code = 2 * returnBit(c, bits) + returnBit(c, bits + 1);
                bits += 2;
                if(cur_code == 3) {
                    buf_pos = 0;
                    mode = bitsToWords;
#ifdef DEBUG
                    // wyswietlenie odczytanego slownika na stderr
                    fprintf(stderr, "List of codes read from the dictionary:\n");
                    printList(list, stderr);
#endif
                } else if(cur_code == 2) {
                    *iterator = (*iterator)->prev;
                    code_buf_pos--; // wyjscie o jeden w gore
                } else if(cur_code == 1) {
                    code_buf[code_buf_pos] = '0' + goDown(iterator); // przejscie o jeden w dol
                    code_buf_pos++;
                    code_buf[code_buf_pos] = '\0';
                    mode = dictWord;
                } else if(cur_code == 0) {
                    code_buf[code_buf_pos] = '0' + goDown(iterator); // przejscie o jeden w dol
                    code_buf_pos++;
                }
                break;
            }
            case dictWord: {
                buffer[buf_pos++] = returnBit(c, bits++);
                buffer[buf_pos++] = returnBit(c, bits++);
                if(buf_pos == compLevel) {
                    int result = 0;
                    for(i = 0; i < compLevel; i++) {
                        result *= 2;
                        result += buffer[i];
                    }
                    addCode(list, result, code_buf);
                    buf_pos = 0;
                    *iterator = (*iterator)->prev;
                    code_buf_pos--;
                    mode = dictRoad;
                }
                break;
            }
            case bitsToWords: {
                buffer[buf_pos++] = '0' + returnBit(c, bits);
                buffer[buf_pos] = '\0';
                bits++;
                if(compareBuffer(list, buffer, output, compLevel, bits == 8 - reduntantBits ? reduntantZero : false))
                    buf_pos = 0;
                break;
            }
        }
    }
}

/**
Funkcja zwracajaca pozadany bit z danego chara
    unsigned char c - znak
    int x - numer bitu, ktory ma byc zwrocony
*/
short returnBit(unsigned char c, int x) {
    unsigned char ch = c;
    ch >>= (7 - x);
    return ch % 2;
}

/**
Funkcja wykonujaca przejscie w dol w drzewie dnode
    dnode_t *head - punkt wzgledem ktorego chcemy wykonac przejscie
Zwraca:
    0 - wykonano przejscie w dol w lewo
    1 - wykonano przejscie w dol w prawo
*/
int goDown(dnode_t **head) {
    if((*head)->left == NULL) {
        (*head)->left = malloc(sizeof(dnode_t));
        (*head)->left->prev = (*head);
        (*head)->left->left = NULL;
        (*head)->left->right = NULL;
        (*head) = (*head)->left;
        return 0;
    } else {
        (*head)->right = malloc(sizeof(dnode_t));
        (*head)->right->prev = (*head);
        (*head)->right->left = NULL;
        (*head)->right->right = NULL;
        (*head) = (*head)->right;
        return 1;
    }
}

/**
Funkcja czyszczaca pamiec po drzewie dnodow
    dnode_t *head - korzen drzewa dnode
*/
void freeDnodeTree(dnode_t *head) {
    if(head->left != NULL)
        freeDnodeTree(head->left);
    if(head->right != NULL)
        freeDnodeTree(head->right);
    free(head);
}

/**
Funkcja dodajaca odczytany kod wraz ze znakiem do listy
    listCodes **list - lista, do ktorej chcemy dokonac zapisu
    int character - znak, ktory chcemy zapisac
    unsigned char *code - kod tego znaku
*/
void addCode(listCodes **list, int character, unsigned char *code) {
    listCodes *new = NULL;
    new = malloc(sizeof(listCodes));
    new->character = character;
    new->code = malloc(sizeof(char) * (strlen(code) + 1));
    strcpy(new->code, code);
    new->next = (*list);
    (*list) = new;
}

/**
Funkcja drukujaca odczytany slownik na wybrany strumien
    listCodes **list - poczatek listy, ktora chcemy wyswietlic
    FILE *stream - strumien, w ktorym ta lista ma zostac wydrukowana
*/
void printList(listCodes **list, FILE *stream) {
    listCodes *iterator = (*list);
    while (iterator != NULL) {
        fprintf(stream, "Character: %d, coded as: %s\n", iterator->character, iterator->code);
        iterator = iterator->next;
    }
}

/**
Funkcja sprawdzajaca, czy aktualny fragment kodu w buforze odpowiada jakiejs literze
Jezeli tak, to zapisuje ta litere do podanego pliku
    listCodes **list - poczatek listy, ktora chcemy wyswietlic
    unsigned char *buf - bufor, ktory mozliwe, ze odpowiada jednej z liter
    FILE *stream - strumien, w ktorym ma zostac wydrukowana litera
Zwraca true, jezeli jakis znak zostal znaleziony, w przeciwnym wypadku false
*/
bool compareBuffer(listCodes **list, unsigned char *buf, FILE *stream, int compLevel, bool reduntantZero) {
    listCodes *iterator = (*list);
    while (iterator != NULL) {
        if(strcmp(iterator->code, buf) == 0) {
            if(compLevel == 8)
                fprintf(stream, "%c", iterator->character);
            else if(compLevel == 16) {
                fprintf(stream, "%c", (unsigned char)((iterator->character) / (1 << 8)));
                if(!reduntantZero)
                    fprintf(stream, "%c", (unsigned char)(iterator->character));
            }
            else if(compLevel == 12) {
                tempCode <<= 12;
                tempCode += iterator->character;
                currentBits += 12;
                if(currentBits == 12) {
                    int temp = tempCode % 16;
                    tempCode >>= 4;
                    fprintf(stream, "%c", (unsigned char)(tempCode));
                    tempCode = temp;
                    currentBits = 4;
                } else {
                    fprintf(stream, "%c", (unsigned char)((tempCode) / (1 << 8)));
                    if(!reduntantZero)
                        fprintf(stream, "%c", (unsigned char)(tempCode));
                    tempCode = 0;
                    currentBits = 0;
                }
            }
            return true;
        }
        iterator = iterator->next;
    }
    return false;
}