#ifndef COUNTCHARACTERS_H
#define COUNTCHARACTERS_H

#include <stdio.h>

typedef struct countChars {
    int character; // int pozwoli na przechowywanie rowniez kodow 12-bit i 16-bit
    int amount;
    struct countChars *next;
    struct countChars *left; // lewy syn
    struct countChars *right; // prawy syn
} count_t;

count_t *addToTheList(count_t **head, int character);

int checkIfElementIsOnTheList(count_t **head, int character);

void showList(count_t **head, FILE *stream);

void sortTheList(count_t **head);

void swap(count_t *ptr1, count_t *ptr2);

void freeList(count_t *head);

void freeRecursively(count_t *head);

#endif
