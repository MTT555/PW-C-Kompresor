#include <stdio.h>
#include <stdlib.h>
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