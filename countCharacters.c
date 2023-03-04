#include "countCharacters.h"
#include <stdlib.h>
#include <stdio.h>
void runCounter(count **head){

(*head) = malloc(sizeof(count));
(*head) = NULL;

}
void addToTheList(count **head, char character){
//dodaje znak do listy jezeli jeszcze go nie ma
count *new;
new = malloc(sizeof(count));
new->character = character;
new->amount = 1;
new->next = (*head);
(*head) = new;

}

int checkIfElementIsOnTheList(count **head, char character){
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
void showList(count **head){

count *iterator = (*head);
while (iterator != NULL){

printf("Character: %c, Code: %d, Amount: %d\n",iterator->character, iterator->character,iterator->amount);
iterator = iterator->next;
}


}
