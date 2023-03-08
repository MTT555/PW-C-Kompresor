#include <stdio.h>

#include "huffman.h"
#include "cipher.h"
#include "utils.h"
#include "countCharacters.h"
#include "string.h"

/**
Funkcja wykonujaca kompresje algorytmem Huffmana
    FILE *input - plik wejsciowy zawierajacy tekst do kompresji
    FILE *output - plik wyjsciowy, w ktorym zostanie zapisany skompresowany tekst
    int comp_level - poziom kompresji podany w bitach (dla comp_level == 0 - brak kompresji)
    bool cipher - zmienna mowiaca, czy tekst ma zostac rowniez zaszyfrowany
    char *cipher_key - klucz szyfrowania (nieistotny, gdy cipher == false)
    count **head - glowa listy zawierajaca ilosci wystapien danych znakow
*/
void huffman(FILE *input, FILE *output, int comp_level, bool cipher, char *cipher_key, count **head) {
	
count *nodeptr1, *nodeptr2, *node1, *node2;

while(1){
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
if (!(*head) || (nodeptr1->amount <= (*head)->amount)){
nodeptr1->next = (*head);
(*head) = nodeptr1;
continue;

}
    nodeptr2 = (*head);
    while (nodeptr2->next && (nodeptr1->amount > nodeptr2->next->amount)) nodeptr2 = nodeptr2->next;
    nodeptr1->next = nodeptr2->next;
    nodeptr2->next = nodeptr1;

}
int *code;//tu przechowujemy poszczegolne kody znakow
//ustawiamy maksymalna ilosc bitow w zaleznosci od zmiennej comp_level
if(comp_level == 8) code = malloc(sizeof(int)*8);

if(comp_level == 12) code = malloc(sizeof(int)*12);

if(comp_level == 16) code = malloc(sizeof(int)*16);
listCodes *listC = NULL;//lista do przechowywania kodow znakow
create_huffmann_tree(head, code, comp_level, 0,&listC);
#ifdef DEBUG
printf("List of codes:\n");
print_huffmann_tree(&listC);

#endif

writeCompressedToFile(input, output, comp_level, &listC);
}

void addToTheList1(listCodes **listC, char character,int *code, int length){
listCodes *new;
new = malloc(sizeof(count));
new->character = character;
new->code = malloc(sizeof(char)*length);
for (int i = 0; i < length; i++){
new->code[i] = '0'+code[i];
}
new->next = (*listC);
(*listC) = new;

}

void create_huffmann_tree(count **head, int *code, int comp_level, int top,listCodes **listC){
if ((*head)->left){
code[top] = 0;
create_huffmann_tree(&((*head)->left), code, comp_level, top+1,listC);
}
if ((*head)->right){
code[top] = 1;
create_huffmann_tree(&((*head)->right), code, comp_level, top+1,listC);

}
if (!((*head)->left) && !((*head)->right)) // jezeli dostalismy sie w koncu do liscia
addToTheList1(listC, (*head)->character, code, top);//dodajemy kazdy kod do listy
}

char *setEndOfString(char *string){
if(string == NULL) return NULL;
//czasem się zdarzy że będą się wyświetlać dziwne krzaki przy niektórych kodach (szczególnie krótkich)- ten poniższy kod ma temu zapobiec
	for (int i = 0; i < strlen(string); i++){
		if (string[i] != '1' && string[i] != '0'){
			string[i] = '\0';
			break;
		}
	}
	return string;
}
void print_huffmann_tree(listCodes **head){
listCodes *iterator = (*head);
while (iterator != NULL){
printf("Character: %c, Code: %s\n",iterator->character, setEndOfString(iterator->code));
iterator = iterator->next;
}
}

/**
Funkcja wykonujaca zapis do pliku skompresowanego tekstu
    FILE *input - plik wejsciowy z tekstem
    FILE *output - plik, do ktorego zostanie wykonany zapis
    int comp_level - poziom kompresji podany w bitach
    listCodes **head - glowa listy zawierajacej kody dla kazdego symbolu
*/

void writeCompressedToFile(FILE *input, FILE *output, int comp_level, listCodes **head) {
    char c;
    const int BUFSIZE = 2 << (comp_level + 2);
    int i, j, buf_pos = 0, comp_chars = 0, temp_symbol_code = 0, offset = 0; // buf_pos - aktualna pozycja w buferze
    short temp_buffer[BUFSIZE]; // zamiana na malloc/realloc?
    for(i = 0; i < BUFSIZE; i++)
        temp_buffer[i] = -1;
    fseek(input, 0, SEEK_SET);
    listCodes *iterator = NULL;
    while((c = fgetc(input)) != EOF) {
        iterator = (*head);
        while (iterator != NULL) {
            if(iterator->character == c) {
                for(i = 0; i < strlen(iterator->code); i++) {
                    temp_buffer[buf_pos] = (iterator->code[i] == '1' ? 1 : 0);
                    buf_pos++;
                }
                if(buf_pos >= comp_level) {
                    comp_chars = buf_pos / comp_level; // liczba gotowych do zapisu skompresowanych znakow
                    for(i = 0; i < comp_chars; i++) {
                        if(temp_buffer[8 * i] == 1)
                            temp_symbol_code -= 128;
                        for(j = 1; j < 8; j++)
                            temp_symbol_code += temp_buffer[8 * i + j] * (1 << (7 - j));
                        fprintf(output, "%c", (char)temp_symbol_code);
#ifdef DEBUG
                        fprintf(stderr, "Printed symbol: %c (code: %d)\n", (char)temp_symbol_code, temp_symbol_code);
#endif
                        temp_symbol_code = 0;
                    }
                    offset = buf_pos % comp_level; // offset - jako ilosc pozostalych znakow w tablicy po zapisie
                    if(offset != 0)
                        for(i = 0; i < offset; i++)
                            temp_buffer[i] = temp_buffer[buf_pos - offset + i];
                    buf_pos = offset;
                    comp_chars = 0;
                }
                break;
            }
            iterator = iterator->next;
        }
    }
    if(buf_pos != 0) {
        if(temp_buffer[8 * i] == 1)
            temp_symbol_code -= 128;
        for(j = 1; j < buf_pos; j++)
            temp_symbol_code += temp_buffer[8 * i + j] * (1 << (7 - j));
        fprintf(output, "%c", (char)temp_symbol_code);
    }
}