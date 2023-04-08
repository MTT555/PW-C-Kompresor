#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "alloc.h"

bool addToListCodes(listCodes_t **list, int character, uchar *code) {
    listCodes_t *new = NULL;
    if(!tryMalloc((void **)&new, sizeof(listCodes_t))) /* przy nieudanej probie alokacji */
		return true; /* zwracamy NULL */
    new->character = character;
    new->code = NULL;
    if(!tryMalloc((void **)(&(new->code)), sizeof(char) * (strlen((char *)code) + 1)))
        return false;
    strcpy((char *)new->code, (char *)code);
    new->next = (*list);
    (*list) = new;
    return true;
}

void printListCodes(listCodes_t **list, FILE *stream) {
    listCodes_t *iterator = (*list);
    while (iterator != NULL) {
        fprintf(stream, "Character: %d, coded as: %s\n", iterator->character, iterator->code);
        iterator = iterator->next;
    }
}

void freeListCodes(listCodes_t **head) {
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