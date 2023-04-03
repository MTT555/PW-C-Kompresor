#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "bits_analyze.h"
#include "decompress.h"
#include "dtree.h"
#include "huffman.h"

/**
Funkcja do analizy kolejnych bitow danego chara z pliku skompresowanego
    FILE *output - plik wyjsciowy
    uchar c - znak analizowany
    int compLevel - poziom kompresji podany w bitach
    listCodes_t **list - lista kodow znakow
    short redundantBits - ilosc koncowych bitow do porzucenia w tym znaku 
    bool redundantZero - zmienna odpowiedzialna za sprawdzanie czy pominac nadmiarowy koncowy znak zerowy podczas zapisu (zazwyczaj false)
    dnode_t **iterator - pseudoiterator po pomocniczym drzewie dnode_t
    mod_t *mode - aktualny tryb dzialania analizy bitow (typ zdefiniowany w decompress.h)
    uchar *buffer - bufor na odczytane bity
    int *curBufSize - aktualna wielkosc buforu na odczytane bity
    uchar *codeBuf - bufor na kolejne przejscia po drzewie
    int *curCodeBufSize - aktualna wielkosc bufora dla kodow przejsc po drzewie
    int *bufPos - aktualna pozycja w buforze na bity
    int *codeBufPos - aktualna pozycja w buforze kodow
    int *currentBits - ilosc aktualnie zajetych bitow (w ramach wsparcia dla dekompresji 12-bit)
    int *tempCode - aktualny kod odczytanego symbolu (w ramach wsparcia dla dekompresji 12-bit)
*/
void analyzeBits(FILE *output, uchar c, int compLevel, listCodes_t **list,
short redundantBits, bool redundantZero, dnode_t **iterator, mod_t *mode,
uchar *buffer, int *curBufSize, uchar *codeBuf, int *curCodeBufSize, 
int *bufPos, int *codeBufPos, int *currentBits, int *tempCode) {
    int i;
    short bits = 0; /* ilosc przeanalizowanych bitow */
    short currentCode = 0; /* obecny kod przejscia w sciezce */
    while (bits != 8 - redundantBits) {
        switch(*mode) {
            case dictRoad: {
                currentCode = 2 * returnBit(c, bits) + returnBit(c, bits + 1);
                bits += 2;
                if(currentCode == 3) {
                    *bufPos = 0;
                    *mode = bitsToWords;
#ifdef DEBUG
                    /* wyswietlenie odczytanego slownika na stderr */
                    fprintf(stderr, "List of codes read from the dictionary:\n");
                    printListCodes(&*list, stderr);
#endif
                } else if(currentCode == 2) {
                    *iterator = (*iterator)->prev;
                    (*codeBufPos)--; /* wyjscie o jeden w gore */
                } else if(currentCode == 1) {
                    codeBuf[*codeBufPos] = '0' + goDown(iterator); /* przejscie o jeden w dol */
                    (*codeBufPos)++;
                    codeBuf[*codeBufPos] = '\0';
                    *mode = dictWord;
                } else if(currentCode == 0) {
                    codeBuf[*codeBufPos] = '0' + goDown(iterator); /* przejscie o jeden w dol */
                    (*codeBufPos)++;
                }
                break;
            }
            case dictWord: {
                buffer[(*bufPos)++] = returnBit(c, bits++);
                buffer[(*bufPos)++] = returnBit(c, bits++);
                if(*bufPos == compLevel) {
                    int result = 0;
                    for(i = 0; i < compLevel; i++) {
                        result *= 2;
                        result += buffer[i];
                    }
                    addCode(list, result, codeBuf);
                    *bufPos = 0;
                    *iterator = (*iterator)->prev;
                    (*codeBufPos)--;
                    *mode = dictRoad;
                }
                break;
            }
            case bitsToWords: {
                buffer[(*bufPos)++] = '0' + returnBit(c, bits);
                buffer[*bufPos] = '\0';
                bits++;
                if(compareBuffer(list, buffer, output, compLevel, (bits == 8 - redundantBits ? redundantZero : false), currentBits, tempCode))
                    *bufPos = 0;
                break;
            }
        }
    }
}

/**
Funkcja zwracajaca pozadany bit z danego chara
    uchar c - znak
    int x - numer bitu, ktory ma byc zwrocony
*/
short returnBit(uchar c, int x) {
    uchar ch = c;
    ch >>= (7 - x);
    return ch % 2;
}