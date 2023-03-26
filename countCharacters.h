#ifndef COUNTCHARACTERS_H
#define COUNTCHARACTERS_H

#include <stdio.h>

typedef struct countCharacters {
    int character; // int pozwoli na przechowywanie rowniez kodow 12-bit i 16-bit
    int amount;
    struct countCharacters *next;
    struct countCharacters *left; // lewy syn
    struct countCharacters *right; // prawy syn
} count;


void addToTheList(count **head, int character);

int checkIfElementIsOnTheList(count **head, int character);

void showList(count **head, FILE *stream);

void sortTheList(count **head);

void swap(count *ptr1, count *ptr2);

void freeList(count **head);

#endif
