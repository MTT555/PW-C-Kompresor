#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "decompress.h"
#include "huffman.h"
#include "bits_analyze.h"
#include "dtree.h"

/**
Funkcja dekompresujaca dany plik pochodzacy z tego kompresora
    FILE *input - plik wejsciowy
    FILE *output - plik wyjsciowy
*/
void decompress(FILE *input, FILE *output) {
    // Deklaracja wszystkich zmiennych statycznych i nadanie im odpowiednich wartosci poczatkowych
    int i; // iteracje po petlach
    unsigned char c; // do odczytywania kolejnych znakow
    unsigned char cipher_key[] = CIPHER; // klucz szyfrowania definiowany w cipher.h
    int cipherPos = 0; // aktualna pozycja w szyfrze
    int cipherLength = (int)strlen((char *)cipher_key);
    mod_t currentMode = dictRoad; // zmienna przechowujaca aktualny tryb czytania pliku
    int bufPos = 0, codeBufPos = 0; // aktualna pozycja w buforze na odczytane bity oraz w buforze dla kodow
    int currentBits = 0; // ilosc aktualnie zajetych bitow (w ramach wsparcia dla dekompresji 12-bit)
    int tempCode = 0; // aktualny kod odczytanego symbolu (w ramach wsparcia dla dekompresji 12-bit)
    int curBufSize = 8192; // aktualna wielkosc buforu na odczytane bity
    int curCodeBufSize = 8192; // aktualna wielkosc buforu dla kodow przejsc po drzewie

    // Deklaracja wszystkich zmiennych dynamicznych, odpowiednia alokacja pamieci i inicjacja
    listCodes *list = NULL; // lista na przechowanie odczytanego slownika
    dnode_t *head = NULL; // pomocnicze drzewo dnode
    head = malloc(sizeof(dnode_t)); // alokacja pamieci na korzen
    head->prev = NULL;
    head->left = NULL;
    head->right = NULL;
    dnode_t *iterator = head; // ustawienie pseudoiteratora po drzewie
    unsigned char *buffer = malloc(curBufSize * sizeof(char)); // bufor na odczytane bity
    unsigned char *codeBuf = malloc(curCodeBufSize * sizeof(char)); // bufor dla kodow przejsc po drzewie

    // Odczytywanie flag
    fseek(input, 0, SEEK_END); // odczytanie, gdzie jest koniec pliku
    int inputEOF = ftell(input);
    fseek(input, 2, SEEK_SET); // ustawienie kursora na trzeci znak zawierajacy flagi
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
            analyzeBits(output, c, compLevel, &list, 0, false, &iterator, &currentMode,
                buffer, &curBufSize, codeBuf, &curCodeBufSize, &bufPos, &codeBufPos, &currentBits, &tempCode);
    }
    analyzeBits(output, c, compLevel, &list, reduntantBits, reduntantZero, &iterator, &currentMode,
        buffer, &curBufSize, codeBuf, &curCodeBufSize, &bufPos, &codeBufPos, &currentBits, &tempCode);
    fprintf(stderr, "File successfully decompressed!\n");
    int outputEOF = ftell(output);
#ifdef DEBUG
    if(outputEOF < inputEOF)
        fprintf(stderr, "File size reduced by %.2f%%\n", 100 - 100 * (double)outputEOF / inputEOF);
    else
        fprintf(stderr, "File size increased by %.2f%%\n", 100 * (double)outputEOF / inputEOF - 100);
    fprintf(stderr, "Input: %ld, output: %ld\n", inputEOF, outputEOF);
#endif

    // zwalnianie pamieci
    freeListCodes(&list);
    freeDTree(head);
    free(buffer);
    free(codeBuf);
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
bool compareBuffer(listCodes **list, unsigned char *buf, FILE *stream, int compLevel, bool reduntantZero, int *currentBits, int *tempCode) {
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
                *tempCode <<= 12;
                *tempCode += iterator->character;
                *currentBits += 12;
                if(*currentBits == 12) {
                    int temp = *tempCode % 16;
                    *tempCode >>= 4;
                    fprintf(stream, "%c", (unsigned char)(*tempCode));
                    *tempCode = temp;
                    *currentBits = 4;
                } else {
                    fprintf(stream, "%c", (unsigned char)((*tempCode) / (1 << 8)));
                    if(!reduntantZero)
                        fprintf(stream, "%c", (unsigned char)(*tempCode));
                    *tempCode = 0;
                    *currentBits = 0;
                }
            }
            return true;
        }
        iterator = iterator->next;
    }
    return false;
}