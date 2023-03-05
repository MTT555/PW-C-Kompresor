#ifndef COUNTCHARACTERS_H
#define COUNTCHARACTERS_H

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
void showList(count **head);
void sortTheList(count **head);
void swap(count *ptr1, count *ptr2);
#endif
