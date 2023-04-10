#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include "huffman.h"
#include "utils.h"
#include "dtree.h"
#include "utils.h"
#include "list.h"

/* flag_t - typ sluzacy do przechowywania ustawien odczytanych z pliku do dekompresji */
typedef struct {
    int compLevel; /* poziom kompresji */
    bool cipher; /* koniecznosc odszyfrowania */
    bool redundantZero; /* koniecznosc odlaczenia nadmiarowego koncowego znaku '\0' */
    int redundantBits; /* ilosc nadmiarowych bitow do odrzucenia */
} flag_t;

/**
Funkcja dekompresujaca dany plik pochodzacy z tego kompresora
    FILE *input - plik wejsciowy
    FILE *output - plik wyjsciowy
    settings_t s - ustawienia
Zwraca
    0 - dekompresja sie powiodla
    1 - blad alokacji pamieci
    2 - zly szyfr uniemozliwil pomyslna dekompresje
*/
int decompress(FILE *input, FILE *output, settings_t s);

#endif