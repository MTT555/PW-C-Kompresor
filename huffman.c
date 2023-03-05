#include <stdio.h>

#include "huffman.h"
#include "cipher.h"
#include "utils.h"
#include "countCharacters.h"

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
#ifdef DEBUG
printf("List of codes:\n");
print_huffmann_tree(head,code,comp_level,0);


#endif
}

void print_huffmann_tree(count **head,int *code,int comp_level,int top){
if ((*head)->left){
code[top] = 0;
print_huffmann_tree(&((*head)->left), code, comp_level, top+1);
}
if ((*head)->right){
code[top] = 1;
print_huffmann_tree(&((*head)->right), code, comp_level, top+1);

}
if ((*head)->character != NULL) { // czyli doszliśmy do liścia
printf("Character: %c, code: ",(*head)->character);
for (int i = 0; i < top; i++){
printf("%d",code[i]);
}
printf("\n");

}

}
