#ifndef DTREE_H
#define DTREE_H

/**
Wyjasnienie nazewnictwa:
    dtree - drzewo pomocniczne do dekompresji
    dnode - wezel tego drzewa
*/

// Zmienna sluzaca do obslugi budowania pseudodrzewa na podstawie slownika zapisanego w pliku
typedef struct dnode {
    struct dnode *prev; // wskazuje na ojca
    struct dnode *left; // wskazuje na lewego syna
    struct dnode *right; // wskazuje na prawego syna
} dnode_t;

int goDown(dnode_t **head);

void freeDTree(dnode_t *head);

#endif // DTREE_H