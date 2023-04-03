#!/bin/bash

# Komendy zawarte w komentarzach sa dostosowane pod instalowanie
# potrzebnych pakietów w dystrybucji openSUSE

if [[ $# -eq 1 && -f "$1" ]]; then
    echo "Encoding of file \"$1\""
    echo "File -i:"
    file -i $1
    echo ""

    # sudo zypper apt install enca -y
    echo "Enca:"
    enca $1
    echo ""

    # sudo zypper apt install uchardet -y
    echo "Uchardet:"
    uchardet $1
    echo ""

    # sudo zypper apt install perl -y
    echo "Encguess:"
    encguess $1
    echo ""

    # sudo zypper install nodejs npm -y
    # npm install -g detect-file-encoding-and-language
    echo "Dfeal:"
    dfeal $1
    echo ""

    # sudo zypper install moreutils -y
    echo "IsUTF8 (no output below means it's UTF-8):"
    isutf8 $1
    echo ""
else
    echo "$0: Invalid usage!"
    echo ""
    echo "---------------------------------------------------------------"
    echo ""
    echo "Check the encoding of any file with many programs using:"
    echo "$0 [file_name]"
    echo ""
    echo "---------------------------------------------------------------"
fi