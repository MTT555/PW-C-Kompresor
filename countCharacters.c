#include "countCharacters.h"
#include <stdlib.h>
#include <stdio.h>

/**
Funkcja dodajaca slowo do listy na jej poczatek (jezeli jeszcze go nie ma)
	count_t **head - poczatek listy
	int character - slowo zapisane pod postacia liczby calkowitej
Zwraca wskaznik liste zawierajaca nowy element
*/
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

/**
Funkcja sprawdzajaca czy dany element jest juz w liscie
	count_t **head - poczatek listy
	int character - slowo zapisane pod postacia liczby calkowitej
Jezeli jest w liscie, zwieksza licznik wystapien o 1 i zwraca 0
Jezeli nie znaleziono, zwraca 1
*/
int checkIfElementIsOnTheList(count_t **head, int character) {
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

/**
Funkcja drukujaca cala liste na wybrany strumien
	count_t **head - poczatek listy
	FILE *stream - strumien wyjscia
*/
void showList(count_t **head, FILE *stream){
	count_t *iterator = (*head);
	while (iterator != NULL) {
		fprintf(stream, "Character: %d, Amount: %d\n", iterator->character, iterator->amount);
		iterator = iterator->next;
	}
}

/**
Funkcja sortujaca liste algorytmem sortowania babelkowego
	count_t **head - poczatek listy
*/
void sortTheList(count_t **head){
	if(*head == NULL)
		return;
	
	int ifSwapped;
	count_t *ptr1 = NULL, *ptr2 = NULL;

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

/**
Funkcja zamieniajaca ze soba wartosci dwoch danych elementow listy
	count_t *ptr1, *ptr2 - wskazniki na te elementy
*/
void swap(count_t *ptr1, count_t *ptr2) {
	int temp_a = ptr1->amount;
	ptr1->amount = ptr2->amount;
	ptr2->amount = temp_a;
	int temp_c = ptr1->character;
	ptr1->character = ptr2->character;
	ptr2->character = temp_c;
}	

/**
Funkcja zwalniajaca pamiec z wszystkich elementow listy
	count_t **head - poczatek listy
*/
void freeList(count_t *head) {
	count_t *iterator = head, *temp;
	while (iterator != NULL) {
		temp = iterator;
		iterator = iterator->next;
		freeRecursively(temp);
	}
}

/**
Funkcja rekurencyjnie zwalniajaca pamiec z wszystkich synow danego elementu
	count_t *head - dany element
*/
void freeRecursively(count_t *head) {
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
		head = NULL;
	}
}