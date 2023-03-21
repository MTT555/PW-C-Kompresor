#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include "huffman.h"
#include "utils.h"

// Zmienna przechowujaca aktualny tryb odczytu pliku do dekompresji
typedef enum {
    dictRoad, // slownik - odczytywanie drogi
    dictWord, // slownik - odczytywanie slowa
    bitsToWords // przeksztalcanie skompresowanych bitow na finalny tekst
} mod_t;

// Zmienna sluzaca do obslugi budowania pseudodrzewa na podstawie slownika zapisanego w pliku
typedef struct dnode {
    struct dnode *prev; // wskazuje na ojca
    struct dnode *left; // wskazuje na lewego syna
    struct dnode *right; // wskazuje na prawego syna
} dnode_t;

void decompress(FILE *input, FILE *output);

void analyzeBits(FILE *output, char c, int comp_level, listCodes **list, short ending);

short returnBit(char c, int x);

void freeDnodeTree(dnode_t *head);

int goDown(dnode_t **head);

void printList(listCodes **list, FILE *stream);

void addCode(listCodes **list, int character, char *code);

bool compareBuffer(listCodes **list, char *buf, FILE *stream, int comp_level);

#endif // DECOMPRESS_H