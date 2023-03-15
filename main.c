#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "cipher.h"
#include "countCharacters.h"
#include "huffman.h"
#include "utils.h"
#include "decompress.h"

int main(int argc, char *argv[]) {
	char c;
	int i;
	
	// Wyswietlenie pomocy pliku w wypadku podania jedynie argumentu --h
	if(argc == 2 && strcmp(argv[1], "-h") == 0) {
		help(stdout);
		return 0;
	}

	// Sprawdzenie, czy nazwy plikow na pewno zostaly podane
	if(argc < 3) {
		fprintf(stderr, "%s: Too few arguments!\n\n", argv[0]);
		help(stderr);
		return 1;
	}

	// Wczytuje tekst uzytkownika z pliku
	// Nazwe pliku podajemy jako pierwszy argument wywolania
	FILE *in = fopen(argv[1], "r");
	if(in == NULL) {
		fprintf(stderr, "%s: Input file could not be opened!\n", argv[0]);
		return 2;
	}
	// Sprawdzenie, czy nie podano pustego pliku
	if((c = fgetc(in)) == EOF) {
		fclose(in);
		fprintf(stderr, "%s: Input file is empty!\n", argv[0]);
		return 4;
	}
	// Nazwa pliku, w ktorym znajdzie sie skomresowany plik jako drugi argument
	FILE *out = fopen(argv[2], "w");
	if(out == NULL) {
		fclose(in);
		fprintf(stderr, "%s: Output file could not be opened!\n", argv[0]);
		return 3;
	}
	
	fseek(in, 0, SEEK_END);
	int input_eof = ftell(in); // znalezienie konca pliku
	fseek(in, 0, SEEK_SET);

	bool cipher = false, set_comp_level = false, comp = false, decomp = false; // zmienne pomocnicze do obslugi argumentow -c -v -x -d
	bool help_displayed = false; // zmienna zapobiegajaca wielokrotnemu wyswietlaniu helpboxa
	int comp_level = 8; // zmienna pomocnicza do obslugi poziomu kompresji, domyslnie kompresja 8-bitowa
	char comp_mode[7]; // zmienna pomocnicza do przechowywania trybu kompresji
	char prog_behaviour[20]; // zmienna pomocnicza do przechowywania zachowania programu (wymuszenie kompresji/dekompresji)

	// Analiza pozostalych argumentow wywolania
	if(argc > 3) {
		int i;
		for(i = 3; i < argc; i++)
			if(strcmp(argv[i], "-c") == 0) {
				cipher = true; // argument -c mowiacy, ze wynik dzialania programu ma zostac dodatkowo zaszyfrowany
				printf("%s: Output encryption has been enabled!\n", argv[0]);
			}
			else if(strcmp(argv[i], "-DDEBUG") == 0)
				printf("%s: Debug mode has been enabled - program will now display various variables and additional information during runtime in stderr!\n", argv[0]);
			else if(strcmp(argv[i], "-o0") == 0) { // brak kompresji
				if(set_comp_level) {
					fprintf(stderr, "%s: %s -> Compression level has already been set to \"%s\"! (ignoring...)\n", argv[0], argv[i], comp_mode);
				}
				else {
					comp_level = 0;
					strcpy(comp_mode, "none");
					set_comp_level = true;
					printf("%s: Compression mode has been set to %s!\n", argv[0], comp_mode);
				}
			}
			else if(strcmp(argv[i], "-o1") == 0) { // kompresja 8-bit
				if(set_comp_level) {
					fprintf(stderr, "%s: %s -> Compression level has already been set to \"%s\"! (ignoring...)\n", argv[0], argv[i], comp_mode);
				}
				else {
					comp_level = 8;
					strcpy(comp_mode, "8-bit");
					set_comp_level = true;
					printf("%s: Compression mode has been set to %s!\n", argv[0], comp_mode);
				}
			}
			else if(strcmp(argv[i], "-o2") == 0) { // kompresja 12-bit
				if(set_comp_level) {
					fprintf(stderr, "%s: %s -> Compression level has already been set to \"%s\"! (ignoring...)\n", argv[0], argv[i], comp_mode);
				}
				else {
					comp_level = 12;
					strcpy(comp_mode, "12-bit");
					set_comp_level = true;
					printf("%s: Compression mode has been set to %s!\n", argv[0], comp_mode);
				}
			}
			else if(strcmp(argv[i], "-o3") == 0) { // kompresja 16-bit
				if(set_comp_level) {
					fprintf(stderr, "%s: %s -> Compression level has already been set to \"%s\"! (ignoring...)\n", argv[0], argv[i], comp_mode);
				}
				else {
					comp_level = 16;
					strcpy(comp_mode, "16-bit");
					set_comp_level = true;
					printf("%s: Compression mode has been set to %s!\n", argv[0], comp_mode);
				}
			}
			else if(strcmp(argv[i], "-h") == 0) {
				if(!help_displayed) { // wyswietlenie pomocy
					help(stdout);
					help_displayed = true;
				}
			}
			else if(strcmp(argv[i], "-x") == 0) // wymuszenie kompresji
				if(comp)
					fprintf(stderr, "%s: %s -> Program behaviour has already been set to: \"%s\"! (ignoring...)\n", argv[0], argv[i], prog_behaviour);
				else {
					comp = true;
					strcpy(prog_behaviour, "force compression");
				}
			else if(strcmp(argv[i], "-d") == 0) // wymuszenie dekompresji
				if(comp)
					fprintf(stderr, "%s: %s -> Program behaviour has already been set to: \"%s\"! (ignoring...)\n", argv[0], argv[i], prog_behaviour);
				else {
					decomp = true;
					strcpy(prog_behaviour, "force decompression");
				}
			else // pominiecie niezidentyfikowanych argumentow
				fprintf(stderr, "%s: %s -> Unknown argument! (ignoring...)\n", argv[0], argv[i]);
	}

	// jezeli nie wymuszono zachowania programu, sprawdzamy plik
	if(!comp && !decomp) {
		if(fileIsGood(in, (char)0b10110111, false))
			comp = true;
		else
			decomp = true;
	}

	if(comp) { // jezeli ma zostac wykonana kompresja
		count *head; //tworze glowe listy w ktorej bede przechowywal zliczenia
		runCounter(&head);

		// wczytuje i zliczam znak po znaku
		for(i = 0 ; i < input_eof; i++) {
			c = fgetc(in);
			if(checkIfElementIsOnTheList(&head, c) == 1) {
				addToTheList(&head, c);
			}
		}

		sortTheList(&head); //sortuje liste wystapien znakow niemalejaco
#ifdef DEBUG
		//wypisujemy liste z wystapieniami
		showList(&head);
#endif
		fseek(in, 0, SEEK_SET); // ustawienie kursora w pliku z powrotem na jego poczatek
		huffman(in, out, comp_level, cipher, &head);
		freeList(&head);
	}
	else if(decomp) { // jezeli ma zostac wykonana dekompresja
		int fileCheck = fileIsGood(in, (char)0b10110111, true);
#ifdef DEBUG
		fprintf(stderr, "File check code: %d\n", fileCheck);
#endif
		if(!fileCheck) { // jezeli fileCheck zwroci 0
			decompress(in, out);
		} else {
			fclose(in);
			fclose(out);
			return 5;
		}
	}

	fclose(in);
	fclose(out);
	return 0;
}
