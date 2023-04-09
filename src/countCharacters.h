#ifndef COUNTCHARACTERS_H
#define COUNTCHARACTERS_H

#include <stdio.h>

/* Typ sluzacy do przechowywania ilosci wystapien poszczegolnych znakow */
typedef struct countChars {
    int character; /* int pozwoli na przechowywanie rowniez slow 12-bit i 16-bit */
    int amount; /* ilosc wystapien */
    struct countChars *next; /* nastepny element */
    struct countChars *left; /* lewy syn */
    struct countChars *right; /* prawy syn */
} count_t;

/**
Funkcja dodajaca slowo do listy na jej poczatek (jezeli jeszcze go nie ma)
	count_t **head - poczatek listy
	int character - slowo zapisane pod postacia liczby calkowitej
Zwraca wskaznik liste zawierajaca nowy element
*/
count_t *addToTheList(count_t **head, int character);

/**
Funkcja sprawdzajaca czy dany element jest juz w liscie
	count_t **head - poczatek listy
	int character - slowo zapisane pod postacia liczby calkowitej
Jezeli jest w liscie, zwieksza licznik wystapien o 1 i zwraca 0
Jezeli nie znaleziono, zwraca 1
*/
int checkIfOnTheList(count_t **head, int character);

/**
Funkcja drukujaca cala liste na wybrany strumien
	count_t **head - poczatek listy
	FILE *stream - strumien wyjscia
*/
void showList(count_t **head, FILE *stream);

/**
Funkcja sortujaca liste algorytmem sortowania babelkowego
	count_t **head - poczatek listy
*/
void sortTheCountList(count_t **head);

/**
Funkcja zamieniajaca ze soba wartosci dwoch danych elementow listy
	count_t *ptr1, *ptr2 - wskazniki na te elementy
*/
void swap(count_t *ptr1, count_t *ptr2);

/**
Funkcja zapisujaca wskazniki wszystkich elementow z listy
	count_t *head - poczatek listy
	count_t **frPtrs - tablica do ktorej zapisujemy wskazniki
	int *frPos - ilosc zapisanych juz wskaznikow
*/
void savePtrsFromList(count_t *head, count_t **frPtrs, int *frPos);

/**
Funkcja rekurencyjnie zapisujaca wskazniki do wszystkich synow danego elementu bez powtorzen
	count_t *head - poczatek listy
	count_t **frPtrs - tablica do ktorej zapisujemy wskazniki
	int *frPos - ilosc zapisanych juz wskaznikow
*/
void savePtrsRecursively(count_t *head, count_t **frPtrs, int *frPos);

/**
Zwalnia pamiec z listy characterow i ich kodow w tradycyjny sposob
	count_t *head - glowa listy, z ktorej chcemy zwolnic pamiec
*/
void freeList(count_t *head);

#endif
