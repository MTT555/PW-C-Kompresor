#ifndef BITSANALYZE_H
#define BITSANALYZE_H

#include <stdlib.h>
#include "huffman.h"
#include "dtree.h"
#include "list.h"
#include "utils.h"
#include "decompress.h"

/* Zmienna przechowujaca aktualny tryb analizy aktualnych bitow */
typedef enum {
    dictRoad, /* slownik - odczytywanie drogi tj. kodu */
    dictWord, /* slownik - odczytywanie slowa */
    bitsToWords /* przeksztalcanie skompresowanych bitow na finalny tekst */
} mod_t;

/**
Funkcja do analizy kolejnych bitow danego chara z pliku skompresowanego
    FILE *output - plik wyjsciowy
    uchar c - znak analizowany
    int compLevel - poziom kompresji podany w bitach
    listCodes_t **list - lista kodow znakow
    short redundantBits - ilosc koncowych bitow do porzucenia w tym znaku 
    bool redundantZero - zmienna odpowiedzialna za sprawdzanie czy pominac nadmiarowy koncowy znak zerowy podczas zapisu (zazwyczaj false)
    dnode_t **iterator - pseudoiterator po pomocniczym drzewie dnode_t
    mod_t *mode - aktualny tryb dzialania analizy bitow (typ zdefiniowany w decompress.h)
    uchar *buffer - bufor na odczytane bity
    int *curBufSize - aktualna wielkosc buforu na odczytane bity
    uchar *codeBuf - bufor na kolejne przejscia po drzewie
    int *curCodeBufSize - aktualna wielkosc bufora dla kodow przejsc po drzewie
    int *bufPos - aktualna pozycja w buforze na bity
    int *codeBufPos - aktualna pozycja w buforze kodow
    int *currentBits - ilosc aktualnie zajetych bitow (w ramach wsparcia dla dekompresji 12-bit)
    int *tempCode - aktualny kod odczytanego symbolu (w ramach wsparcia dla dekompresji 12-bit)
*/
bool analyzeBits(FILE *output, uchar c, flag_t f, listCodes_t **list, dnode_t **iterator,
    mod_t *mode, buffer_t *buf, buffer_t *codeBuf, int *currentBits, int *tempCode);

/**
Funkcja zwracajaca pozadany bit z danego chara
    uchar c - znak
    int x - numer bitu, ktory ma byc zwrocony
*/
short returnBit(uchar c, int x);

#endif