#ifndef OUTPUT_H
#define OUTPUT_H

#include "utils.h"
#include "list.h"

/**
Funkcja wykonujaca zapis do pliku skompresowanego tekstu
    FILE *input - plik wejsciowy zawierajacy tekst do kompresji
    FILE *output - plik wyjsciowy, w ktorym zostanie zapisany skompresowany tekst
    int compLevel - poziom kompresji podany w bitach (brak obslugi przypadku braku kompresji)
    bool cipher - zmienna mowiaca, czy tekst ma zostac rowniez zaszyfrowany
    uchar *cipherKey - klucz szyfrowania (nieistotny, gdy cipher == false)
    listCodes_t **head - glowa listy zawierajaca ilosci wystapien danych znakow
    uchar xor_start_value - poczatkowa wartosc bajtu do wykonania sumy kontrolnej
    pack_t *buffer - union pack uzyty wczesniej do zapisu slownika
    int packPos - pozycja ostatniego zajetego bitu w tym packu
*/
void compressedToFile(FILE *input, FILE *output, int compLevel, bool cipher,
    uchar *cipher_key, listCodes_t **head, uchar *xor, pack_t *buffer, int *pack_pos);

/**
Funkcja wykonujaca bitowy zapis znaku na podstawie union pack
    FILE *output - plik wyjsciowy
    bool cipher - zmienna mowiaca, czy tekst ma zostac rowniez zaszyfrowany
    uchar *cipherKey - klucz szyfrowania (nieistotny, gdy cipher == false)
    pack_t *pack - union pack, nak torym wykonujemy operacje bitowe
    int *packPos - pozycja ostatniego zajetego bitu w tym union packu
    int bit - wartosc bitu, ktora ma zostac nadana
*/
void saveBitIntoPack(FILE *output, bool cipher, uchar *cipherKey, int *cipherPos,
    pack_t *buffer, int *packPos, uchar *xor, int bit);

#endif