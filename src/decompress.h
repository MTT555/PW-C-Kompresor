#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include "huffman.h"
#include "utils.h"
#include "dtree.h"

void decompress(FILE *input, FILE *output);

void printList(listCodes **list, FILE *stream);

void addCode(listCodes **list, int character, unsigned char *code);

bool compareBuffer(listCodes **list, unsigned char *buf, FILE *stream, int compLevel, bool endingZero, int *currentBits, int *tempCode);

#endif // DECOMPRESS_H