#include <stdio.h>
#include <string.h>
#include "countCharacters.h"
#include "huffman.h"
#include "utils.h"
#include "decompress.h"
#include "list.h"

int main(int argc, char **argv) {
	uchar c;
	int i, inputEOF;
	count_t *head = NULL;
	FILE *in, *out;
	bool cipher = false, comp = false, decomp = false; /* zmienne pomocnicze do obslugi argumentow -c -v -x -d */
	int compLevel = 8; /* zmienna pomocnicza do obslugi poziomu kompresji, domyslnie kompresja 8-bitowa */
	
	/* Wyswietlenie pomocy pliku w wypadku podania jedynie argumentu --h */
	if(argc == 1 || (argc == 2 && strcmp(argv[1], "-h") == 0)) {
		help(stderr);
		return 0;
	}

	/* Sprawdzenie, czy podano zarowno plik wejsciowy, jak i wyjsciowy */
	if(argc < 3) {
		fprintf(stderr, "%s: Too few arguments!\n", argv[0]);
		return 1;
	}

	/* Nazwa pliku, z ktorego dane beda wczytywane */
	in = fopen(argv[1], "rb");
	if(in == NULL) {
		fprintf(stderr, "%s: Input file could not be opened!\n", argv[0]);
		return 2;
	}

	/* Sprawdzenie, czy nie podano pustego pliku wejsciowego */
	fseek(in, 0, SEEK_END);
	inputEOF = ftell(in); /* znalezienie konca pliku */
	fseek(in, 0, SEEK_SET);	
	if(!inputEOF) {
		fclose(in);
		fprintf(stderr, "%s: Input file is empty!\n", argv[0]);
		return 4;
	}

	/* Nazwa pliku, w ktorym znajdzie sie plik wyjsciowy */
	out = fopen(argv[2], "wb");
	if(out == NULL) {
		fclose(in);
		fprintf(stderr, "%s: Output file could not be opened!\n", argv[0]);
		return 3;
	}

	/* Analiza pozostalych argumentow wywolania */
	analyzeArgs(argc, argv, &cipher, &comp, &decomp, &compLevel);
	
	if(!comp && !decomp) { /* jezeli nie wymuszono zachowania programu, sprawdzamy plik */
		if(fileIsGood(in, (uchar)183, false)) /* (183 = 0b10110111) */
			comp = true;
		else
			decomp = true;
	}
	
	int tempCode = 0, currentBits = 0; /* tymczasowy kod wczytanego znaku oraz ilosc obecne wczytanych bitow */

	if(comp) { /* jezeli ma zostac wykonana kompresja */
		if(compLevel == 0 && !cipher) {
			fprintf(stderr, "%s: Due to chosen settings, file has been rewritten to \"%s\" with no changes!\n", argv[0], argc > 2 ? argv[2] : "stdout");
			for(i = 0; i < inputEOF; i++) {
				fread(&c, sizeof(char), 1, in);
				fwrite(&c, sizeof(char), 1, out);
			}
		} else if(compLevel == 0 && cipher) {
			uchar xor = (uchar)183; /* (183 = 0b10110111) */
			uchar cipher_key[] = "Politechnika_Warszawska";
			int cipher_pos = 0;
			int cipher_len = (int)strlen((char *)cipher_key);
			fprintf(out, "CT%cX", (uchar)40); /* zapalone bity szyfrowania i kompresji (40 == 0b00101000) */
			for(i = 0; i < inputEOF; i++) {
				fread(&c, sizeof(char), 1, in);
				c += cipher_key[cipher_pos % cipher_len];
				cipher_pos++;
				fwrite(&c, sizeof(char), 1, out);
				xor ^= c;
			}
			fseek(out, 3, SEEK_SET);
			fwrite(&xor, sizeof(char), 1, out);
		} else {
			/* wczytuje i zliczam znak po znaku */
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
				} else if (currentBits >= compLevel) { /* taki przypadek wystapi jedynie w kompresji 12-bit */
					int temp = tempCode % 16;
					tempCode >>= 4;
					if(checkIfOnTheList(&head, tempCode) == 1)
						head = addToTheList(&head, tempCode);
					tempCode = temp;
					currentBits = 4;
				}
			}

			sortTheCountList(&head); /* sortowanie listy wystapien znakow niemalejaco */
#ifdef DEBUG
			/* wypisanie listy z wystapieniami */
			showList(&head, stderr);
#endif
			fseek(in, 0, SEEK_SET); /* ustawienie kursora w pliku z powrotem na jego poczatek */
			huffman(in, out, compLevel, cipher, &head);
			freeRecursively(head);
			free(head);
		}
	}
	else if(decomp) { /* jezeli ma zostac wykonana dekompresja */
		int fileCheck = fileIsGood(in, (uchar)183, true); /* (183 = 0b10110111) */
#ifdef DEBUG
		fprintf(stderr, "File check code: %d\n", fileCheck);
#endif
		if(!fileCheck) { /* dekompresja jedynie, jezeli fileCheck zwrocil 0 */
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