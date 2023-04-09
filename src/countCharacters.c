#include <stdlib.h>
#include <stdio.h>
#include "countCharacters.h"
#include "alloc.h"

count_t *addToTheList(count_t **head, int character) {
	count_t *new = NULL;
	if(!tryMalloc((void **)&new, sizeof(count_t))) /* przy nieudanej probie alokacji */
		return NULL; /* zwracamy NULL */
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

void savePtrsFromList(count_t *head, count_t **frPtrs, int *frPos) {
	count_t *iterator = head, *temp = NULL;
	while (iterator != NULL) {
		temp = iterator;
		iterator = iterator->next;
		savePtrsRecursively(temp, frPtrs, frPos);
	}
}

void savePtrsRecursively(count_t *head, count_t **frPtrs, int *frPos) {
	int i;
	bool dontfree = false;
	if(head != NULL) { /* rekurencyjnie odpalamy funkcje na synach */
		if(head->left != NULL) {
			savePtrsRecursively(head->left, frPtrs, frPos);
			head->left = NULL;
		}
		if(head->right != NULL) {
			savePtrsRecursively(head->right, frPtrs, frPos);
			head->right = NULL;
		}

		for(i = 0; i < *frPos; i++)
			if(frPtrs[i] == head) { /* jezeli wskaznik juz zapisany w tablicy */
				dontfree = true; /* zapobiegamy jego ponownemu zapisowi */
				break;
			}
		if(!dontfree) { /* jezeli niezapisany to zapisujemy */
			frPtrs[*frPos] = head;
			(*frPos)++;
		}
	}
}

void freeList(count_t *head) {
	count_t *iterator = head, *temp = NULL;
	while (iterator != NULL) {
		temp = iterator;
		iterator = iterator->next;
		free(temp);
	}
}