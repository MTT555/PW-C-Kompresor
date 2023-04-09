#include <stdio.h>
#include "huffman.h"
#include "utils.h"
#include "countCharacters.h"
#include "list.h"
#include "output.h"
#include "alloc.h"

bool huffman(FILE *input, FILE *output, settings_t s, count_t **head) {
    /* Deklaracja potrzebnych zmiennych */
    char *xxxx = "CTFX"; 
    uchar xor = (uchar)XOR; /* ustawienie poczatkowej wartosci sumy kontrolnej (183 = 0b10110111) */
    pack_t buffer; /* typ pomocniczny do zapisu bitowego z output.h */
    int packPos = 0; /* zmienna zapisujaca przejscia po drzewie od ostatniego zapisu znaku */
    int cipherPos = 0; /* aktualna pozycja w tym szyfrze */
    count_t *nodeptr1 = NULL, *nodeptr2 = NULL, *node1 = NULL, *node2 = NULL;
    int *code = NULL; /* tu przechowujemy poszczegolne kody znakow */
    listCodes_t *listC = NULL; /* lista do przechowywania kodow znakow */
    count_t *head_ptr = (*head);
    count_t *frPtrs[1 << 17];
    int i, frPtrsTabPos = 0;
    
    /* Obsluga buforu */
    buffer_t roadBuf; /* tymczasowe przechowywanie drogi przed zapisem do pliku */
    roadBuf.buf = NULL;
    roadBuf.curSize = 8192; /* aktualna wielkosc buferu na droge */
    roadBuf.pos = 0; /* ilosc aktualnie zapisanych bitow na droge */

    if(!tryMalloc((void **)&code, (1 << (s.compLevel + 1)) * sizeof(int))
         || !tryMalloc((void **)(&(roadBuf.buf)), roadBuf.curSize * sizeof(char)))
        return false;

    buffer.whole = 0; /* czyszcze calosc unii pack_t */
    roadBuf.buf[roadBuf.pos++] = 0; /* zapelnienie dwoch pierwszych bitow, ktore za kazdym nastepnym razem beda pomijane */
    roadBuf.buf[roadBuf.pos++] = 0;

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
        if(!tryMalloc((void **)&nodeptr1, sizeof(count_t))) {
            free(roadBuf.buf); /* zwalnianie zaalokowanych juz elementow przy nieudanej alokacji nowego wezla*/
            free(code);
            return false;
        }
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
    if(!createHuffmanTree(output, head, code, s, &xor, 0, &listC, &buffer, &packPos, &cipherPos, &roadBuf)) {
        freeListCodes(&listC); /* w wypadku niepowodzenia zwalniam pamiec */
        savePtrsFromList(head_ptr, frPtrs, &frPtrsTabPos);
        for(i = 0; i < frPtrsTabPos; i++)
            free(frPtrs[i]);
        free(roadBuf.buf);
        free(code);
        return false;
    }

    /* Po zapisaniu calego slownika do pliku trzeba wyraznie zaznaczyc jego koniec */
    saveBitIntoPack(output, s, &cipherPos, &buffer, &packPos, &xor, 1);
    saveBitIntoPack(output, s, &cipherPos, &buffer, &packPos, &xor, 1);
#ifdef DEBUG
    fprintf(stderr, "List of codes:\n");
    printListCodes(&listC, stderr);
#endif
    compressedToFile(input, output, s, &cipherPos, &listC, &xor, &buffer, &packPos);
    
    /* Zwalnianie pamieci */
    freeListCodes(&listC);
    savePtrsFromList(head_ptr, frPtrs, &frPtrsTabPos);
    for(i = 0; i < frPtrsTabPos; i++)
        free(frPtrs[i]);
    free(roadBuf.buf);
    free(code);
    return true;
}

bool addToTheListCodes(FILE *output, settings_t s, listCodes_t **listC, pack_t *buffer, int *packPos,
int character, const int *code, int length, uchar *xor, int *cipherPos, buffer_t *roadBuf) {
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
    for(i = 2; i < roadBuf->pos - 2; i++) /* i = 2, poniewaz 2 pierwsze symbole sa zawsze jednoznacznie przewidywalne */
        saveBitIntoPack(output, s, cipherPos, buffer, packPos, xor, roadBuf->buf[i] - '0');
    roadBuf->pos = 0; /* zerowanie pozycji */
    /* zapisanie 01 na przejscie w dol i napotkanie liscia tj. slowa */
    saveBitIntoPack(output, s, cipherPos, buffer, packPos, xor, 0);
    saveBitIntoPack(output, s, cipherPos, buffer, packPos, xor, 1);
    for(i = 0; i < s.compLevel; i++)
        saveBitIntoPack(output, s, cipherPos, buffer, packPos, xor, (new->character / (1 << (s.compLevel - 1 - i))) % 2);
    return true;
}

bool createHuffmanTree(FILE *output, count_t **head, int *code, settings_t s, uchar *xor, int top, 
listCodes_t **listC, pack_t *buffer, int *packPos, int *cipherPos, buffer_t *roadBuf) {
    if ((*head)->left) {
        code[top] = 0;
        if(roadBuf->pos == roadBuf->curSize) { /* sprawdzenie, czy nie trzeba realokowac tablicy na wieksza */
            if(!tryRealloc((void **)(&(roadBuf->buf)), 2 * (roadBuf->curSize) * sizeof(char)))
                return false;
            roadBuf->curSize *= 2;
        }
        roadBuf->buf[(roadBuf->pos)++] = '0'; /* zapisanie dwoch zer na przejscie w dol */
        roadBuf->buf[(roadBuf->pos)++] = '0';
        if(!createHuffmanTree(output, &((*head)->left), code, s, xor, top + 1, listC, buffer, packPos, cipherPos, roadBuf))
            return false;
    }
    if ((*head)->right) {
        code[top] = 1;
        if(roadBuf->pos == roadBuf->curSize) { /* sprawdzenie, czy nie trzeba realokowac tablicy na wieksza */
            if(!tryRealloc((void **)(&(roadBuf->buf)), 2 * (roadBuf->curSize) * sizeof(char)))
                return false;
            roadBuf->curSize *= 2;
        }
        roadBuf->buf[(roadBuf->pos)++] = '0'; /* zapisanie dwoch zer na przejscie w dol */
        roadBuf->buf[(roadBuf->pos)++] = '0';
        if(!createHuffmanTree(output, &((*head)->right), code, s, xor, top + 1, listC, buffer, packPos, cipherPos, roadBuf))
            return false;
    }
    if (!((*head)->left) && !((*head)->right)) { /* jezeli dostalismy sie w koncu do liscia, probujemy dodac kazdy kod do listy */
        if(!addToTheListCodes(output, s, listC, buffer, packPos, (*head)->character, code, top, xor, cipherPos, roadBuf))
            return false;
    }
    /* wyjscie do gory znajduje sie na koncu tej funkcji, poniewaz jest to funkcja rekurencyjna */
    if(roadBuf->pos == roadBuf->curSize) { /* sprawdzenie, czy nie trzeba realokowac tablicy na wieksza */
        if(!tryRealloc((void **)(&(roadBuf->buf)), 2 * (roadBuf->curSize) * sizeof(char)))
            return false;
        roadBuf->curSize *= 2;
    }
    roadBuf->buf[(roadBuf->pos)++] = '1';
    roadBuf->buf[(roadBuf->pos)++] = '0';
    return true;
}

/**
Dodatkowe info na temat pierwszych 4 bajtow pliku wyjsciowego:
Poczatek zapisu do pliku - oznaczenie pliku skompresowanego
    C, T - pierwsze litery nazwisk autorow tego kompresora
    F - 8 bitow zarezerwowane na pozniejsze dopisanie potrzebnych flag
    X - wynik sumy kontrolnej xor, ktora ma sprawdzac czy plik nie jest uszkodzony przy probie dekompresji
*/