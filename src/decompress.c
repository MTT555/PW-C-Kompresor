#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "decompress.h"
#include "huffman.h"
#include "bits_analyze.h"
#include "dtree.h"
#include "utils.h"

/**
Funkcja dekompresujaca dany plik pochodzacy z tego kompresora
    FILE *input - plik wejsciowy
    FILE *output - plik wyjsciowy
*/
void decompress(FILE *input, FILE *output) {
    /* Deklaracja wszystkich zmiennych statycznych i nadanie im odpowiednich wartosci poczatkowych */
    int i; /* iteracje po petlach */
    uchar c; /* do odczytywania kolejnych znakow */
    uchar *cipher_key = (uchar *)"Politechnika_Warszawska"; /* klucz szyfrowania definiowany w cipher.h */
    int cipherPos = 0; /* aktualna pozycja w szyfrze */
    int cipherLength = (int)strlen((char *)cipher_key);
    mod_t currentMode = dictRoad; /* zmienna przechowujaca aktualny tryb czytania pliku */
    int bufPos = 0, codeBufPos = 0; /* aktualna pozycja w buforze na odczytane bity oraz w buforze dla kodow */
    int currentBits = 0; /* ilosc aktualnie zajetych bitow (w ramach wsparcia dla dekompresji 12-bit) */
    int tempCode = 0; /* aktualny kod odczytanego symbolu (w ramach wsparcia dla dekompresji 12-bit) */
    int curBufSize = 8192; /* aktualna wielkosc buforu na odczytane bity */
    int curCodeBufSize = 8192; /* aktualna wielkosc buforu dla kodow przejsc po drzewie */
    int compLevel, redundantBits; /* zmienne mowiace o poziomie kompresji i ilosci nadmiarowych bitow konczacych */
    bool cipher, redundantZero; /* zmienne do odczytywanie szyfrowania i koniecznosci odlaczenia koncowego nadmiarowego zera */
    int inputEOF; /* zmienne zawierajace pozycje koncowe pliku wejsciowego i wyjsciowego */
#ifdef DEBUG
    int outputEOF;
#endif

    /* Deklaracja wszystkich zmiennych dynamicznych, odpowiednia alokacja pamieci i inicjacja */
    listCodes_t *list = NULL; /* lista na przechowanie odczytanego slownika */
    dnode_t *head = malloc(sizeof(dnode_t)); /* pomocnicze drzewo dnode i alokacja pamieci na korzen */
    dnode_t *iterator = head; /* ustawienie pseudoiteratora po drzewie */
    uchar *buffer = malloc(curBufSize * sizeof(char)); /* bufor na odczytane bity */
    uchar *codeBuf = malloc(curCodeBufSize * sizeof(char)); /* bufor dla kodow przejsc po drzewie */
    head->prev = NULL;
    head->left = NULL;
    head->right = NULL;
    fseek(input, 0, SEEK_END); /* odczytanie, gdzie jest koniec pliku */
    inputEOF = ftell(input);

    /* Odczytywanie flag */
    fseek(input, 2, SEEK_SET); /* ustawienie kursora na trzeci znak zawierajacy flagi */
    fread(&c, sizeof(char), 1, input);
    compLevel = (c & 192) >> 6 ? 4 * (((c & 192) >> 6) + 1) : 0; /* odczytanie poziomu kompresji (192 == 0b11000000) */
    cipher = c & 32 ? true : false; /* odczytanie szyfrowania (32 == 0b00100000) */
    redundantZero = c & 16 ? true : false; /* sprawdzenie, czy konieczne bedzie odlaczenie nadmiarowego koncowego znaku '\0' (16 == 0b00010000) */
    redundantBits = c & 7; /* odczytanie ilosci nadmiarowych bitow konczacych (7 == 0b00000111) */
    fseek(input, 4, SEEK_SET);
#ifdef DEBUG
    /* wyswietlenie odczytanych wartosci na stderr */
    fprintf(stderr, "The following values have been read: "
        "compression level: %d, encrypted: %s, redundant ending bits: %d, redundant '\\0' symbol: %s\n",
        compLevel, cipher ? "true" : "false", redundantBits, redundantZero ? "true" : "false");
#endif

    /* przypadek pliku nieskompresowanego, ale zaszyfrowanego */
    if(compLevel == 0 && cipher) {
        for(i = 4; i < inputEOF; i++) {
            fread(&c, sizeof(char), 1, input);
            c -= cipher_key[cipherPos % cipherLength];
            cipherPos++;
            fwrite(&c, sizeof(char), 1, output);
        }
        return;
    }
    for(i = 4; i < inputEOF; i++) {
        fread(&c, sizeof(char), 1, input);
        if(cipher) { /* odszyfrowanie */
            c -= cipher_key[cipherPos % cipherLength];
            cipherPos++;
        }
        if(i != inputEOF - 1) /* analizowanie kazdego bitu przy pomocy funkcji */
            analyzeBits(output, c, compLevel, &list, 0, false, &iterator, &currentMode,
                buffer, &curBufSize, codeBuf, &curCodeBufSize, &bufPos, &codeBufPos, &currentBits, &tempCode);
    }
    analyzeBits(output, c, compLevel, &list, redundantBits, redundantZero, &iterator, &currentMode,
        buffer, &curBufSize, codeBuf, &curCodeBufSize, &bufPos, &codeBufPos, &currentBits, &tempCode);
    
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
    free(buffer);
    free(codeBuf);
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
    while (iterator != NULL) {
        if(strcmp((char *)iterator->code, (char *)buf) == 0) {
            if(compLevel == 8)
                fprintf(stream, "%c", (uchar)(iterator->character));
            else if(compLevel == 16) {
                fprintf(stream, "%c", (uchar)((iterator->character) / (1 << 8)));
                if(!redundantZero)
                    fprintf(stream, "%c", (uchar)(iterator->character));
            }
            else if(compLevel == 12) {
                *tempCode <<= 12;
                *tempCode += iterator->character;
                *currentBits += 12;
                if(*currentBits == 12) {
                    int temp = *tempCode % 16;
                    *tempCode >>= 4;
                    fprintf(stream, "%c", (uchar)(*tempCode));
                    *tempCode = temp;
                    *currentBits = 4;
                } else {
                    fprintf(stream, "%c", (uchar)((*tempCode) / (1 << 8)));
                    if(!redundantZero)
                        fprintf(stream, "%c", (uchar)(*tempCode));
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