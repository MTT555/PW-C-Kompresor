#ifndef LIST_H
#define LIST_H

#include "utils.h"

/* Lista do przechowywania kodow znakow */
typedef struct codes {
    int character; /* znak jako liczba calkowita */
    uchar *code; /* kod odpowiadajacy temu znaku */
    struct codes *next; /* wskaznik na nastepny element */
} listCodes_t;

/**
Funkcja dodajaca odczytany kod wraz ze znakiem do listy
    listCodes_t **list - lista, do ktorej chcemy dokonac zapisu
    int character - znak, ktory chcemy zapisac
    uchar *code - kod tego znaku
Zwraca
    true - dodanie do listy sie powiodlo
    false - nastapily problemy z pamiecia
*/
bool addToListCodes(listCodes_t **list, int character, uchar *code);

/**
Funkcja drukujaca odczytany slownik na wybrany strumien
    listCodes_t **list - poczatek listy, ktora chcemy wyswietlic
    FILE *stream - strumien, w ktorym ta lista ma zostac wydrukowana
*/
void printListCodes(listCodes_t **list, FILE *stream);

/**
Funkcja zwalniajaca pamiec zajeta przez liste kodow
    listCodes_t **list - poczatek listy, ktora chcemy zwolnic
*/
void freeListCodes(listCodes_t **listC);

#endif