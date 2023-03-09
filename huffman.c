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

writeCompressedToFile(input, output, comp_level, cipher, cipher_key, &listC);
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
*/
typedef union pack {
    short whole;  // 16-bitowa zmienna do sprawnego wykonywania operacji bitowych na calym union
    struct {
        char buf; // znak utworzony z bitow 0-7 sluzacych jako bufer
        char out; // znak utworzony z bitow 8-15 tworzacych faktyczny znak wyjsciowy
    } chars;
} pack_t;

/**
Funkcja wykonujaca zapis do pliku skompresowanego tekstu
    Wszystkie argumenty zgodne z opisem zawartym w funkcji void huffman() powyzej 
*/
void writeCompressedToFile(FILE *input, FILE *output, int comp_level, bool cipher, char *cipher_key, listCodes **head) {
    char c;
    int i, j, ending = 0; // ending - ilosc niezapisanych bitow konczacych plik

    unsigned int cipher_pos = 0; // zmienna przechowujaca aktualna pozycje w szyfrze
    unsigned int cipher_len = strlen(cipher_key); // dlugosc szyfru
    
    pack_t buffer;
    buffer.whole = 0; // wyzerowanie calosci swiezoutworzonej paczki
    short pack_pos = 0; // zmienna sluzaca do monitorowania aktualnej pozycji w paczce
    
    fseek(input, 0, SEEK_SET); // ustawienie kursora na poczatek inputu
    listCodes *iterator = NULL; // iterator po liscie kodow
    
    while((c = fgetc(input)) != EOF) { 
        iterator = (*head); // ustawienie iteratora na poczatek przy kazdym nastepnym symbolu
        while (iterator != NULL) {
            if(iterator->character == c) {
                // jesli znaleziono symbol, to przepisujemy znak po znaku do buforu jednoczesnie przeksztalcajac chary na shorty
                for(i = 0; i < strlen(iterator->code); i++) {
                    if(pack_pos == 16) { // jezeli bufer jest pelen
                        if(cipher) { // jezeli plik ma zostac zaszyfrowany
                            buffer.chars.out += cipher_key[cipher_pos % cipher_len]; // dokonujemy szyfrowania znaku
                            cipher_pos++;
                        }
                        fprintf(output, "%c", buffer.chars.out); // wydrukuj znak
#ifdef DEBUG
                        // wyswietlenie zapisanego znaku wraz z jego kodem na stderr
                        fprintf(stderr, "Printed symbol: %c (code: %d)\n", buffer.chars.out, (int)buffer.chars.out);
#endif
                        pack_pos -= 8; // zmniejsz pozycje o 8 bitow
                    }
                    buffer.whole <<= 1; // przesuniecie wszystkich liczb o jeden w prawo
                    buffer.whole += (iterator->code[i] == '1' ? 1 : 0); // nadanie wartosci bitowi powstalemu po przesunieciu
                    pack_pos++; // aktualizacja pozycji
                }
                break;
            }
            else
                iterator = iterator->next;
        }
    }
    // po przejsciu po calym pliku, w razie potrzeby tworzymy ostatni "niepelny" znak z pozostalych nadmiarowych zapisanych bitow
    if(pack_pos != 16) {
        ending = 16 - pack_pos;
        buffer.whole <<= ending;
        fprintf("%c%c", buffer.chars.out, buffer.chars.buf);
#ifdef DEBUG
        // wyswietlenie tych znakow wraz z kodami na stderr
        fprintf(stderr, "Printed symbol: %c (code: %d)\n", buffer.chars.out, (int)buffer.chars.out);
        fprintf(stderr, "Printed symbol: %c (code: %d)\n", buffer.chars.buf, (int)buffer.chars.buf);
#endif
    }
}