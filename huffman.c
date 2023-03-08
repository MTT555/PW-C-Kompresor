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
Funkcja wykonujaca zapis do pliku skompresowanego tekstu
    Wszystkie argumenty zgodne z opisem zawartym w funkcji void huffman() powyzej 
*/
void writeCompressedToFile(FILE *input, FILE *output, int comp_level, bool cipher, char *cipher_key, listCodes **head) {
    char c;
    const int BUFSIZE = 2 << (comp_level + 2); // wielkosc buforu
    int i, j, comp_chars = 0, offset = 0;
    int symbol_code = 0; // zmienna, w ktorej beda "tworzone" kody przekompresowanych symboli
    int buf_pos = 0; // zmienna przechowujaca aktualna pozycje w buforze
    int cipher_pos = 0; // zmienna przechowujaca aktualna pozycje w szyfrze
    unsigned int cipher_len = strlen(cipher_key); // dlugosc szyfru
    short buffer[BUFSIZE]; // tablica na bufor
    
    fseek(input, 0, SEEK_SET); // ustawienie kursora na poczatek inputu
    listCodes *iterator = NULL; // iterator po liscie kodow
    
    while((c = fgetc(input)) != EOF) { 
        iterator = (*head); // ustawienie iteratora na poczatek przy kazdym nastepnym symbolu
        while (iterator != NULL) {
            if(iterator->character == c) {
                // jesli znaleziono symbol, to przepisujemy znak po znaku do buforu jednoczesnie przeksztalcajac chary na shorty
                for(i = 0; i < strlen(iterator->code); i++) {
                    buffer[buf_pos] = (iterator->code[i] == '1' ? 1 : 0);
                    buf_pos++;
                }

                // jezeli w buforze jest wystarczajaco bitow na zlozenie bajtu tj. chara
                if(buf_pos >= comp_level) {
                    comp_chars = buf_pos / comp_level; // liczba gotowych do zapisu znakow
                    for(i = 0; i < comp_chars; i++) {
                        if(buffer[8 * i] == 1) // pierwszy bit jako bit znaku
                            symbol_code -= 128; // -128 spowoduje znajdziemy sie w znakach o kodach w zakresie <-128, 0)
                        for(j = 1; j < 8; j++) // dodanie reszty bitow przemnozonych przez odpowiednie potegi liczby 2
                            symbol_code += buffer[8 * i + j] * (1 << (7 - j));
                        if(cipher) { // jesli uzytkownik zdecydowal sie na zaszyfrowanie
                            symbol_code += cipher_key[cipher_pos]; // szyfrujemy symbol zgodnie z kluczem
                            if(symbol_code > 127) // jesli przekroczylismy zakres chara tj. <-128, 127>
                                symbol_code -= 256; // -256, aby powrocic do prawidlowego zakresu
                            cipher_pos = (cipher_pos + 1) % cipher_len; // przesuniecie o 1 w prawo pozycji w kluczu szyfru
                        }
                        fprintf(output, "%c", (char)symbol_code); // drukowanie gotowego znaku do pliku
#ifdef DEBUG
                        // wyswietlenie zapisanego znaku wraz z jego kodem na stderr
                        fprintf(stderr, "Printed symbol: %c (code: %d)\n", (char)symbol_code, symbol_code);
#endif
                        symbol_code = 0; // zerowanie zmiennej oznaczajacej kod symbolu
                    }
                    
                    /* offset - jako ilosc pozostalych nadmiarowych znakow w tablicy po zapisie,
                       z ktorych nie bylo mozliwosci utworzenia pelnego symbolu */
                    offset = buf_pos % comp_level;
                    if(offset != 0) // jesli nie jest rowny zero
                        for(i = 0; i < offset; i++) // to przesuwamy znaki znajdujace sie gdzies w srodku tablicy
                            buffer[i] = buffer[buf_pos - offset + i]; // na jej poczatek
                    buf_pos = offset; // nadpisanie aktualnej pozycji w buforze na wartosc offsetu
                    comp_chars = 0; // zerowanie zmiennej oznaczajacej ilosc gotowych do zapisu znakow
                }
                break;
            }
            iterator = iterator->next;
        }
    }

    // po przejsciu po calym pliku, tworzymy ostatni "niepelny" znak z pozostalych nadmiarowych zapisanych bitow
    if(buf_pos != 0) {
        if(buffer[8 * i] == 1)
            symbol_code -= 128;
        for(j = 1; j < buf_pos; j++)
            symbol_code += buffer[8 * i + j] * (1 << (7 - j));
        fprintf(output, "%c", (char)symbol_code);
    }
}