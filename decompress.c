#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "decompress.h"

static char cipher_key[] = "Politechnika_Warszawska"; // klucz szyfrowania
static char *buffer = NULL;
static int buf_pos = 0; // aktualna pozycja w buforze
static int cur_buf_size = 512; // aktualna wielkosc buforu
static mode_t mode = dictRoad; // zmienna przechowujaca aktualny tryb czytania pliku

/**
Funkcja dekompresujaca dany plik pochodzacy z tego kompresora
    FILE *input - plik wejsciowy
    FILE *output - plik wyjsciowy
*/
void decompress(FILE *input, FILE *output) {
    int i;
    buffer = malloc(512 * sizeof(char)); // alokacja pamieci na bufor

    // odczytanie, gdzie jest koniec pliku
    fseek(input, 0, SEEK_END);
    int end_pos = ftell(input);
    fseek(input, 0 , SEEK_SET);
    
    pack_t pack; // deklaracja union packa
    pack.whole = 0; // i jego czyszczenie

    fseek(input, 2, SEEK_SET); // ustawienie kursora na trzeci znak w celu odczytania flag
    char c = fgetc(input);
    int comp_level = ((c & 0b11000000) >> 6) * 4 + 4; // odczytanie poziomu kompresji
    int cipher = (c & 0b00100000) >> 5; // odczytanie szyfrowania
    int ending = c & 0b00000111; // odczytanie ilosci bitow konczacych
#ifdef DEBUG
    // wyswietlenie odczytanych wartosci na stderr
    fprintf(stderr, "The following values have been read:\n"
        "Compression level: %d, encrypted: %s, ending bits: %d\n", comp_level, cipher ? "true" : "false", ending);
#endif
    fseek(input, 4, SEEK_SET);
    for(i = 4; i < end_pos; i++)
        analyzeBits(fgetc(input));
}

void analyzeBits(char c) {
    short bits = 0; // ilosc przeanalizowanych bitow
    short cur_bit = 0; // wartosc obecnie analizowanego bitu
    short cur_code = 0; // obecny kod przejscia w sciezce
    while (bits != 8) {
        switch(mode) {
            case dictRoad: {
                bits += 2;
                break;
            }
            case dictWord: {
                break;
            }
            case bitsToWords: {
                break;
            }
        }
    }
}