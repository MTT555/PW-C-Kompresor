#include "countCharacters.h"
#include <stdlib.h>
#include <stdio.h>

count *addToTheList(count **head, int character) {
//dodaje znak do listy jezeli jeszcze go nie ma
	count *new;
	new = malloc(sizeof(count));
	new->character = character;
	new->amount = 1;
	new->next = (*head);
	new->left = NULL;
	new->right = NULL;
	fprintf(stderr, "added %d %d ||", new->character, new->amount);
	return new;
}

int checkIfElementIsOnTheList(count **head, int character) {
//jezeli element jest na liscie, zwroc 0 w przeciwnym razie zwroc 1
	count *iterator = (*head);
	while(iterator != NULL) {
		if(iterator->character == character) {
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

void freeList(count *head) {
	count *iterator = head, *temp;
	while (iterator != NULL) {
		temp = iterator;
		iterator = iterator->next;
		freeRecursively(temp);
	}
}

void freeRecursively(count *head) {
	if(head != NULL) {
		if(head->left != NULL) {
			freeRecursively(head->left);
			head->left = NULL;
		}
		if(head->right != NULL) {
			freeRecursively(head->right);
			head->right = NULL;
		}
		
		free(head);
		head->right = NULL;
	}
}