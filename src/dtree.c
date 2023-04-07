#include <stdio.h>
#include <stdlib.h>
#include "dtree.h"

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

void freeDTree(dnode_t *head) {
    if(head->left != NULL)
        freeDTree(head->left);
    if(head->right != NULL)
        freeDTree(head->right);
    free(head);
}