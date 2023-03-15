#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "decompress.h"
#include "huffman.h"

static char cipher_key[] = "Politechnika_Warszawska"; // klucz szyfrowania
static char *buffer = NULL;
static int buf_pos = 0; // aktualna pozycja w buforze
static int cur_buf_size = 512; // aktualna wielkosc buforu
static char *code_buf = NULL; // bufor dla kodow znakow
static int code_buf_pos = 0; // aktualna pozycja w buforze dla kodow
static mod_t mode = dictRoad; // zmienna przechowujaca aktualny tryb czytania pliku
static dnode_t *head = NULL, *it = NULL; // pomocnicze drzewo dnode oraz pseudoiterator po nim

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
    
    pack_t pack; // deklaracja union packa
    pack.whole = 0; // i jego czyszczenie

    fseek(input, 2, SEEK_SET); // ustawienie kursora na trzeci znak w celu odczytania flag
    char c = fgetc(input);
    int comp_level = ((c & 0b11000000) >> 6) * 4 + 4; // odczytanie poziomu kompresji
    int cipher = (c & 0b00100000) >> 5; // odczytanie szyfrowania
    int ending = c & 0b00000111; // odczytanie ilosci bitow konczacych
#ifdef DEBUG
    // wyswietlenie odczytanych wartosci na stderr
    fprintf(stderr, "The following values have been read: "
        "compression level: %d, encrypted: %s, ending bits: %d\n", comp_level, cipher ? "true" : "false", ending);
#endif
    fseek(input, 4, SEEK_SET);
    for(i = 4; i < end_pos - 1; i++)
        analyzeBits(output, fgetc(input), comp_level, &list, 0);
    analyzeBits(output, fgetc(input), comp_level, &list, ending);

    free(buffer); // czyszczenie pamieci zaraz przed zakonczeniem funkcji
    free(code_buf);
}
/**
Funkcja do analizy kolejnych bitow danego chara z pliku skompresowanego
    FILE *output - plik wyjsciowy
    char c - znak analizowany
    int comp_level - poziom kompresji podany w bitach (dla comp_level == 0 - brak kompresji)
    short ending - ilosc koncowych bitow do porzucenia w tym znaku (zazwyczaj 0)
*/
void analyzeBits(FILE *output, char c, int comp_level, listCodes **list, short ending) {
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
                int result = 0;
                if(buf_pos == comp_level) {
                    if(comp_level == 8) {
                        for(i = 0; i < 8; i++) {
                            result *= 2;
                            result += buffer[i];
                        }
                        addCode(list, (char)result, code_buf);
                        buf_pos = 0;
                        it = it->prev;
                        code_buf_pos--;
                        mode = dictRoad;
                    } else if(comp_level == 16) { // niedokonczone dla 16-bit
                        for(i = 0; i < 16; i++) {
                            result *= 2;
                            result += buffer[i];
                        }
                        fprintf(output, "%c%c", (char)(result / (2 << 8)), (char)result);
                    } else if(comp_level == 12) {
                        // dorobic
                    }
                }
                break;
            }
            case bitsToWords: {
                bits = 8 - ending;
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
    char character - znak, ktory chcemy zapisac
    char *code - kod tego znaku
*/
void addCode(listCodes **list, char character, char *code) {
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
    listCodes *head - poczatek listy, ktora chcemy wyswietlic
    FILE *stream - strumien, w ktorym ta lista ma zostac wydrukowana
*/
void printList(listCodes **list, FILE *stream) {
    listCodes *iterator = (*list);
    while (iterator != NULL) {
        fprintf(stream, "Character: %c (int value: %d), coded as: %s\n", iterator->character, (int)(iterator->character), iterator->code);
        iterator = iterator->next;
    }
}