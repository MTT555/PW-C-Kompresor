#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "bits_analyze.h"
#include "decompress.h"
#include "dtree.h"
#include "huffman.h"
#include "alloc.h"

bool analyzeBits(FILE *output, uchar c, flag_t f, listCodes_t **list, dnode_t **iterator,
mod_t *mode, buffer_t *buf, buffer_t *codeBuf, int *currentBits, int *tempCode) {
    int i, down;
    short bits = 0; /* ilosc przeanalizowanych bitow */
    short currentCode = 0; /* obecny kod przejscia w sciezce */
    while (bits != 8 - f.redundantBits) {
        switch(*mode) {
            case dictRoad: {
                currentCode = 2 * returnBit(c, bits) + returnBit(c, bits + 1);
                bits += 2;
                if(currentCode == 3) {
                    buf->pos = 0;
                    *mode = bitsToWords;
#ifdef DEBUG
                    /* wyswietlenie odczytanego slownika na stderr */
                    fprintf(stderr, "List of codes read from the dictionary:\n");
                    printListCodes(&*list, stderr);
#endif
                } else if(currentCode == 2) {
                    *iterator = (*iterator)->prev;
                    (codeBuf->pos)--; /* wyjscie o jeden w gore */
                } else if(currentCode == 1) {
                    down = goDown(iterator);
                    if(down == -1)
                        return false;
                    if(codeBuf->pos == codeBuf->curSize) { /* sprawdzenie, czy nie trzeba realokowac tablicy na wieksza */
                        if(!tryRealloc((void **)(&(codeBuf->buf)), 2 * (codeBuf->curSize) * sizeof(char)))
                            return false;
                        codeBuf->curSize *= 2;
                    }
                    codeBuf->buf[codeBuf->pos] = '0' + down; /* przejscie o jeden w dol */
                    (codeBuf->pos)++;
                    codeBuf->buf[codeBuf->pos] = '\0';
                    *mode = dictWord;
                } else if(currentCode == 0) {
                    down = goDown(iterator);
                    if(down == -1)
                        return false;
                    if(codeBuf->pos == codeBuf->curSize) { /* sprawdzenie, czy nie trzeba realokowac tablicy na wieksza */
                        if(!tryRealloc((void **)(&(codeBuf->buf)), 2 * (codeBuf->curSize) * sizeof(char)))
                            return false;
                        codeBuf->curSize *= 2;
                    }
                    codeBuf->buf[codeBuf->pos] = '0' + down; /* przejscie o jeden w dol */
                    (codeBuf->pos)++;
                }
                break;
            }
            case dictWord: {
                if(buf->pos == buf->curSize) { /* sprawdzenie, czy nie trzeba realokowac tablicy na wieksza */
                    if(!tryRealloc((void **)(&(buf->buf)), 2 * (buf->curSize) * sizeof(char)))
                        return false;
                    buf->curSize *= 2;
                }
                buf->buf[(buf->pos)++] = returnBit(c, bits++);
                buf->buf[(buf->pos)++] = returnBit(c, bits++);
                if(buf->pos == f.compLevel) {
                    int result = 0;
                    for(i = 0; i < f.compLevel; i++) {
                        result *= 2;
                        result += buf->buf[i];
                    }
                    addToListCodes(list, result, codeBuf->buf);
                    buf->pos = 0;
                    *iterator = (*iterator)->prev;
                    (codeBuf->pos)--;
                    *mode = dictRoad;
                }
                break;
            }
            case bitsToWords: {
                if(buf->pos == buf->curSize) { /* sprawdzenie, czy nie trzeba realokowac tablicy na wieksza */
                    if(!tryRealloc((void **)(&(buf->buf)), 2 * (buf->curSize) * sizeof(char)))
                        return false;
                    buf->curSize *= 2;
                }
                buf->buf[(buf->pos)++] = '0' + returnBit(c, bits);
                buf->buf[buf->pos] = '\0';
                bits++;
                if(compareBuffer(list, buf->buf, output, f.compLevel, (bits == 8 - f.redundantBits ? f.redundantZero : false), currentBits, tempCode))
                    buf->pos = 0;
                break;
            }
        }
    }
    return true;
}

short returnBit(uchar c, int x) {
    uchar ch = c;
    ch >>= (7 - x);
    return ch % 2;
}

bool compareBuffer(listCodes_t **list, uchar *buf, FILE *stream, int compLevel, bool redundantZero, int *currentBits, int *tempCode) {
    listCodes_t *iterator = (*list);
    uchar tempC;
    int temp = 0;
    while (iterator != NULL) {
        if(strcmp((char *)iterator->code, (char *)buf) == 0) {
            if(compLevel == 8) {
                tempC = (uchar)(iterator->character);
                fwrite(&tempC, sizeof(char), 1, stream);
            }
            else if(compLevel == 16) {
                tempC = (uchar)((iterator->character) / (1 << 8));
                fwrite(&tempC, sizeof(char), 1, stream);
                if(!redundantZero) {
                    tempC = (uchar)(iterator->character);
                    fwrite(&tempC, sizeof(char), 1, stream);
                }
            }
            else if(compLevel == 12) {
                *tempCode <<= 12;
                *tempCode += iterator->character;
                *currentBits += 12;
                if(*currentBits == 12) {
                    temp = *tempCode % 16;
                    *tempCode >>= 4;
                    tempC = (uchar)(*tempCode);
                    fwrite(&tempC, sizeof(char), 1, stream);
                    *tempCode = temp;
                    *currentBits = 4;
                } else {
                    tempC = (uchar)((*tempCode) / (1 << 8));
                    fwrite(&tempC, sizeof(char), 1, stream);
                    if(!redundantZero) {
                        tempC = (uchar)(*tempCode);
                        fwrite(&tempC, sizeof(char), 1, stream);
                    }
                    *tempCode = 0;
                    *currentBits = 0;
                }
            }
            return true;
        }
        iterator = iterator->next;
    }
    return false;
}

/**
Funkcja sprawdzajaca, czy aktualny fragment kodu w buforze odpowiada jakiejs literze
Jezeli tak, to zapisuje ta litere do podanego pliku
    listCodes_t **list - poczatek listy, ktora chcemy wyswietlic
    uchar *buf - bufor, ktory mozliwe, ze odpowiada jednej z liter
    FILE *stream - strumien, w ktorym ma zostac wydrukowana litera
    int compLevel - 
    bool endingZero - 
    int *currentBits - 
    int *tempCode - 
Zwraca true, jezeli jakis znak zostal znaleziony, w przeciwnym wypadku false
*/
bool compareBuffer(listCodes_t **list, uchar *buf, FILE *stream, int compLevel, bool endingZero, int *currentBits, int *tempCode);
