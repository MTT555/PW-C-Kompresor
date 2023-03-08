#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include "utils.h"
#include <stdlib.h>
#include "countCharacters.h"

//lista do przechowywania kodow znakow
typedef struct codes {
char character;
char *code;
struct codes *next;
} listCodes;

void huffman(FILE *input, FILE *output, int comp_level, bool cipher, char *cipher_key,count **head);
void print_huffmann_tree(listCodes **listC);
void create_huffmann_tree(count **head, int *code, int comp_level, int top,listCodes **listC);
void addToTheList1(listCodes **head, char character,int *code, int length);
void writeCompressedToFile(FILE *input, FILE *output, int comp_level, listCodes **head);
#endif //HUFFMAN_H
