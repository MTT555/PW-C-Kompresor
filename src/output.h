#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdlib.h>
#include "utils.h"
#include "list.h"

/**
Funkcja wykonujaca zapis do pliku skompresowanego tekstu
    FILE *input - plik wejsciowy zawierajacy tekst do kompresji
    FILE *output - plik wyjsciowy, w ktorym zostanie zapisany skompresowany tekst
    settings_t s - ustawienia
    int *cipherPos - aktualna pozycja w szyfrze
    listCodes_t **head - glowa listy zawierajaca ilosci wystapien danych znakow
    uchar xor_start_value - poczatkowa wartosc bajtu do wykonania sumy kontrolnej
    pack_t *buffer - union pack uzyty wczesniej do zapisu slownika
    int packPos - pozycja ostatniego zajetego bitu w tym packu
*/
void compressedToFile(FILE *input, FILE *output, settings_t s, int *cipherPos, listCodes_t **head, uchar *xor, pack_t *buffer, int *pack_pos);

/**
Funkcja wykonujaca bitowy zapis znaku na podstawie union pack
    FILE *output - plik wyjsciowy
    settings s - ustawienia
    int *cipherPos - aktualna pozycja w szyfrze
    pack_t *buffer - union pack, nak torym wykonujemy operacje bitowe
    int *packPos - pozycja ostatniego zajetego bitu w tym union packu
    uchar *xor - aktualna wartosc sumy kontrolnej XOR
    int bit - wartosc bitu, ktora ma zostac nadana
*/
void saveBitIntoPack(FILE *output, settings_t s, int *cipherPos, pack_t *buffer, int *packPos, uchar *xor, int bit);

#endif