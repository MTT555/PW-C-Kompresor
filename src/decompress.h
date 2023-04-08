#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include "huffman.h"
#include "utils.h"
#include "dtree.h"
#include "utils.h"
#include "list.h"

/**
Funkcja dekompresujaca dany plik pochodzacy z tego kompresora
    FILE *input - plik wejsciowy
    FILE *output - plik wyjsciowy
Zwraca
    true - dekompresja sie powiodla
    false - nastapily problemy z pamiecia
*/
bool decompress(FILE *input, FILE *output);

/**
Funkcja dodajaca odczytany kod wraz ze znakiem do listy
    listCodes_t **list - lista, do ktorej chcemy dokonac zapisu
    int character - znak, ktory chcemy zapisac
    uchar *code - kod tego znaku
*/
void addtoListCodes(listCodes_t **list, int character, uchar *code);

/**
Funkcja sprawdzajaca, czy aktualny fragment kodu w buforze odpowiada jakiejs literze
Jezeli tak, to zapisuje ta litere do podanego pliku
    listCodes_t **list - poczatek listy, ktora chcemy wyswietlic
    uchar *buf - bufor, ktory mozliwe, ze odpowiada jednej z liter
    FILE *stream - strumien, w ktorym ma zostac wydrukowana litera
Zwraca true, jezeli jakis znak zostal znaleziony, w przeciwnym wypadku false
*/
bool compareBuffer(listCodes_t **list, uchar *buf, FILE *stream, int compLevel, bool endingZero, int *currentBits, int *tempCode);

#endif