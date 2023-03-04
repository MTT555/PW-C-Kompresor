#ifndef COUNTCHARACTERS_H
#define COUNTCHARACTERS_H

typedef struct countCharacters{

char character;
int amount;
struct countCharacters *next;
} count;


void runCounter(count **head);
void addToTheList(count **head, char character);
int checkIfElementIsOnTheList(count **head, char character);
void showList(count **head);
#endif
