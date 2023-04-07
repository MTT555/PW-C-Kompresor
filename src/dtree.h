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

/**
Funkcja wykonujaca przejscie w dol w drzewie dnode
    dnode_t *head - punkt wzgledem ktorego chcemy wykonac przejscie
Zwraca:
    0 - wykonano przejscie w dol w lewo
    1 - wykonano przejscie w dol w prawo
*/
int goDown(dnode_t **head);

/**
Funkcja czyszczaca pamiec po dtree
    dnode_t *head - korzen drzewa dnode
*/
void freeDTree(dnode_t *head);

#endif