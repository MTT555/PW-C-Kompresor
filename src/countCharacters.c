#include <stdlib.h>
#include <stdio.h>
#include "countCharacters.h"

count_t *addToTheList(count_t **head, int character) {
	count_t *new = NULL;
	new = malloc(sizeof(count_t));
	new->character = character;
	new->amount = 1;
	new->next = (*head);
	new->left = NULL;
	new->right = NULL;
	return new;
}

int checkIfOnTheList(count_t **head, int character) {
	count_t *iterator = (*head);
	while(iterator != NULL) {
		if(iterator->character == character) {
			iterator->amount++;      
			return 0;
		}
		iterator = iterator->next;
	}
	return 1;
}

void showList(count_t **head, FILE *stream) {
	count_t *iterator = (*head);
	while (iterator != NULL) {
		fprintf(stream, "Character: %d, Amount: %d\n", iterator->character, iterator->amount);
		iterator = iterator->next;
	}
}

void sortTheCountList(count_t **head) {
	int ifSwapped;
	count_t *ptr1 = NULL, *ptr2 = NULL;
	
	if(*head == NULL) /* jezeli lista jest pusta, konczymy dzialanie funkcji */
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

void swap(count_t *ptr1, count_t *ptr2) {
	int temp_a = ptr1->amount;
	int temp_c = ptr1->character;
	ptr1->amount = ptr2->amount;
	ptr2->amount = temp_a;
	ptr1->character = ptr2->character;
	ptr2->character = temp_c;
}	

/**
Funkcja zwalniajaca pamiec z wszystkich elementow listy
	count_t **head - poczatek listy
*/
void freeList(count_t *head) {
	int i, n = 0;
	count_t **toFree = malloc(65535 * sizeof(count_t)); /*uzaleznic od compLevel*/
	count_t *iterator = head;
	while (iterator != NULL) {
		toFree[n] = iterator;
		iterator = iterator->next;
		n++;
	}
	for(i = 0; i < n; i++) {
		freeRecursively(toFree[i]);
	}
	free(toFree);
}

/**
Funkcja rekurencyjnie zwalniajaca pamiec z wszystkich synow danego elementu
	count_t *head - dany element
*/
void freeRecursively(count_t *head) {
	if(head != NULL) {
		if(head->left != NULL) {
			free(head->left);
			head->left = NULL;
		}
		if(head->right != NULL) {
			free(head->right);
			head->right = NULL;
		}
	}
}