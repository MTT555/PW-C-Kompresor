#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "cipher.h"
#include "countCharacters.h"
#include "huffman.h"
#include "utils.h"
#define BUF_SIZE 256

int main(int argc, char *argv[]) {
	// Wyswietlenie pomocy pliku w wypadku podania jedynie argumentu --h
	if(argc == 2 && strcmp(argv[1], "--h") == 0) {
		help(argv[0], stdout);
		return 0;
	}

	// Sprawdzenie, czy nazwy plikow na pewno zostaly podane
	if(argc < 3) {
		fprintf(stderr, "Too few arguments!\n\n");
		help(argv[0], stderr);
		return -1;
	}

	//Wczytuje tekst uzytkownika z pliku
	//Nazwe pliku podajemy jako pierwszy argument wywolania
	FILE *in = fopen(argv[1], "r");
	if(in == NULL) {
		fprintf(stderr, "Input file could not be opened!\n");
		return -2;
	}

	//Nazwa pliku, w ktorym znajdzie sie skomresowany plik jako drugi argument
	FILE *out = fopen(argv[2], "wb");
	if(out == NULL) {
		fprintf(stderr, "Output file could not be opened!\n");
		return -3;
	}

	bool cipher = false, var = false, set_comp_level = false; // zmienne pomocnicze do obslugi argumentow --c i --v
	int comp_level = 8; // zmienna pomocnicza do obslugi poziomu kompresji, domyslnie kompresja 8-bitowa
	char compression_mode[7]; // zmienna pomocnicza do przechowywania trybu kompresji

	// Analiza pozostalych argumentow wywolania
	if(argc > 3) {
		int i;
		for(i = 3; i < argc; i++)
			if(strcmp(argv[i], "--c") == 0)
				cipher = true; // argument --c mowiacy, ze wynik dzialania programu ma zostac dodatkowo zaszyfrowany
			else if(strcmp(argv[i], "--v") == 0)
				var = true; // argument --v mowiacy, ze w trakcie dzialania programu maja byc widoczne istotne zmienne
			else if(strcmp(argv[i], "--00") == 0) { // brak kompresji
				if(set_comp_level) {
					fprintf(stderr, "%s - Compression level has already been set to \"%s\"! (skipping...)\n", argv[i], compression_mode);
				}
				else {
					comp_level = 0;
					strcpy(compression_mode, "none");
					set_comp_level = true;
				}
			}
			else if(strcmp(argv[i], "--01") == 0) { // kompresja 8-bit
				if(set_comp_level) {
					fprintf(stderr, "%s - Compression level has already been set to \"%s\"! (skipping...)\n", argv[i], compression_mode);
				}
				else {
					comp_level = 8;
					strcpy(compression_mode, "8-bit");
					set_comp_level = true;
				}
			}
			else if(strcmp(argv[i], "--02") == 0) { // kompresja 12-bit
				if(set_comp_level) {
					fprintf(stderr, "%s - Compression level has already been set to \"%s\"! (skipping...)\n", argv[i], compression_mode);
				}
				else {
					comp_level = 12;
					strcpy(compression_mode, "12-bit");
					set_comp_level = true;
				}
			}
			else if(strcmp(argv[i], "--03") == 0) { // kompresja 16-bit
				if(set_comp_level) {
					fprintf(stderr, "%s - Compression level has already been set to \"%s\"! (skipping...)\n", argv[i], compression_mode);
				}
				else {
					comp_level = 16;
					strcpy(compression_mode, "16-bit");
					set_comp_level = true;
				}
			}
			else if(strcmp(argv[i], "--h") == 0) // wyswietlenie pomocy
				help(argv[0], stdout);
			else // pominiecie niezidentyfikowanych argumentow
				fprintf(stderr, "%s - Unidentified argument! (skipping...)\n", argv[i]);
	}
	
	count *head; //tworze glowe listy w ktorej bede przechowywal zliczenia
	runCounter(&head);

	char c;
	//wczytuje i zliczam znak po znaku
	while((c=fgetc(in)) != EOF) {

		if(checkIfElementIsOnTheList(&head, c) == 1){
		addToTheList(&head, c);
		}

	}

	sortTheList(&head);//sortuje liste wystapien znakow niemalejaco
#ifdef DEBUG

	//wypisujemy liste z wystapieniami
	
	showList(&head);
#endif
	fseek(in, 0, SEEK_SET); // ustawienie kursora w pliku z powrotem na jego poczatek
	
	huffman(in, out, comp_level, cipher, "Politechnika_Warszawska", &head);

	fclose(in);
	// fclose(out);
	return 0;
}
