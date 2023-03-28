#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include "utils.h"
#include <stdlib.h>
#include "countCharacters.h"

//lista do przechowywania kodow znakow
typedef struct codes {
    int character;
    unsigned char *code;
    struct codes *next;
} listCodes;

void huffman(FILE *input, FILE *output, int compLevel, bool cipher, count_t **head);

void print_huffmann_tree(listCodes **listC, FILE *stream);

void create_huffmann_tree(FILE *output, count_t **head, int *code, bool cipher, int compLevel, int top, listCodes **listC);

void addToTheList1(FILE *output, int compLevel, bool cipher, listCodes **head, int character, int *code, int length);

void freeListCodes(listCodes **listC);

#endif //HUFFMAN_H