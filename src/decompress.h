#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include "huffman.h"
#include "utils.h"
#include "dtree.h"
#include "utils.h"
#include "list.h"

void decompress(FILE *input, FILE *output);

void printList(listCodes_t **list, FILE *stream);

void addCode(listCodes_t **list, int character, uchar *code);

bool compareBuffer(listCodes_t **list, uchar *buf, FILE *stream, int compLevel, bool endingZero, int *currentBits, int *tempCode);

#endif