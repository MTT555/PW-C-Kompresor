# **Kompresor algorytmem Huffmana**
### ***Adrian Chmiel, Mateusz Tyl***
\
Program w języku C realizujący bezstratną kompresję oraz dekompresję plików wykonaną algorytmem Huffmana. Składa się on z następujących plików:
* **gen**
  * **gen.c** -> służy do generowania gotowych testów do tego kompresora \
    Użycie *(pola \<charactersRange> i \<seed> można pominąć)*:
    > `./generate <outputFile> <charactersAmount> <charactersRange> <seed>` 
  * **testo3.c** -> odpowiedzialny za wygenerowanie testu testo3.in zawierającego wszystkie możliwe znaki dla wszystkich możliwych poziomów kompresji
* **src**
  * **countCharacters.\*** -> zawiera funkcje odpowiedzialne za zliczanie wystąpień wszystkich znaków w pliku wejściowym
  * **decompress.\*** -> zawiera funkcje odpowiedzialne za dekompresję
  * **huffman.\*** -> zawiera funkcje realizujące algorytm Huffmana
  * **main.c** -> odpowiada za sprawdzanie danych wejściowych oraz sterowanie całym programem
  * **output.\*** -> zawiera funkcje realizujące zapis skompresowanego pliku
  * **utils.\*** -> zawiera pomocnicze typy oraz funkcje
* **test** -> katalog zawierający przykładowe testy
* **CMake.sh** -> skrypt automatyzujący kompilację przy pomocy CMake
* **CMakeLists.txt** -> zawiera komendy do kompilacji przy użyciu CMake
* **compare.c** -> służy do porównywania, czy dwa podane pliki są jednakowe (weryfikacja, czy otrzymaliśmy plik początkowy po wykonaniu kompresji, a następnie dekompresji) \
  Użycie:
  > `./compare <file1> <file2>` 
* **Makefile** -> zawiera komendy do standardowej kompilacji
* **README.md** - właśnie go czytasz :)

W celu wyświetlenia prawidłowego sposobu uruchamiania programu głównego można go po prostu uruchomić bez podawania żadnych argumentów wejściowych (bądź jedynie z argumentem -h).

> `./compressor` \
> `./compressor -h`