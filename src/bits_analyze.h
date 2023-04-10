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
    flag_t f - zmienna przekazujaca odczytane flagi
    listCodes_t **list - lista kodow znakow
    dnode_t **iterator - pseudoiterator po pomocniczym drzewie dnode_t
    mod_t *mode - aktualny tryb dzialania analizy bitow
    buffer_t *buf - bufor na odczytanie kody
    buffer_t *codeBuf - bufor na aktualny kod przejsc po drzewie
    int *currentBits - ilosc aktualnie zajetych bitow (w ramach wsparcia dla dekompresji 12-bit)
    int *tempCode - aktualny kod odczytanego symbolu (w ramach wsparcia dla dekompresji 12-bit)
Zwraca:
    0 - funkcja przebiegla pomyslnie
    1 - blad alokacji pamieci
    2 - zly szyfr uniemozliwia dalsza dekompresje
*/
int analyzeBits(FILE *output, uchar c, flag_t f, listCodes_t **list, dnode_t **iterator,
    mod_t *mode, buffer_t *buf, buffer_t *codeBuf, int *currentBits, int *tempCode);

/**
Funkcja zwracajaca pozadany bit z danego chara
    uchar c - znak
    int x - numer bitu, ktory ma byc zwrocony
*/
int returnBit(uchar c, int x);

/**
Funkcja sprawdzajaca, czy aktualny fragment kodu w buforze odpowiada jakiejs literze
Jezeli tak, to zapisuje ta litere do podanego pliku
    listCodes_t **list - poczatek listy, ktora chcemy wyswietlic
    uchar *buf - bufor, ktory mozliwe, ze odpowiada jednej z liter
    FILE *stream - strumien, w ktorym ma zostac wydrukowana litera
    int compLevel - poziom kompresji
    bool endingZero - informacja o nadmiarowym koncowym znaku '\0'
    int *currentBits - obecna ilosc zajetych bitow
    int *tempCode - obecny tymczasowy kod symbolu
Zwraca true, jezeli jakis znak zostal znaleziony, w przeciwnym wypadku false
*/
bool compareBuffer(listCodes_t **list, uchar *buf, FILE *stream, int compLevel, bool endingZero, int *currentBits, int *tempCode);

#endif