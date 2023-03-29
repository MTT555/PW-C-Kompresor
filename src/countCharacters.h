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

count_t *addToTheList(count_t **head, int character);

int checkIfOnTheList(count_t **head, int character);

void showList(count_t **head, FILE *stream);

void sortTheCountList(count_t **head);

void swap(count_t *ptr1, count_t *ptr2);

void freeList(count_t *head);

void freeRecursively(count_t *head);

#endif
