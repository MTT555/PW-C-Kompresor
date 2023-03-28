#include <stdio.h>
#include <string.h>
#include "countCharacters.h"
#include "huffman.h"
#include "utils.h"
#include "decompress.h"

int main(int argc, char **argv) {
	unsigned char c;
	int i;
	count_t *head = NULL;
	
	// Wyswietlenie pomocy pliku w wypadku podania jedynie argumentu --h
	if(argc == 1 || (argc == 2 && strcmp(argv[1], "-h") == 0)) {
		help(stderr);
		return 0;
	}

	// Sprawdzenie, czy podano zarowno plik wejsciowy, jak i wyjsciowy
	if(argc < 3) {
		fprintf(stderr, "%s: Too few arguments!\n", argv[0]);
		return 1;
	}

	// Nazwa pliku, z ktorego dane beda wczytywane
	FILE *in = fopen(argv[1], "rb");
	if(in == NULL) {
		fprintf(stderr, "%s: Input file could not be opened!\n", argv[0]);
		return 2;
	}

	// Sprawdzenie, czy nie podano pustego pliku wejsciowego
	fseek(in, 0, SEEK_END);
	int inputEOF = ftell(in); // znalezienie konca pliku
	fseek(in, 0, SEEK_SET);	
	if(!inputEOF) {
		fclose(in);
		fprintf(stderr, "%s: Input file is empty!\n", argv[0]);
		return 4;
	}

	// Nazwa pliku, w ktorym znajdzie sie plik wyjsciowy
	FILE *out = fopen(argv[2], "wb");
	if(out == NULL) {
		fclose(in);
		fprintf(stderr, "%s: Output file could not be opened!\n", argv[0]);
		return 3;
	}

	bool cipher = false, set_compLevel = false, comp = false, decomp = false; // zmienne pomocnicze do obslugi argumentow -c -v -x -d
	bool help_displayed = false; // zmienna zapobiegajaca wielokrotnemu wyswietlaniu helpboxa
	int compLevel = 8; // zmienna pomocnicza do obslugi poziomu kompresji, domyslnie kompresja 8-bitowa
	char comp_mode[7]; // zmienna pomocnicza do przechowywania trybu kompresji
	char prog_behaviour[20]; // zmienna pomocnicza do przechowywania zachowania programu (wymuszenie kompresji/dekompresji)

	// Analiza pozostalych argumentow wywolania
	if(argc > 3) {
		for(i = 3; i < argc; i++)
			if(strcmp(argv[i], "-c") == 0) {
				cipher = true; // argument -c mowiacy, ze wynik dzialania programu ma zostac dodatkowo zaszyfrowany
				fprintf(stderr, "%s: Output encryption has been enabled!\n", argv[0]);
			} else if(strcmp(argv[i], "-o0") == 0) { // brak kompresji
				if(set_compLevel) {
					fprintf(stderr, "%s: %s -> Compression level has already been set to \"%s\"! (ignoring...)\n", argv[0], argv[i], comp_mode);
				}
				else {
					compLevel = 0;
					strcpy(comp_mode, "none");
					set_compLevel = true;
					comp = true;
					strcpy(prog_behaviour, "force compression");
					fprintf(stderr, "%s: Compression mode has been set to %s and program behaviour has been changed to \"force compression\"!\n", argv[0], comp_mode);
				}
			} else if(strcmp(argv[i], "-o1") == 0) { // kompresja 8-bit
				if(set_compLevel) {
					fprintf(stderr, "%s: %s -> Compression level has already been set to \"%s\"! (ignoring...)\n", argv[0], argv[i], comp_mode);
				}
				else {
					compLevel = 8;
					strcpy(comp_mode, "8-bit");
					set_compLevel = true;
					comp = true;
					strcpy(prog_behaviour, "force compression");
					fprintf(stderr, "%s: Compression mode has been set to %s and program behaviour has been changed to \"force compression\"!\n", argv[0], comp_mode);
				}
			} else if(strcmp(argv[i], "-o2") == 0) { // kompresja 12-bit
				if(set_compLevel) {
					fprintf(stderr, "%s: %s -> Compression level has already been set to \"%s\"! (ignoring...)\n", argv[0], argv[i], comp_mode);
				}
				else {
					compLevel = 12;
					strcpy(comp_mode, "12-bit");
					set_compLevel = true;
					comp = true;
					strcpy(prog_behaviour, "force compression");
					fprintf(stderr, "%s: Compression mode has been set to %s and program behaviour has been changed to \"force compression\"!\n", argv[0], comp_mode);
				}
			} else if(strcmp(argv[i], "-o3") == 0) { // kompresja 16-bit
				if(set_compLevel) {
					fprintf(stderr, "%s: %s -> Compression level has already been set to \"%s\"! (ignoring...)\n", argv[0], argv[i], comp_mode);
				}
				else {
					compLevel = 16;
					strcpy(comp_mode, "16-bit");
					set_compLevel = true;
					comp = true;
					strcpy(prog_behaviour, "force compression");
					fprintf(stderr, "%s: Compression mode has been set to %s and program behaviour has been changed to \"force compression\"!\n", argv[0], comp_mode);
				}
			} else if(strcmp(argv[i], "-h") == 0) {
				if(!help_displayed) { // wyswietlenie pomocy
					help(stderr);
					help_displayed = true;
				}
			} else if(strcmp(argv[i], "-x") == 0) { // wymuszenie kompresji
				if(comp || decomp)
					fprintf(stderr, "%s: %s -> Program behaviour has already been set to: \"%s\"! (ignoring...)\n", argv[0], argv[i], prog_behaviour);
				else {
					comp = true;
					strcpy(prog_behaviour, "force compression");
					fprintf(stderr, "%s: Program behaviour has been set to \"%s\"!\n", argv[0], prog_behaviour);
				}
			} else if(strcmp(argv[i], "-d") == 0) { // wymuszenie dekompresji
				if(comp || decomp)
					fprintf(stderr, "%s: %s -> Program behaviour has already been set to: \"%s\"! (ignoring...)\n", argv[0], argv[i], prog_behaviour);
				else {
					decomp = true;
					strcpy(prog_behaviour, "force decompression");
					fprintf(stderr, "%s: Program behaviour has been set to \"%s\"!\n", argv[0], prog_behaviour);
				}
			} else // pominiecie niezidentyfikowanych argumentow
				fprintf(stderr, "%s: %s -> Unknown argument! (ignoring...)\n", argv[0], argv[i]);
	}

	
	if(!comp && !decomp) { // jezeli nie wymuszono zachowania programu, sprawdzamy plik
		if(fileIsGood(in, (unsigned char)0b10110111, false))
			comp = true;
		else
			decomp = true;
	}
	
	int tempCode = 0, currentBits = 0; // tymczasowy kod wczytanego znaku oraz ilosc obecne wczytanych bitow

	if(comp) { // jezeli ma zostac wykonana kompresja
		if(compLevel == 0 && !cipher) {
			fprintf(stderr, "%s: Due to chosen settings, file has been rewritten to \"%s\" with no changes!\n", argv[0], argc > 2 ? argv[2] : "stdout");
			for(i = 0; i < inputEOF; i++) {
				fread(&c, sizeof(char), 1, in);
				fprintf(out, "%c", c);
			}
		} else if(compLevel == 0 && cipher) {
			unsigned char xor = (unsigned char)0b10110111;
			unsigned char cipher_key[] = "Politechnika_Warszawska";
			int cipher_pos = 0;
			int cipher_len = (int)strlen(cipher_key);
			fprintf(out, "CT%cX", (unsigned char)0b00101000); // zapalone bity szyfrowania i kompresji, zeby dzialala funkcja fileIsGood()
			for(i = 0; i < inputEOF; i++) {
				fread(&c, sizeof(char), 1, in);
				c += cipher_key[cipher_pos % cipher_len];
				cipher_pos++;
				fprintf(out, "%c", c);
				xor ^= c;
			}
			fseek(out, 3, SEEK_SET);
			fprintf(out, "%c", xor);
		} else {
			// wczytuje i zliczam znak po znaku
			for(i = 0; i <= inputEOF; i++) {
				if(i != inputEOF)
					fread(&c, sizeof(char), 1, in);
				else if((compLevel == 12 && (currentBits == 8 || currentBits == 4)) || (compLevel == 16 && currentBits == 8))
					c = '\0';
				else
					break;
				
				currentBits += 8;
				tempCode <<= 8;
				tempCode += (int)c;
				if(currentBits == compLevel) {
					if(checkIfOnTheList(&head, tempCode) == 1)
						head = addToTheList(&head, tempCode);
					tempCode = 0;
					currentBits = 0;
				} else if (currentBits >= compLevel) { // taki przypadek wystapi jedynie w kompresji 12-bit
					int temp = tempCode % 16;
					tempCode >>= 4;
					if(checkIfOnTheList(&head, tempCode) == 1)
						head = addToTheList(&head, tempCode);
					tempCode = temp;
					currentBits = 4;
				}
			}

			sortTheCountList(&head); // sortowanie listy wystapien znakow niemalejaco
#ifdef DEBUG
			// wypisanie listy z wystapieniami
			showList(&head, stderr);
#endif
			fseek(in, 0, SEEK_SET); // ustawienie kursora w pliku z powrotem na jego poczatek
			huffman(in, out, compLevel, cipher, &head);
			freeRecursively(head);
		}
	}
	else if(decomp) { // jezeli ma zostac wykonana dekompresja
		int fileCheck = fileIsGood(in, (unsigned char)0b10110111, true);
#ifdef DEBUG
		fprintf(stderr, "File check code: %d\n", fileCheck);
#endif
		if(!fileCheck) { // dekompresja jedynie, jezeli fileCheck zwrocil 0
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