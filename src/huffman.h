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
    FILE *output - plik wyjsciowy
    count_t **head - glowa listy zawierajaca ilosci wystapien danych znakow
    int *code - tablica zawierajaca kod danego znaku
    settings_t s - ustawienia
    uchar *xor - zmienna odpowiedzialna za sume kontrolna XOR
    int top - poziom
    listCodes_t **listC - glowa listy zawierajacej znaki i ich kody
    pack_t *buffer - union pack stanowiacy bufor dla bitow
    int *packPos - aktualna pozycja w tym buforze
    int *cipherPos - aktualna pozycja w szyfrze
    buffer_t *roadBuf - bufor dla drogi po drzewie zapisywanej do pliku
Zwraca
    true - tworzenie danego wierzcholka drzewa sie powiodlo
    false - nastapily problemy z pamiecia
*/
bool createHuffmanTree(FILE *output, count_t **head, int *code, settings_t s, uchar *xor, int top,
    listCodes_t **listC, pack_t *buffer, int *packPos, int *cipherPos, buffer_t *roadBuf);

/**
Funkcja realizujaca dodanie danego kodu do slownika kodow oraz odpowiedni zapis tego faktu do pliku
    FILE *output - plik wyjsciowy
    settings_t s - ustawienia
    listCodes_t **listC - glowa listy zawierajacej znaki i ich kody
    pack_t *buffer - union pack stanowiacy bufor dla bitow
    int *packPos - aktualna pozycja w tym buforze
    int character - znak, ktory chcemy dodac
    const int *code - kod tego znaku
    int length - dlugosc tego kodu
    uchar *xor - zmienna odpowiedzialna za sume kontrolna XOR
    int *cipherPos - aktualna pozycja w szyfrze
    buffer_t *roadBuf - bufor dla drogi po drzewie zapisywanej do pliku
Zwraca
    true - dodanie do listy sie powiodlo
    false - nastapily problemy z pamiecia
*/
bool addToTheListCodes(FILE *output, settings_t s, listCodes_t **listC, pack_t *buffer, int *packPos,
    int character, const int *code, int length, uchar *xor, int *cipherPos, buffer_t *roadBuf);

#endif
