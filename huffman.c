#include <stdio.h>
#include <string.h>
#include "huffman.h"
#include "cipher.h"
#include "utils.h"
#include "countCharacters.h"
#include "output.h"

// zmienna zapisujaca liczbe przejsc w lewo od ostatniego zapisania slowa slownika do pliku
static short pack_pos = 0;
static pack_t buffer; // typ pomocniczny do zapisu bitowego z output.h
static char cipher_key[] = "Politechnika_Warszawska"; // klucz szyfrowania
static char xor = (char)0b10110111; // ustawienie poczatkowej wartosci sumy kontrolnej
static char *road_buffer = NULL; // tymczasowe przechowywanie drogi przed zapisem do pliku
static int road_pos = 0; // ilosc aktualnie zapisanych bitow na droge
static int cur_buf_size = 512; // obecna wielkosc bufora, zmieniana, gdy trzeba uzyc realloca

/**
Funkcja wykonujaca kompresje algorytmem Huffmana
    FILE *input - plik wejsciowy zawierajacy tekst do kompresji
    FILE *output - plik wyjsciowy, w ktorym zostanie zapisany skompresowany tekst
    int comp_level - poziom kompresji podany w bitach (dla comp_level == 0 - brak kompresji)
    bool cipher - zmienna mowiaca, czy tekst ma zostac rowniez zaszyfrowany
    char *cipher_key - klucz szyfrowania (nieistotny, gdy cipher == false)
    count **head - glowa listy zawierajaca ilosci wystapien danych znakow
*/
void huffman(FILE *input, FILE *output, int comp_level, bool cipher, count **head) {
    count *nodeptr1, *nodeptr2, *node1, *node2;
    fprintf(output, "XXXX"); // zajecie pierwszych 4 bajtow outputu na pozniejsze oznaczenia pliku
    road_buffer = malloc(512 * sizeof(char)); // alokacja pamieci na bufor dla drogi
    road_buffer[road_pos++] = 0; // zapelnienie dwoch pierwszych bitow, ktore potem beda za kazdym razem pomijane
    road_buffer[road_pos++] = 0;
    buffer.whole = 0;

    while(1) {
        // pobieramy z listy dwa pierwsze elementy o najmniejszej czestosci wystapien
        node1 = (*head);
        node2 = node1->next;
        if(!node2) break;
            // usuwamy te dwa elementy z listy
            (*head) = node2->next;
        // budujemy nowy wezel zlozony z tych dwoch elementow
        nodeptr1 = malloc(sizeof(count));
        nodeptr1->left = node1;
        nodeptr1->right = node2;
        nodeptr1->amount = node1->amount + node2->amount; // sumujemy ilosc wystapien tych liter
        // nowo utworzony wezel wstawiamy do listy
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

    int *code; //tu przechowujemy poszczegolne kody znakow
    //ustawiamy maksymalna ilosc bitow w zaleznosci od zmiennej comp_level
    if(comp_level == 8)
        code = malloc(sizeof(int)*8);
    else if(comp_level == 12)
        code = malloc(sizeof(int)*12);
    else if(comp_level == 16)
        code = malloc(sizeof(int)*16);
    listCodes *listC = NULL; //lista do przechowywania kodow znakow
    create_huffmann_tree(output, head, code, cipher, comp_level, 0, &listC); // tworzenie drzewa Huffmana
    // po zapisaniu calego slownika do pliku trzeba wyraznie zaznaczyc jego koniec
    saveBitIntoPack(output, cipher, cipher_key, &buffer, &pack_pos, &xor, 1);
    saveBitIntoPack(output, cipher, cipher_key, &buffer, &pack_pos, &xor, 1);
#ifdef DEBUG
    fprintf(stderr, "List of codes:\n");
    print_huffmann_tree(&listC, stderr);
#endif

    compressedToFile(input, output, comp_level, cipher, cipher_key, &listC, &xor, &buffer, &pack_pos);
    free(road_buffer); // po zapisie do pliku zwalniam pamiec po buforze
}

void addToTheList1(FILE *output, int comp_level, bool cipher, listCodes **listC, char character, int *code, int length) {
    int i;
    listCodes *new = NULL;
    new = malloc(sizeof(listCodes));
    new->character = character;
    new->code = malloc(sizeof(char) * length);
    for(i = 0; i < length; i++)
        new->code[i] = '0'+code[i];
    new->next = (*listC);
    (*listC) = new;

    // zapis nowego znaku w slowniku do bufera
    for(i = 2; i < road_pos - 2; i++) // i = 2, poniewaz 2 pierwsze symbole sa zawsze jednoznacznie przewidywalne
        saveBitIntoPack(output, cipher, cipher_key, &buffer, &pack_pos, &xor, (short)(road_buffer[i] - '0'));
    road_pos = 0; // zerowanie pozycji
    // zapisanie 01 na przejscie w dol i napotkanie liscia tj. slowa
    saveBitIntoPack(output, cipher, cipher_key, &buffer, &pack_pos, &xor, 0);
    saveBitIntoPack(output, cipher, cipher_key, &buffer, &pack_pos, &xor, 1);
    for(i = 0; i < comp_level; i++)
        saveBitIntoPack(output, cipher, cipher_key, &buffer, &pack_pos, &xor, (new->character / (1 << (7 - i))) % 2);
}

void create_huffmann_tree(FILE *output, count **head, int *code, bool cipher, int comp_level, int top, listCodes **listC) {
    if ((*head)->left) {
        code[top] = 0;
        if(road_pos == cur_buf_size) {
            realloc(road_buffer, 2 * cur_buf_size * sizeof(char));
            cur_buf_size *= 2;
        } 
        road_buffer[road_pos++] = '0'; // zapisanie dwoch zer na przejscie w dol
        road_buffer[road_pos++] = '0';
        create_huffmann_tree(output, &((*head)->left), code, cipher, comp_level, top + 1, listC);
    }
    if ((*head)->right) {
        code[top] = 1;
        if(road_pos == cur_buf_size){
            realloc(road_buffer, 2 * cur_buf_size * sizeof(char));
            cur_buf_size *= 2;
        }
        road_buffer[road_pos++] = '0'; // zapisanie dwoch zer na przejscie w dol
        road_buffer[road_pos++] = '0';
        create_huffmann_tree(output, &((*head)->right), code, cipher, comp_level, top + 1, listC);
    }
    if (!((*head)->left) && !((*head)->right)) { // jezeli dostalismy sie w koncu do liscia
        addToTheList1(output, comp_level, cipher, listC, (*head)->character, code, top);//dodajemy kazdy kod do listy
    }
    // wyjscie do gory znajduje sie na koncu tej funkcji, poniewaz jest to funkcja rekurencyjna
    road_buffer[road_pos++] = '1';
    road_buffer[road_pos++] = '0';
}

char *setEndOfString(char *string){
    if(string == NULL)
        return NULL;
    
    // czasem się zdarzy że będą się wyświetlać dziwne krzaki przy niektórych kodach (szczególnie krótkich) - ten poniższy kod ma temu zapobiec
	for (int i = 0; i < strlen(string); i++) {
		if (string[i] != '1' && string[i] != '0') {
			string[i] = '\0';
			break;
		}
	}
	return string;
}

void print_huffmann_tree(listCodes **head, FILE *stream) {
    listCodes *iterator = (*head);
    while (iterator != NULL) {
        fprintf(stream, "Character: %c, Code: %s\n", iterator->character, setEndOfString(iterator->code));
        iterator = iterator->next;
    }
}