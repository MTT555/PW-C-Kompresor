#ifndef NOCOMPRESS_H
#define NOCOMPRESS_H

#include "utils.h"

/**
Funkcja, ktora przepisuje jeden plik symbol po symbolu do drugiego
    FILE *input - plik wejsciowy
    FILE *input - plik wyjsciowy
    int n - ilosc znakow do przepisania
    settings_t s - ustawienia
*/
void rewriteFile(FILE *input, FILE *output, int n, settings_t s);

#endif