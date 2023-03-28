#include <stdio.h>
#include <stdlib.h>
#include "dtree.h"

/**
Funkcja wykonujaca przejscie w dol w drzewie dnode
    dnode_t *head - punkt wzgledem ktorego chcemy wykonac przejscie
Zwraca:
    0 - wykonano przejscie w dol w lewo
    1 - wykonano przejscie w dol w prawo
*/
int goDown(dnode_t **head) {
    if((*head)->left == NULL) {
        (*head)->left = malloc(sizeof(dnode_t));
        (*head)->left->prev = (*head);
        (*head)->left->left = NULL;
        (*head)->left->right = NULL;
        (*head) = (*head)->left;
        return 0;
    } else {
        (*head)->right = malloc(sizeof(dnode_t));
        (*head)->right->prev = (*head);
        (*head)->right->left = NULL;
        (*head)->right->right = NULL;
        (*head) = (*head)->right;
        return 1;
    }
}

/**
Funkcja czyszczaca pamiec po dtree
    dnode_t *head - korzen drzewa dnode
*/
void freeDTree(dnode_t *head) {
    if(head->left != NULL)
        freeDTree(head->left);
    if(head->right != NULL)
        freeDTree(head->right);
    free(head);
}