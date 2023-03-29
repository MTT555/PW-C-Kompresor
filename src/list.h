#ifndef LIST_H
#define LIST_H

#include "utils.h"

/* Lista do przechowywania kodow znakow */
typedef struct codes {
    int character;
    uchar *code;
    struct codes *next;
} listCodes_t;

void printListCodes(listCodes_t **list, FILE *stream);

void freeListCodes(listCodes_t **listC);

#endif