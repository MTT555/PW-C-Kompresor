#include <stdio.h>
#include <stdlib.h>
#include "dtree.h"
#include "alloc.h"

int goDown(dnode_t **head) {
    if((*head)->left == NULL) { /* jezeli lewy wezel jest wolny */
        if(!tryMalloc((void **)(&((*head)->left)), sizeof(dnode_t)))
            return -1;
        (*head)->left->prev = (*head); /* zapisanie poprzedniego wezla */
        (*head)->left->left = NULL;
        (*head)->left->right = NULL;
        (*head) = (*head)->left;
        return 0;
    } else { /* w przeciwnym razie zajmujemy prawy wezel */
        if(!tryMalloc((void **)(&((*head)->right)), sizeof(dnode_t)))
            return -1;
        (*head)->right->prev = (*head); /* zapisanie poprzedniego wezla */
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