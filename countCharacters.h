#ifndef COUNTCHARACTERS_H
#define COUNTCHARACTERS_H

#include <stdio.h>

typedef struct countCharacters{
    char character;
    int amount;
    struct countCharacters *next;
    struct countCharacters *left; // lewy syn
    struct countCharacters *right; // prawy syn
} count;


void runCounter(count **head);
void addToTheList(count **head, char character);
int checkIfElementIsOnTheList(count **head, char character);
void showList(count **head, FILE *stream);
void sortTheList(count **head);
void swap(count *ptr1, count *ptr2);
void freeList(count **head);

#endif
