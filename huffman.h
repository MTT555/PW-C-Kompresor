#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include "utils.h"
#include <stdlib.h>
#include "countCharacters.h"

//lista do przechowywania kodow znakow
typedef struct codes {
    int character;
    char *code;
    struct codes *next;
} listCodes;

void huffman(FILE *input, FILE *output, int comp_level, bool cipher, count **head);

void print_huffmann_tree(listCodes **listC, FILE *stream);

void create_huffmann_tree(FILE *output, count **head, int *code, bool cipher, int comp_level, int top, listCodes **listC);

void addToTheList1(FILE *output, int comp_level, bool cipher, listCodes **head, int character, int *code, int length);

#endif //HUFFMAN_H
