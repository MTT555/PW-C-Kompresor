#include "countCharacters.h"
#include <stdlib.h>
#include <stdio.h>

void addToTheList(count **head, int character){
//dodaje znak do listy jezeli jeszcze go nie ma
	count *new;
	new = malloc(sizeof(count));
	new->character = character;
	new->amount = 1;
	new->next = (*head);
	fprintf(stderr, "added %c %d ||", new->character, new->amount);
	(*head) = new;
}

int checkIfElementIsOnTheList(count **head, int character) {
//jezeli element jest na liscie, zwroc 0 w przeciwnym razie zwroc 1
	count *iterator = (*head);
	while(iterator != NULL){
		if(iterator->character == character){
		iterator->amount++;      
			return 0;
		}
		iterator = iterator->next;
	}
	return 1;
}

void showList(count **head, FILE *stream){
	count *iterator = (*head);
	while (iterator != NULL) {
		fprintf(stream, "Character: %d, Amount: %d\n", iterator->character, iterator->amount);
		iterator = iterator->next;
	}
}

void sortTheList(count **head){
//sortuje liste niemalejaco algorytmem sortowania babelkowego
	int ifSwapped;
	count *ptr1;
	count *ptr2 = NULL;
	if(*head == NULL)
		return;
	
	do {
		ifSwapped = 0;
		ptr1 = (*head);
		while(ptr1->next != ptr2) {
			if(ptr1->amount > ptr1->next->amount) {
				swap(ptr1, ptr1->next);
				ifSwapped = 1;
			}
			ptr1 = ptr1->next;
		}
		ptr2 = ptr1;
	} while(ifSwapped);
}

void swap(count *ptr1, count *ptr2){
	int temp = ptr1->amount;
	ptr1->amount = ptr2->amount;
	ptr2->amount = temp;
	int temp_c = ptr1->character;
	ptr1->character = ptr2->character;
	ptr2->character = temp_c;
}	

void freeList(count **head) {
// zwalnianie pamieci zajetej przez liste
	count **current = &head;
	fprintf(stderr, "c%p ", current);
    count **next;

    while (current != NULL) {
        next = (*current)->next;
		fprintf(stderr, "freed %c %d || ", (*current)->character, (*current)->next);
        free(*current);
        *current = next;
    }

    *head = NULL; // ustawiamy wskaznik na poczatek listy na NULL
}