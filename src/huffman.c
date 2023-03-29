#include <stdio.h>
#include <string.h>
#include "huffman.h"
#include "utils.h"
#include "countCharacters.h"
#include "list.h"
#include "output.h"

static pack_t buffer; /* typ pomocniczny do zapisu bitowego z output.h */
static short pack_pos = 0; /* zmienna zapisujaca przejscia po drzewie od ostatniego zapisu znaku */
static uchar cipher_key[] = CIPHER; /* klucz szyfrowania */
static uchar xor = (uchar)183; /* ustawienie poczatkowej wartosci sumy kontrolnej (183 = 0b10110111) */
static uchar road_buffer[512]; /* tymczasowe przechowywanie drogi przed zapisem do pliku */
static int road_pos = 0; /* ilosc aktualnie zapisanych bitow na droge */

/**
Funkcja wykonujaca kompresje algorytmem Huffmana
    FILE *input - plik wejsciowy zawierajacy tekst do kompresji
    FILE *output - plik wyjsciowy, w ktorym zostanie zapisany skompresowany tekst
    int compLevel - poziom kompresji podany w bitach (dla compLevel == 0 - brak kompresji)
    bool cipher - zmienna mowiaca, czy tekst ma zostac rowniez zaszyfrowany
    uchar *cipher_key - klucz szyfrowania (nieistotny, gdy cipher == false)
    count_t **head - glowa listy zawierajaca ilosci wystapien danych znakow
*/
void huffman(FILE *input, FILE *output, int compLevel, bool cipher, count_t **head) {
    count_t *nodeptr1 = NULL, *nodeptr2 = NULL, *node1 = NULL, *node2 = NULL;
    int *code = malloc((1 << (compLevel + 1)) * sizeof(int)); /* tu przechowujemy poszczegolne kody znakow */
    listCodes_t *listC = NULL; /* lista do przechowywania kodow znakow */
    char *x = "CTFX"; 
    count_t *head_ptr = (*head);

    buffer.whole = 0;
    road_buffer[road_pos++] = 0; /* zapelnienie dwoch pierwszych bitow, ktore potem beda za kazdym razem pomijane */
    road_buffer[road_pos++] = 0;

    /* Zajecie pierwszych 4 bajtow outputu na pozniejsze oznaczenia pliku (wiecej info na koncu tego pliku) */
    fwrite(x, sizeof(char), 4, output);

    while((bool)true) {
        /* pobieramy z listy dwa pierwsze elementy o najmniejszej czestosci wystapien */
        node1 = (*head);
        node2 = node1->next;
        if(!node2)
            break;
        /* usuwamy te dwa elementy z listy */
        (*head) = node2->next;
        /* budujemy nowy wezel zlozony z tych dwoch elementow */
        nodeptr1 = malloc(sizeof(count_t));
        nodeptr1->left = node1;
        nodeptr1->right = node2;
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

    createHuffmanTree(output, head, code, cipher, compLevel, 0, &listC); /* tworzenie drzewa Huffmana */
    /* po zapisaniu calego slownika do pliku trzeba wyraznie zaznaczyc jego koniec */
    saveBitIntoPack(output, cipher, cipher_key, &buffer, &pack_pos, &xor, 1);
    saveBitIntoPack(output, cipher, cipher_key, &buffer, &pack_pos, &xor, 1);
#ifdef DEBUG
    fprintf(stderr, "List of codes:\n");
    printListCodes(&listC, stderr);
#endif

    compressedToFile(input, output, compLevel, cipher, cipher_key, &listC, &xor, &buffer, &pack_pos);
    freeListCodes(&listC);
    freeList(head_ptr);
    free(code);
}

void addToTheList1(FILE *output, int compLevel, bool cipher, listCodes_t **listC, int character, int *code, int length) {
    int i;
    listCodes_t *new = NULL;
    new = malloc(sizeof(listCodes_t));
    new->character = character;
    new->code = malloc(sizeof(char) * (length + 1));
    for(i = 0; i < length; i++)
        new->code[i] = '0'+code[i];
    new->code[length] = '\0';
    new->next = (*listC);
    (*listC) = new;

    /* zapis nowego znaku w slowniku do bufera */
    for(i = 2; i < road_pos - 2; i++) /* i = 2, poniewaz 2 pierwsze symbole sa zawsze jednoznacznie przewidywalne */
        saveBitIntoPack(output, cipher, cipher_key, &buffer, &pack_pos, &xor, (short)(road_buffer[i] - '0'));
    road_pos = 0; /* zerowanie pozycji */
    /* zapisanie 01 na przejscie w dol i napotkanie liscia tj. slowa */
    saveBitIntoPack(output, cipher, cipher_key, &buffer, &pack_pos, &xor, 0);
    saveBitIntoPack(output, cipher, cipher_key, &buffer, &pack_pos, &xor, 1);
    for(i = 0; i < compLevel; i++)
        saveBitIntoPack(output, cipher, cipher_key, &buffer, &pack_pos, &xor, (new->character / (1 << (compLevel - 1 - i))) % 2);
}

void createHuffmanTree(FILE *output, count_t **head, int *code, bool cipher, int compLevel, int top, listCodes_t **listC) {
    if ((*head)->left) {
        code[top] = 0;
        road_buffer[road_pos++] = '0'; /* zapisanie dwoch zer na przejscie w dol */
        road_buffer[road_pos++] = '0';
        createHuffmanTree(output, &((*head)->left), code, cipher, compLevel, top + 1, listC);
    }
    if ((*head)->right) {
        code[top] = 1;
        road_buffer[road_pos++] = '0'; /* zapisanie dwoch zer na przejscie w dol */
        road_buffer[road_pos++] = '0';
        createHuffmanTree(output, &((*head)->right), code, cipher, compLevel, top + 1, listC);
    }
    if (!((*head)->left) && !((*head)->right)) { /* jezeli dostalismy sie w koncu do liscia */
        addToTheList1(output, compLevel, cipher, listC, (*head)->character, code, top); /* dodajemy kazdy kod do listy */
    }
    /* wyjscie do gory znajduje sie na koncu tej funkcji, poniewaz jest to funkcja rekurencyjna */
    road_buffer[road_pos++] = '1';
    road_buffer[road_pos++] = '0';
}

/**
Dodatkowe info na temat pierwszych 4 bajtow pliku wyjsciowego:
Poczatek zapisu do pliku - oznaczenie pliku skompresowanego
    C, T - pierwsze litery nazwisk autorow tego kompresora
    F - 8 bitow zarezerwowane na pozniejsze dopisanie potrzebnych flag
    X - wynik sumy kontrolnej xor, ktora ma sprawdzac czy plik nie jest uszkodzony przy probie dekompresji
*/