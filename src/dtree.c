#include <stdio.h>
#include <stdlib.h>
#include "dtree.h"
#include "alloc.h"

int goDown(dnode_t **head) {
    if((*head)->left == NULL) {
        if(!tryMalloc((void **)(&((*head)->left)), sizeof(dnode_t)))
            return -1;
        (*head)->left->prev = (*head);
        (*head)->left->left = NULL;
        (*head)->left->right = NULL;
        (*head) = (*head)->left;
        return 0;
    } else {
        if(!tryMalloc((void **)(&((*head)->right)), sizeof(dnode_t)))
            return -1;
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