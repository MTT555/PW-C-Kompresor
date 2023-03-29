#ifndef DTREE_H
#define DTREE_H

/**
Wyjasnienie nazewnictwa:
    dtree - drzewo pomocniczne do dekompresji
    dnode - wezel tego drzewa
*/

/* Zmienna sluzaca do obslugi budowania pseudodrzewa na podstawie slownika zapisanego w pliku */
typedef struct dnode {
    struct dnode *prev; /* ojciec */
    struct dnode *left; /* lewy syn */
    struct dnode *right; /* prawy syn */
} dnode_t;

int goDown(dnode_t **head);

void freeDTree(dnode_t *head);

#endif