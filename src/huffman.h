#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include "countCharacters.h"
#include "utils.h"
#include "list.h"

void huffman(FILE *input, FILE *output, int compLevel, bool cipher, count_t **head);

void createHuffmanTree(FILE *output, count_t **head, int *code, bool cipher, int compLevel, uchar *xor,
    int top, listCodes_t **listC, pack_t *buffer, int *packPos, uchar *cipherKey, int *cipherPos, uchar *roadBuffer, int *roadPos);

void addToTheListCodes(FILE *output, int compLevel, bool cipher, listCodes_t **listC, pack_t *buffer, int *packPos,
    int character, int *code, int length, uchar *xor, uchar *cipherKey, int *cipherPos, uchar *roadBuffer, int *roadPos);

#endif
