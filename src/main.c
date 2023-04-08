#include <stdio.h>
#include <string.h>
#include "countCharacters.h"
#include "huffman.h"
#include "utils.h"
#include "decompress.h"
#include "list.h"
#include "alloc.h"
#include "noCompress.h"

int main(int argc, char **argv) {
	/* Deklaracja zmiennych */
	uchar c;
	int i, inputEOF, fileCheck, temp;
	count_t *head = NULL, *tempPtr = NULL;
	FILE *in, *out;
	int tempCode = 0, currentBits = 0; /* tymczasowy kod wczytanego znaku oraz ilosc obecnie wczytanych bitow (dla kompresji 12- i 16-bit) */
	
	settings_t s;
	s.comp = false; /* domyslnie wylaczone wymuszenie kompresji i dekompresji */
	s.decomp = false;
	s.cipher = false; /* domyslnie szyfrowanie wylaczone */
	s.compLevel = 8; /* domyslna kompresja 8-bit */
	strcpy((char *)s.cipherKey, "Politechnika_Warszawska");
	
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
	analyzeArgs(argc, argv, &s);
	
#ifdef DEBUG
	/* Wydrukowanie debug info na stderr */
	fprintf(stderr, "The following settings have been chosen:\n");
	fprintf(stderr, "Force compression: %s, force decompression: %s, compLevel: %d, cipher: %s\n",
		s.comp ? "true" : "false", s.decomp ? "true" : "false", s.compLevel, s.cipher ? "true" : "false");
	if(s.cipher)
		fprintf(stderr, "Cipher key: %s\n", s.cipherKey);
#endif

	if(!s.comp && !s.decomp) { /* jezeli nie wymuszono zachowania programu, sprawdzamy plik */
		if(fileIsGood(in, (uchar)183, false)) /* (183 = 0b10110111) */
			s.comp = true;
		else
			s.decomp = true;
	}

	if(s.comp) { /* jezeli ma zostac wykonana kompresja */
		if(s.compLevel == 0){
			rewriteFile(in, out, inputEOF, s);}
		else {
			/* wczytuje i zliczam znak po znaku */
			for(i = 0; i <= inputEOF; i++) {
				if(i != inputEOF)
					fread(&c, sizeof(char), 1, in);
				else if((s.compLevel == 12 && (currentBits == 8 || currentBits == 4)) || (s.compLevel == 16 && currentBits == 8))
					c = '\0';
				else
					break;
				
				currentBits += 8;
				tempCode <<= 8;
				tempCode += (int)c;
				if(currentBits == s.compLevel) {
					if(checkIfOnTheList(&head, tempCode) == 1) {
						tempPtr = head;
						head = addToTheList(&head, tempCode);
						if(head == NULL) {
							fprintf(stderr, "%s: Compression memory failure!\n", argv[0]);
							fclose(in);
							fclose(out);
							freeRecursively(tempPtr);
							free(tempPtr);
							return 6;
						}
					}
					tempCode = 0;
					currentBits = 0;
				} else if (currentBits >= s.compLevel) { /* taki przypadek wystapi jedynie w kompresji 12-bit */
					temp = tempCode % 16;
					tempCode >>= 4;
					if(checkIfOnTheList(&head, tempCode) == 1) {
						tempPtr = head;
						head = addToTheList(&head, tempCode);
						if(head == NULL) {
							fprintf(stderr, "%s: Compression memory failure!\n", argv[0]);
							fclose(in);
							fclose(out);
							freeRecursively(tempPtr);
							free(tempPtr);
							return 6;
						}
					}
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
			if(!huffman(in, out, s, &head)) {
				fprintf(stderr, "%s: Decompression memory failure!\n", argv[0]);
				freeRecursively(head);
				free(head);
				fclose(in);
				fclose(out);
				return 6;
			}
			freeRecursively(head);
			free(head);
		}
	}
	else if(s.decomp) { /* jezeli ma zostac wykonana dekompresja */
		fileCheck = fileIsGood(in, (uchar)183, true); /* (183 = 0b10110111) */
#ifdef DEBUG
		fprintf(stderr, "File check code: %d\n", fileCheck);
#endif
		if(!fileCheck) { /* dekompresja jedynie, jezeli fileCheck zwrocil 0 */
			if(!decompress(in, out, s)) {
				fprintf(stderr, "%s: Decompression memory failure!\n", argv[0]);
				fclose(in);
				fclose(out);
				return 6;
			}
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