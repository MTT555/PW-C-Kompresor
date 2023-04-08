#include <stdio.h>
#include <string.h>
#include "huffman.h"
#include "utils.h"
#include "countCharacters.h"
#include "list.h"
#include "output.h"
#include "alloc.h"

bool huffman(FILE *input, FILE *output, int compLevel, bool cipher, count_t **head) {
    char *xxxx = "CTFX"; 
    uchar xor = (uchar)183; /* ustawienie poczatkowej wartosci sumy kontrolnej (183 = 0b10110111) */
    pack_t buffer; /* typ pomocniczny do zapisu bitowego z output.h */
    int packPos = 0; /* zmienna zapisujaca przejscia po drzewie od ostatniego zapisu znaku */
    uchar *cipherKey = (uchar *)"Politechnika_Warszawska"; /* klucz szyfrowania */
    int cipherPos = 0; /* aktualna pozycja w tym szyfrze */
    count_t *nodeptr1 = NULL, *nodeptr2 = NULL, *node1 = NULL, *node2 = NULL;
    int *code = NULL; /* tu przechowujemy poszczegolne kody znakow */
    listCodes_t *listC = NULL; /* lista do przechowywania kodow znakow */
    count_t *head_ptr = (*head);
    int roadBufSize = 8192; /* aktualna wielkosc buferu na droge */
    uchar *roadBuffer = NULL; /* tymczasowe przechowywanie drogi przed zapisem do pliku */
    int roadPos = 0; /* ilosc aktualnie zapisanych bitow na droge */

    if(!tryMalloc((void **)&code, (1 << (compLevel + 1)) * sizeof(int))
        || !tryMalloc((void **)&roadBuffer, roadBufSize * sizeof(char)))
        return false;

    buffer.whole = 0; /* czyszcze całosc unii pack_t */
    roadBuffer[roadPos++] = 0; /* zapelnienie dwoch pierwszych bitow, ktore za kazdym nastepnym razem beda pomijane */
    roadBuffer[roadPos++] = 0;

    /* Zajecie pierwszych 4 bajtow outputu na pozniejsze oznaczenia pliku (wiecej info na koncu tego pliku) */
    fwrite(xxxx, sizeof(char), 4, output);

    while((bool)true) {
        /* pobieramy z listy dwa pierwsze elementy o najmniejszej czestosci wystapien */
        node1 = (*head);
        node2 = node1->next;
        if(!node2)
            break;
        /* usuwamy te dwa elementy z listy */
        (*head) = node2->next;
        /* budujemy nowy wezel zlozony z tych dwoch elementow */
        if(!tryMalloc((void **)&nodeptr1, sizeof(count_t)))
            return false;
        nodeptr1->left = node1;
        nodeptr1->right = node2;
        nodeptr1->next = NULL;
        nodeptr1->amount = node1->amount + node2->amount; /* sumujemy ilosc wystapien tych liter */
        /* nowo utworzony wezel wstawiamy do listy */
        if (!(*head) || (nodeptr1->amount <= (*head)->amount)) {
            nodeptr1->next = (*head);
            (*head) = nodeptr1;
            continue;
        }
        nodeptr2 = (*head);
        while (nodeptr2->next && (nodeptr1->amount > nodeptr2->next->amount))
            nodeptr2 = nodeptr2->next;
        nodeptr1->next = nodeptr2->next;
        nodeptr2->next = nodeptr1;
    }

    /* Tworzenie drzewa Huffmana */
    createHuffmanTree(output, head, code, cipher, compLevel, &xor, 0,
    &listC, &buffer, &packPos, cipherKey, &cipherPos, roadBuffer, &roadPos);
    /* Po zapisaniu calego slownika do pliku trzeba wyraznie zaznaczyc jego koniec */
    saveBitIntoPack(output, cipher, cipherKey, &cipherPos, &buffer, &packPos, &xor, 1);
    saveBitIntoPack(output, cipher, cipherKey, &cipherPos, &buffer, &packPos, &xor, 1);
#ifdef DEBUG
    fprintf(stderr, "List of codes:\n");
    printListCodes(&listC, stderr);
#endif
    compressedToFile(input, output, compLevel, cipher, cipherKey, &listC, &xor, &buffer, &packPos);
    
    /* Zwalnianie pamieci */
    freeListCodes(&listC);
    freeList(head_ptr);
    free(roadBuffer);
    free(code);
    return true;
}

bool addToTheListCodes(FILE *output, int compLevel, bool cipher, listCodes_t **listC, pack_t *buffer, int *packPos,
int character, int *code, int length, uchar *xor, uchar *cipherKey, int *cipherPos, uchar *roadBuffer, int *roadPos) {
    int i;
    listCodes_t *new = NULL;
    if(!tryMalloc((void **)&new, sizeof(listCodes_t)))
        return false;

    new->character = character;
    new->code = NULL;
    if(!tryMalloc((void **)(&(new->code)), sizeof(char) * (length + 1)))
        return false;

    for(i = 0; i < length; i++)
        new->code[i] = '0' + code[i];
    new->code[length] = '\0';
    new->next = (*listC);
    (*listC) = new;

    /* zapis nowego znaku w slowniku do bufera */
    for(i = 2; i < (*roadPos) - 2; i++) /* i = 2, poniewaz 2 pierwsze symbole sa zawsze jednoznacznie przewidywalne */
        saveBitIntoPack(output, cipher, cipherKey, cipherPos, buffer, packPos, xor, roadBuffer[i] - '0');
    *roadPos = 0; /* zerowanie pozycji */
    /* zapisanie 01 na przejscie w dol i napotkanie liscia tj. slowa */
    saveBitIntoPack(output, cipher, cipherKey, cipherPos, buffer, packPos, xor, 0);
    saveBitIntoPack(output, cipher, cipherKey, cipherPos, buffer, packPos, xor, 1);
    for(i = 0; i < compLevel; i++)
        saveBitIntoPack(output, cipher, cipherKey, cipherPos, buffer, packPos, xor, (new->character / (1 << (compLevel - 1 - i))) % 2);
    return true;
}

bool createHuffmanTree(FILE *output, count_t **head, int *code, bool cipher, int compLevel, uchar *xor,
int top, listCodes_t **listC, pack_t *buffer, int *packPos, uchar *cipherKey, int *cipherPos, uchar *roadBuffer, int *roadPos) {
    if ((*head)->left) {
        code[top] = 0;
        roadBuffer[(*roadPos)++] = '0'; /* zapisanie dwoch zer na przejscie w dol */
        roadBuffer[(*roadPos)++] = '0';
        if(!createHuffmanTree(output, &((*head)->left), code, cipher, compLevel, xor,
        top + 1, listC, buffer, packPos, cipherKey, cipherPos, roadBuffer, roadPos))
            return false;
    }
    if ((*head)->right) {
        code[top] = 1;
        roadBuffer[(*roadPos)++] = '0'; /* zapisanie dwoch zer na przejscie w dol */
        roadBuffer[(*roadPos)++] = '0';
        if(!createHuffmanTree(output, &((*head)->right), code, cipher, compLevel, xor,
        top + 1, listC, buffer, packPos, cipherKey, cipherPos, roadBuffer, roadPos))
            return false;
    }
    if (!((*head)->left) && !((*head)->right)) { /* jezeli dostalismy sie w koncu do liscia */
        if(!addToTheListCodes(output, compLevel, cipher, listC, buffer, packPos, (*head)->character,
        code, top, xor, cipherKey, cipherPos, roadBuffer, roadPos)) /* dodajemy kazdy kod do listy */
            return false;
    }
    /* wyjscie do gory znajduje sie na koncu tej funkcji, poniewaz jest to funkcja rekurencyjna */
    roadBuffer[(*roadPos)++] = '1';
    roadBuffer[(*roadPos)++] = '0';
    return true;
}

/**
Dodatkowe info na temat pierwszych 4 bajtow pliku wyjsciowego:
Poczatek zapisu do pliku - oznaczenie pliku skompresowanego
    C, T - pierwsze litery nazwisk autorow tego kompresora
    F - 8 bitow zarezerwowane na pozniejsze dopisanie potrzebnych flag
    X - wynik sumy kontrolnej xor, ktora ma sprawdzac czy plik nie jest uszkodzony przy probie dekompresji
*/