#include <stdio.h>
#include <stdlib.h>
#include "list.h"

/**
Funkcja drukujaca odczytany slownik na wybrany strumien
    listCodes_t **list - poczatek listy, ktora chcemy wyswietlic
    FILE *stream - strumien, w ktorym ta lista ma zostac wydrukowana
*/
void printListCodes(listCodes_t **list, FILE *stream) {
    listCodes_t *iterator = (*list);
    while (iterator != NULL) {
        fprintf(stream, "Character: %d, coded as: %s\n", iterator->character, iterator->code);
        iterator = iterator->next;
    }
}

void freeListCodes(listCodes_t **head) {
/* zwalnianie pamieci zajetej przez liste kodow */
	listCodes_t *iterator = *head, *temp;
	while(iterator != NULL) {
		temp = iterator;
		iterator = iterator->next;
        if(temp->code != NULL) {
            free(temp->code);
            temp->code = NULL;
        }
		free(temp);
        temp = NULL;
	}
}