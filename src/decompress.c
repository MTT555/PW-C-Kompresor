#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "decompress.h"
#include "huffman.h"
#include "bits_analyze.h"
#include "dtree.h"
#include "utils.h"
#include "alloc.h"
#include "noCompress.h"

/**
Funkcja dekompresujaca dany plik pochodzacy z tego kompresora
    FILE *input - plik wejsciowy
    FILE *output - plik wyjsciowy
*/
bool decompress(FILE *input, FILE *output, settings_t s) {
    /* Deklaracja wszystkich zmiennych statycznych i nadanie im odpowiednich wartosci poczatkowych */
    int i; /* iteracje po petlach */
    uchar c; /* do odczytywania kolejnych znakow */
    uchar *cipherKey = s.cipherKey; /* klucz szyfrowania */
    int cipherPos = 0; /* aktualna pozycja w szyfrze */
    int cipherLength = (int)strlen((char *)cipherKey); /* dlugosc szyfru */
    mod_t currentMode = dictRoad; /* zmienna przechowujaca aktualny tryb czytania pliku */
    int currentBits = 0; /* ilosc aktualnie zajetych bitow (w ramach wsparcia dla dekompresji 12-bit) */
    int tempCode = 0; /* aktualny kod odczytanego symbolu (w ramach wsparcia dla dekompresji 12-bit) */
    flag_t defFlag, allFlag; /* zmienna na odczytanie flag */
    buffer_t buf, codeBuf; /* przechowywanie buforu */
    int inputEOF; /* zmienne zawierajace pozycje koncowe pliku wejsciowego i wyjsciowego */
#ifdef DEBUG
    int outputEOF;
#endif
    /* Deklaracja wszystkich zmiennych dynamicznych, odpowiednia alokacja pamieci i inicjacja */
    listCodes_t *list = NULL; /* lista na przechowanie odczytanego slownika */
    dnode_t *head = NULL, *iterator = NULL; /* pomocnicze drzewo dnode oraz pseudoiterator po nim */
    fseek(input, 0, SEEK_END); /* odczytanie, gdzie jest koniec pliku */
    inputEOF = ftell(input);
    buf.buf = NULL; /* bufor na odczytane bity */
    codeBuf.buf = NULL; /* bufor dla kodow przejsc po drzewie */
    buf.curSize = 8192; /* aktualna wielkosc buforu na odczytane bity */
    buf.pos = 0; /* aktualna pozycja w buforze na odczytane bity */
    codeBuf.curSize = 8192; /* aktualna wielkosc buforu dla kodow przejsc po drzewie */
    codeBuf.pos = 0; /* aktualna pozycja w buforze dla kodow */
    if(!tryMalloc((void **)&head, sizeof(dnode_t)) || !tryMalloc((void **)(&(buf.buf)), sizeof(buf.curSize * sizeof(char)))
        || !tryMalloc((void **)(&(codeBuf.buf)), sizeof(codeBuf.curSize * sizeof(char))))
        return false;
    iterator = head;
    head->prev = NULL;
    head->left = NULL;
    head->right = NULL;

    /* Odczytywanie flag */
    fseek(input, 2, SEEK_SET); /* ustawienie kursora na trzeci znak zawierajacy flagi */
    fread(&c, sizeof(char), 1, input);
    allFlag.compLevel = (c & 192) >> 6 ? 4 * (((c & 192) >> 6) + 1) : 0; /* odczytanie poziomu kompresji (192 == 0b11000000) */
    allFlag.cipher = c & 32 ? true : false; /* odczytanie szyfrowania (32 == 0b00100000) */
    allFlag.redundantZero = c & 16 ? true : false; /* sprawdzenie, czy konieczne bedzie odlaczenie nadmiarowego koncowego znaku '\0' (16 == 0b00010000) */
    allFlag.redundantBits = c & 7; /* odczytanie ilosci nadmiarowych bitow konczacych (7 == 0b00000111) */
    defFlag.compLevel = allFlag.compLevel;
    defFlag.cipher = allFlag.cipher;
    defFlag.redundantZero = false;
    defFlag.redundantBits = 0;
    fseek(input, 4, SEEK_SET);
#ifdef DEBUG
    /* wyswietlenie odczytanych wartosci na stderr */
    fprintf(stderr, "The following values have been read: "
        "compression level: %d, encrypted: %s, redundant ending bits: %d, redundant '\\0' symbol: %s\n",
        allFlag.compLevel, allFlag.cipher ? "true" : "false", allFlag.redundantBits, allFlag.redundantZero ? "true" : "false");
#endif

    /* Przypadek pliku nieskompresowanego, ale zaszyfrowanego */
    if(!allFlag.compLevel && allFlag.cipher) {
        decryptFile(input, output, inputEOF, cipherKey);
        return true;
    }
    /* Analiza pliku */
    for(i = 4; i < inputEOF; i++) {
        fread(&c, sizeof(char), 1, input);
        if(allFlag.cipher) { /* odszyfrowanie */
            c -= cipherKey[cipherPos % cipherLength];
            cipherPos++;
        }
        if(i != inputEOF - 1) /* analizowanie kazdego bitu przy pomocy funkcji */
            if(!analyzeBits(output, c, defFlag, &list, &iterator, &currentMode, &buf, &codeBuf, &currentBits, &tempCode)) {
                freeListCodes(&list);
                freeDTree(head);
                free(buf.buf);
                free(codeBuf.buf);
                return false;
            }
    }
    if(!analyzeBits(output, c, allFlag, &list, &iterator, &currentMode, &buf, &codeBuf, &currentBits, &tempCode)) {
        freeListCodes(&list);
        freeDTree(head);
        free(buf.buf);
        free(codeBuf.buf);
        return false;
    }
    
    /* Komunikat koncowy */
    fprintf(stderr, "File successfully decompressed!\n");
#ifdef DEBUG
    outputEOF = ftell(output);
    if(outputEOF < inputEOF)
        fprintf(stderr, "File size reduced by %.2f%%!\n", 100 - 100 * (double)outputEOF / inputEOF);
    else
        fprintf(stderr, "File size increased by %.2f%%!\n", 100 * (double)outputEOF / inputEOF - 100);
    fprintf(stderr, "Input: %d, output: %d\n", inputEOF, outputEOF);
#endif

    /* Zwalnianie pamieci */
    freeListCodes(&list);
    freeDTree(head);
    free(buf.buf);
    free(codeBuf.buf);
    return true;
}

/**
Funkcja sprawdzajaca, czy aktualny fragment kodu w buforze odpowiada jakiejs literze
Jezeli tak, to zapisuje ta litere do podanego pliku
    listCodes_t **list - poczatek listy, ktora chcemy wyswietlic
    uchar *buf - bufor, ktory mozliwe, ze odpowiada jednej z liter
    FILE *stream - strumien, w ktorym ma zostac wydrukowana litera
Zwraca true, jezeli jakis znak zostal znaleziony, w przeciwnym wypadku false
*/
bool compareBuffer(listCodes_t **list, uchar *buf, FILE *stream, int compLevel, bool redundantZero, int *currentBits, int *tempCode) {
    listCodes_t *iterator = (*list);
    uchar tempC;
    int temp = 0;
    while (iterator != NULL) {
        if(strcmp((char *)iterator->code, (char *)buf) == 0) {
            if(compLevel == 8) {
                tempC = (uchar)(iterator->character);
                fwrite(&tempC, sizeof(char), 1, stream);
            }
            else if(compLevel == 16) {
                tempC = (uchar)((iterator->character) / (1 << 8));
                fwrite(&tempC, sizeof(char), 1, stream);
                if(!redundantZero) {
                    tempC = (uchar)(iterator->character);
                    fwrite(&tempC, sizeof(char), 1, stream);
                }
            }
            else if(compLevel == 12) {
                *tempCode <<= 12;
                *tempCode += iterator->character;
                *currentBits += 12;
                if(*currentBits == 12) {
                    temp = *tempCode % 16;
                    *tempCode >>= 4;
                    tempC = (uchar)(*tempCode);
                    fwrite(&tempC, sizeof(char), 1, stream);
                    *tempCode = temp;
                    *currentBits = 4;
                } else {
                    tempC = (uchar)((*tempCode) / (1 << 8));
                    fwrite(&tempC, sizeof(char), 1, stream);
                    if(!redundantZero) {
                        tempC = (uchar)(*tempCode);
                        fwrite(&tempC, sizeof(char), 1, stream);
                    }
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