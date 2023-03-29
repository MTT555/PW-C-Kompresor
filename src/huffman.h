#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include "countCharacters.h"
#include "utils.h"
#include "list.h"

void huffman(FILE *input, FILE *output, int compLevel, bool cipher, count_t **head);

void createHuffmanTree(FILE *output, count_t **head, int *code, bool cipher, int compLevel, int top, listCodes_t **listC);

void addToTheList1(FILE *output, int compLevel, bool cipher, listCodes_t **head, int character, int *code, int length);

#endif
