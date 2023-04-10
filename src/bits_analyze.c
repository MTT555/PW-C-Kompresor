#include <stdio.h>
#include <string.h>
#include "list.h"
#include "bits_analyze.h"
#include "decompress.h"
#include "dtree.h"
#include "alloc.h"

int analyzeBits(FILE *output, uchar c, flag_t f, listCodes_t **list, dnode_t **iterator,
mod_t *mode, buffer_t *buf, buffer_t *codeBuf, int *currentBits, int *tempCode) {
    int i, down;
    int bits = 0; /* ilosc przeanalizowanych bitow */
    int currentCode; /* obecny kod przejscia w sciezce */
    while (bits != 8 - f.redundantBits) { /* f.redundantBits bedzie != 0 jedynie przy ostatnim analizowanym znaku */
        if(buf->pos > 100000 || codeBuf->pos > 100000) /* zapobieganie przepelnianiu pamieci w momencie podania zlego szyfru do odszyfrowania */
            return 2;
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
                        return 1;
                    if(codeBuf->curSize - codeBuf->pos <= 1) { /* sprawdzenie, czy nie trzeba realokowac tablicy na wieksza */
                        if(!tryRealloc((void **)(&(codeBuf->buf)), 2 * (codeBuf->curSize) * sizeof(char)))
                            return 1;
                        codeBuf->curSize *= 2;
                    }
                    codeBuf->buf[codeBuf->pos] = '0' + down; /* przejscie o jeden w dol */
                    (codeBuf->pos)++;
                    codeBuf->buf[codeBuf->pos] = '\0';
                    *mode = dictWord;
                } else if(currentCode == 0) {
                    down = goDown(iterator);
                    if(down == -1)
                        return 1;
                    if(codeBuf->curSize - codeBuf->pos <= 1) { /* sprawdzenie, czy nie trzeba realokowac tablicy na wieksza */
                        if(!tryRealloc((void **)(&(codeBuf->buf)), 2 * (codeBuf->curSize) * sizeof(char)))
                            return 1;
                        codeBuf->curSize *= 2;
                    }
                    codeBuf->buf[codeBuf->pos] = '0' + down; /* przejscie o jeden w dol */
                    (codeBuf->pos)++;
                }
                break;
            }
            case dictWord: {
                if(buf->curSize - buf->pos <= 1) { /* sprawdzenie, czy nie trzeba realokowac tablicy na wieksza */
                    if(!tryRealloc((void **)(&(buf->buf)), 2 * (buf->curSize) * sizeof(char)))
                        return 1;
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
                if(buf->curSize - buf->pos <= 1) { /* sprawdzenie, czy nie trzeba realokowac tablicy na wieksza */
                    if(!tryRealloc((void **)(&(buf->buf)), 2 * (buf->curSize) * sizeof(char)))
                        return 1;
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
    return 0;
}

int returnBit(uchar c, int x) {
    uchar ch = c;
    ch >>= (7 - x);
    return ch % 2;
}

bool compareBuffer(listCodes_t **list, uchar *buf, FILE *stream, int compLevel, bool redundantZero, int *currentBits, int *tempCode) {
    listCodes_t *iterator = (*list);
    uchar tempC;
    int temp;
    while (iterator != NULL) {
        if(strcmp((char *)iterator->code, (char *)buf) == 0) {
            if(compLevel == 8) { /* dla kompresji 8-bit po prostu piszemy symbol */
                tempC = (uchar)(iterator->character);
                fwrite(&tempC, sizeof(char), 1, stream);
            }
            else if(compLevel == 16) { /* dla kompresji 16-bit */
                tempC = (uchar)((iterator->character) / (1 << 8));
                fwrite(&tempC, sizeof(char), 1, stream); /* piszemy 2 symbole */
                if(!redundantZero) { /* chyba ze oznaczenie o nadmiarowym znaku '\0' ustawione na true */
                    tempC = (uchar)(iterator->character);
                    fwrite(&tempC, sizeof(char), 1, stream);
                }
            }
            else if(compLevel == 12) { /* dla kompresji 12-bit */
                *tempCode <<= 12;
                *tempCode += iterator->character;
                *currentBits += 12;
                if(*currentBits == 12) { /* jezeli liczba zajetych bitow wynosi dokladnie 12 */
                    temp = *tempCode % 16; /* odcinamy 4 ostatnie bity i przechowujemy pod zmienna tymczasowa */
                    *tempCode >>= 4;
                    tempC = (uchar)(*tempCode);
                    fwrite(&tempC, sizeof(char), 1, stream); /* zapisujemy jeden znak */
                    *tempCode = temp; /* i przywracamy 4 odciete bity do tempCode */
                    *currentBits = 4;
                } else { /* w przeciwnym wypadku wynosi 16, wiec robimy to samo co dla kompresji 16-bit */
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