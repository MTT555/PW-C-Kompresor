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

/**
Funkcja, ktora dokonuje jedynie odszyfrowania pliku wedlug podanego szyfru
    FILE *input - plik wejsciowy
    FILE *input - plik wyjsciowy
    int n - ilosc znakow do przepisania
    uchar *cipherKey - klucz szyfrowania
*/
void decryptFile(FILE *input, FILE *output, int n, uchar *cipherKey);

#endif