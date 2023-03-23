#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "decompress.h"
#include "huffman.h"

static char cipher_key[] = "Politechnika_Warszawska"; // klucz szyfrowania
static int cipher_pos = 0; // pozycja w szyfrze
static char *buffer = NULL;
static int buf_pos = 0; // aktualna pozycja w buforze
static int cur_buf_size = 4096; // aktualna wielkosc buforu
static char *code_buf = NULL; // bufor dla kodow znakow
static int code_buf_pos = 0; // aktualna pozycja w buforze dla kodow
static mod_t mode = dictRoad; // zmienna przechowujaca aktualny tryb czytania pliku
static dnode_t *head = NULL, *it = NULL; // pomocnicze drzewo dnode oraz pseudoiterator po nim
static currentBits = 0, tempCode = 0;

/**
Funkcja dekompresujaca dany plik pochodzacy z tego kompresora
    FILE *input - plik wejsciowy
    FILE *output - plik wyjsciowy
*/
void decompress(FILE *input, FILE *output) {
    int i;
    listCodes *list = NULL; // lista na przechowanie odczytanego slownika
    buffer = malloc(512 * sizeof(char)); // alokacja pamieci na bufor
    code_buf = malloc(512 * sizeof(char));
    head = malloc(sizeof(dnode_t));
    head->prev = NULL;
    head->left = NULL;
    head->right = NULL;
    it = head;

    // odczytanie, gdzie jest koniec pliku
    fseek(input, 0, SEEK_END);
    int end_pos = ftell(input);
    fseek(input, 0 , SEEK_SET);

    fseek(input, 2, SEEK_SET); // ustawienie kursora na trzeci znak w celu odczytania flag
    char c = fgetc(input);
    int comp_level = ((c & 0b11000000) >> 6) * 4 + 4; // odczytanie poziomu kompresji
    if(comp_level == 4)
        comp_level = 0;
    int cipher = (c & 0b00100000) >> 5; // odczytanie szyfrowania
    bool endingZero = c & 0b00010000 ? true : false; // sprawdzenie, czy konieczne bedzie odlaczenie koncowego zera
    int ending = c & 0b00000111; // odczytanie ilosci bitow konczacych
#ifdef DEBUG
    // wyswietlenie odczytanych wartosci na stderr
    fprintf(stderr, "The following values have been read: "
        "compression level: %d, encrypted: %s, ending bits: %d, reduntant '\\0' symbol: %s\n",
        comp_level, cipher ? "true" : "false", ending, endingZero ? "true" : "false");
#endif
    fseek(input, 4, SEEK_SET);
    int cipher_len = strlen(cipher_key);

    // przypadek pliku nieskompresowanego, ale zaszyfrowanego
    if(comp_level == 0 && cipher) {
        for(i = 4; i < end_pos; i++) {
            c = fgetc(input);
            c -= cipher_key[cipher_pos % cipher_len];
            cipher_pos++;
            fprintf(output, "%c", c);
        }
        return;
    }
    for(i = 4; i < end_pos; i++) {
        c = fgetc(input);
        if(cipher) {
            c -= cipher_key[cipher_pos % cipher_len];
            cipher_pos++;
        }
        if(i != end_pos - 1)
            analyzeBits(output, c, comp_level, &list, 0, false);
    }
    analyzeBits(output, c, comp_level, &list, ending, endingZero);
    fprintf(stderr, "File successfully decompressed!\n");
#ifdef DEBUG
    if(input != stdin && output != stdout) {
        if(ftell(output) < end_pos)
            fprintf(stderr, "File size reduced by %.2f%%\n", 100 - 100 * (double)ftell(output)/end_pos);
        else
            fprintf(stderr, "File size increased by %.2f%%\n", 100 * (double)ftell(output)/end_pos - 100);
        fprintf(stderr, "Input: %ld, output: %ld\n", end_pos, ftell(output));
    }
#endif
    free(buffer); // czyszczenie pamieci zaraz przed zakonczeniem funkcji
    free(code_buf);
}
/**
Funkcja do analizy kolejnych bitow danego chara z pliku skompresowanego
    FILE *output - plik wyjsciowy
    char c - znak analizowany
    int comp_level - poziom kompresji podany w bitach (dla comp_level == 0 - brak kompresji)
    short ending - ilosc koncowych bitow do porzucenia w tym znaku (zazwyczaj 0)
    bool endingZero - zmienna odpowiedzialna za sprawdzanie czy pominac nadmiarowy koncowy znak zerowy podczas zapisu
*/
void analyzeBits(FILE *output, char c, int comp_level, listCodes **list, short ending, bool endingZero) {
    int i;
    short bits = 0; // ilosc przeanalizowanych bitow
    short cur_bit = 0; // wartosc obecnie analizowanego bitu
    short cur_code = 0; // obecny kod przejscia w sciezce
    while (bits != 8 - ending) {
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
                    it = it->prev;
                    code_buf_pos--; // wyjscie o jeden w gore
                } else if(cur_code == 1) {
                    code_buf[code_buf_pos] = '0' + goDown(&it); // przejscie o jeden w dol
                    code_buf_pos++;
                    code_buf[code_buf_pos] = '\0';
                    mode = dictWord;
                } else if(cur_code == 0) {
                    code_buf[code_buf_pos] = '0' + goDown(&it); // przejscie o jeden w dol
                    code_buf_pos++;
                }
                break;
            }
            case dictWord: {
                buffer[buf_pos++] = returnBit(c, bits++);
                buffer[buf_pos++] = returnBit(c, bits++);
                if(buf_pos == comp_level) {
                    int result = 0;
                    for(i = 0; i < comp_level; i++) {
                        result *= 2;
                        result += buffer[i];
                    }
                    addCode(list, result, code_buf);
                    buf_pos = 0;
                    it = it->prev;
                    code_buf_pos--;
                    mode = dictRoad;
                }
                break;
            }
            case bitsToWords: {
                buffer[buf_pos++] = '0' + returnBit(c, bits);
                buffer[buf_pos] = '\0';
                bits++;
                if(compareBuffer(list, buffer, output, comp_level, bits == 8 - ending ? endingZero : false))
                    buf_pos = 0;
                break;
            }
        }
    }
    //freeDnodeTree(head);
}

/**
Funkcja zwracajaca pozadany bit z danego chara
    char c - znak
    int x - numer bitu, ktory ma byc zwrocony
*/
short returnBit(char c, int x) {
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
    char *code - kod tego znaku
*/
void addCode(listCodes **list, int character, char *code) {
    int i;
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
    char *buf - bufor, ktory mozliwe, ze odpowiada jednej z liter
    FILE *stream - strumien, w ktorym ma zostac wydrukowana litera
Zwraca true, jezeli jakis znak zostal znaleziony, w przeciwnym wypadku false
*/
bool compareBuffer(listCodes **list, char *buf, FILE *stream, int comp_level, bool endingZero) {
    listCodes *iterator = (*list);
    while (iterator != NULL) {
        if(strcmp(iterator->code, buf) == 0) {
            if(comp_level == 8)
                fprintf(stream, "%c", iterator->character);
            else if(comp_level == 16) {
                fprintf(stream, "%c", (char)((iterator->character) / (1 << 8)));
                if(!endingZero)
                    fprintf(stream, "%c", (char)(iterator->character));
            }
            else if(comp_level == 12) {
                tempCode <<= 12;
                tempCode += iterator->character;
                currentBits += 12;
                if(currentBits == 12) {
                    int temp = tempCode % 16;
                    tempCode >>= 4;
                    fprintf(stream, "%c", (char)(tempCode));
                    tempCode = temp;
                    currentBits = 4;
                } else {
                    fprintf(stream, "%c", (char)((tempCode) / (1 << 8)));
                    if(!endingZero)
                        fprintf(stream, "%c", (char)(tempCode));
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