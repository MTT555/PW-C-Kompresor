#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include "countCharacters.h"
#include "utils.h"
#include "list.h"

/**
Funkcja wykonujaca kompresje algorytmem Huffmana
    FILE *input - plik wejsciowy zawierajacy tekst do kompresji
    FILE *output - plik wyjsciowy, w ktorym zostanie zapisany skompresowany tekst
    int compLevel - poziom kompresji podany w bitach (dla compLevel == 0 - brak kompresji)
    bool cipher - zmienna mowiaca, czy tekst ma zostac rowniez zaszyfrowany
    uchar *cipherKey - klucz szyfrowania (nieistotny, gdy cipher == false)
    count_t **head - glowa listy zawierajaca ilosci wystapien danych znakow
Zwraca
    true - kompresja sie powiodla
    false - nastapily problemy z pamiecia
*/
bool huffman(FILE *input, FILE *output, settings_t s, count_t **head);

/**
Funkcja rekurencyjna odpowiedzialna za budowe drzewa Huffmana
    args
Zwraca
    true - tworzenie danego wierzcholka drzewa sie powiodlo
    false - nastapily problemy z pamiecia
*/
bool createHuffmanTree(FILE *output, count_t **head, int *code, settings_t s, uchar *xor, int top,
    listCodes_t **listC, pack_t *buffer, int *packPos, int *cipherPos, buffer_t *roadBuf);

/**
Funkcja realizujaca dodanie danego kodu do slownika kodow oraz odpowiedni zapis tego faktu do pliku
    args
Zwraca
    true - dodanie do listy sie powiodlo
    false - nastapily problemy z pamiecia
*/
bool addToTheListCodes(FILE *output, settings_t s, listCodes_t **listC, pack_t *buffer, int *packPos,
    int character, int *code, int length, uchar *xor, int *cipherPos, buffer_t *roadBuf);

#endif
