# **Kompresor algorytmem Huffmana**
### ***Adrian Chmiel, Mateusz Tyl***
\
Program w języku C realizujący bezstratną kompresję oraz dekompresję plików wykonaną algorytmem Huffmana. Składa się on z następujących plików:
* **gen**
  * **gen.c** -> służy do generowania gotowych testów do tego kompresora \
    Użycie *(pola \<charactersRange> i \<seed> można pominąć)*:
    > `./generate <outputFile> <charactersAmount> <charactersRange> <seed>` 
  * **testhufftree.c** -> odpowiedzialny za wygenerowanie testu w wyniku którego możliwe było przeciążenie pamięci przez tworzenie drzewa Huffmana
  * **testmax.c** -> odpowiedzialny za wygenerowanie testu o wielkości ok. 1.5 MB
  * **testo3.c** -> odpowiedzialny za wygenerowanie testu testo3.in zawierającego wszystkie możliwe znaki/słowa dla wszystkich możliwych poziomów kompresji
* **src**
  * **alloc.\*** -> zawiera funkcje służące do bezpiecznej alokacji pamięci
  * **bits_analyze.\*** -> zawiera funkcje służące do analizy kolejnych bitów zawartych kolejnych znakach przy dekompresji
  * **countCharacters.\*** -> zawiera funkcje odpowiedzialne za zliczanie wystąpień wszystkich znaków w pliku wejściowym
  * **decompress.\*** -> zawiera funkcje odpowiedzialne za dekompresję
  * **dtree.\*** -> zawiera typ pomocniczego pseudodrzewa do dekompresji oraz związane z nim funkcje
  * **huffman.\*** -> zawiera funkcje realizujące algorytm Huffmana
  * **list.\*** -> zawiera implementację listy liniowej
  * **main.c** -> odpowiada za sprawdzanie danych wejściowych oraz sterowanie całym programem
  * **noCompress.\*** -> zawiera funkcje obsługujące plik w wypadku, gdy nie zachodzi żadna kompresja
  * **output.\*** -> zawiera funkcje realizujące zapis skompresowanego pliku
  * **utils.\*** -> zawiera pomocnicze typy oraz funkcje
* **test** -> katalog zawierający przykładowe testy
* **CMake.sh** -> skrypt automatyzujący kompilację przy pomocy CMake
* **CMakeLists.txt** -> zawiera komendy do kompilacji przy użyciu CMake
* **compare.c** -> służy do porównywania, czy dwa podane pliki są jednakowe (weryfikacja, czy otrzymaliśmy plik początkowy po wykonaniu kompresji, a następnie dekompresji) \
  Użycie:
  > `./compare <file1> <file2>` 
* **encoding.sh** -> skrypt sprawdzający na wiele sposobów kodowanie danego pliku *(może wymagać instalacji dodatkowych pakietów)*
* **Makefile** -> zawiera komendy do standardowej kompilacji oraz przykładowe testy programu
* **Readme.md** - właśnie go czytasz :)
* **Specyfikacja_funkcjonalna.pdf** - faktyczna specyfikacja funkcjonalna
* **Specyfikacja_funkcjonalna.tex** - kod specyfikacji funkcjonalnej zapisany w LaTeX
* **Specyfikacja_implementacyjna.pdf** - faktyczna specyfikacja implementacyjna
* **Specyfikacja_implementacyjna.tex** - kod specyfikacji implemetacyjnej zapisany w LaTeX

W celu wyświetlenia prawidłowego sposobu uruchamiania programu głównego można go po prostu uruchomić bez podawania żadnych argumentów wejściowych (bądź jedynie z argumentem -h).

> `./compressor` \
> `./compressor -h`
