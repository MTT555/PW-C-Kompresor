#include <stdio.h>
#include <stdlib.h>
#include "bits_analyze.h"
#include "decompress.h"
#include "dtree.h"
#include "huffman.h"

/**
Funkcja do analizy kolejnych bitow danego chara z pliku skompresowanego
    FILE *output - plik wyjsciowy
    unsigned char c - znak analizowany
    int compLevel - poziom kompresji podany w bitach
    listCodes **list - lista kodow znakow
    short reduntantBits - ilosc koncowych bitow do porzucenia w tym znaku 
    bool reduntantZero - zmienna odpowiedzialna za sprawdzanie czy pominac nadmiarowy koncowy znak zerowy podczas zapisu (zazwyczaj false)
    dnode_t **iterator - pseudoiterator po pomocniczym drzewie dnode_t
    mod_t *mode - aktualny tryb dzialania analizy bitow (typ zdefiniowany w decompress.h)
    unsigned char *buffer - bufor na odczytane bity
    int *curBufSize - aktualna wielkosc buforu na odczytane bity
    unsigned char *codeBuf - bufor na kolejne przejscia po drzewie
    int *curCodeBufSize - aktualna wielkosc bufora dla kodow przejsc po drzewie
    int *bufPos - aktualna pozycja w buforze na bity
    int *codeBufPos - aktualna pozycja w buforze kodow
    int *currentBits - ilosc aktualnie zajetych bitow (w ramach wsparcia dla dekompresji 12-bit)
    int *tempCode - aktualny kod odczytanego symbolu (w ramach wsparcia dla dekompresji 12-bit)
*/
void analyzeBits(FILE *output, unsigned char c, int compLevel, listCodes **list,
short reduntantBits, bool reduntantZero, dnode_t **iterator, mode_t *mode,
unsigned char *buffer, int *curBufSize, unsigned char *codeBuf, int *curCodeBufSize, 
int *bufPos, int *codeBufPos, int *currentBits, int *tempCode) {
    int i;
    short bits = 0; // ilosc przeanalizowanych bitow
    short cur_bit = 0; // wartosc obecnie analizowanego bitu
    short cur_code = 0; // obecny kod przejscia w sciezce
    while (bits != 8 - reduntantBits) {
        switch(*mode) {
            case dictRoad: {
                cur_code = 2 * returnBit(c, bits) + returnBit(c, bits + 1);
                bits += 2;
                if(cur_code == 3) {
                    *bufPos = 0;
                    *mode = bitsToWords;
#ifdef DEBUG
                    // wyswietlenie odczytanego slownika na stderr
                    fprintf(stderr, "List of codes read from the dictionary:\n");
                    printList(list, stderr);
#endif
                } else if(cur_code == 2) {
                    *iterator = (*iterator)->prev;
                    (*codeBufPos)--; // wyjscie o jeden w gore
                } else if(cur_code == 1) {
                    codeBuf[*codeBufPos] = '0' + goDown(iterator); // przejscie o jeden w dol
                    (*codeBufPos)++;
                    codeBuf[*codeBufPos] = '\0';
                    *mode = dictWord;
                } else if(cur_code == 0) {
                    codeBuf[*codeBufPos] = '0' + goDown(iterator); // przejscie o jeden w dol
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
                if(compareBuffer(list, buffer, output, compLevel, (bits == 8 - reduntantBits ? reduntantZero : false), currentBits, tempCode))
                    *bufPos = 0;
                break;
            }
        }
    }
}

/**
Funkcja zwracajaca pozadany bit z danego chara
    unsigned char c - znak
    int x - numer bitu, ktory ma byc zwrocony
*/
short returnBit(unsigned char c, int x) {
    unsigned char ch = c;
    ch >>= (7 - x);
    return ch % 2;
}