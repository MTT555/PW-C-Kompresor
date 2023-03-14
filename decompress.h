#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include <stdlib.h>

// Zmienna przechowujaca aktualny tryb odczytu pliku do dekompresji
typedef enum {
    dictRoad, // slownik - odczytywanie drogi
    dictWord, // slownik - odczytywanie slowa
    bitsToWords // przeksztalcanie skompresowanych bitow na finalny tekst
} mode_t;

void decompress(FILE *input, FILE *output);

#endif // DECOMPRESS_H