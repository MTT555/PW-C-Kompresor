#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "cipher.h"
#include "countCharacters.h"
#include "utils.h"
#define BUF_SIZE 256

int main(int argc, char *argv[]) {
	// Sprawdzenie, czy nazwy plikow na pewno zostaly podane
	if(argc < 3) {
		fprintf(stderr, "Too few arguments!\n\n");
		help(argv[0], stderr);
		return -1;
	}

	//Wczytuje tekst uzytkownika z pliku
	//Nazwe pliku podajemy jako pierwszy argument wywolania
	FILE *in = fopen(argv[1], "r");

	//Nazwa pliku, w ktorym znajdzie sie skomresowany plik jako drugi argument
	FILE *out = fopen(argv[2], "w");

	// Analiza pozostalych argumentow wywolania
	if(argc > 3) {
		int i;
		for(i = 3; i < argc; i++)
			if(strcmp(argv[i], "--c") == 0) {
				// plik ma zostac zaszyfrowany
			}
			else if(strcmp(argv[i], "--v") == 0) {
				// plik ma pokazywac najistotniejsze zmienne
			}
			else if(strcmp(argv[i], "--00") == 0) {
				// brak kompresji
			}
			else if(strcmp(argv[i], "--01") == 0) {
				// kompresja 8-bit
			}
			else if(strcmp(argv[i], "--02") == 0) {
				// kompresja 12-bit
			}
			else if(strcmp(argv[i], "--03") == 0) {
				// kompresja 16-bit
			}
			else if(strcmp(argv[i], "--h") == 0) { // wyswietlenie pomocy
				help(argv[0], stdout);
			}
			else // pominiecie niezidentyfikowanych argumentow
				fprintf(stderr, "%s - Unidentified argument! (skipping...)\n", argv[i]);
	}
	
	count *head; //tworze glowe listy w ktorej bede przechowywal zliczenia
	runCounter(&head);

	char c;
	//wczytuje i zliczam znak po znaku
	while((c=fgetc(in)) != EOF){

	if(checkIfElementIsOnTheList(&head, c) == 1){
	addToTheList(&head, c);
	}

	}
	fclose(in);

#ifdef DEBUG

	//wypisujemy liste z wystapieniami
	
	showList(&head);
#endif

	char test[] = "aaaa";
	printf("%s\n", test);
	vigenere(test, 4, "~~~~", 4);
	printf("%s\n", test);

	//fclose(out);
	return 0;
}
