#!/usr/bin/python3
# -*-coding:Utf-8 -*
#Deus, in adjutorium meum intende

import os, sys

def find(expr : str):
    """Looks into every file of the 'frumul' dir
    and print them on the screen if
    they contain expr
    """
    folder = 'frumul'
    for file in os.listdir(folder):
        try:
            with open(f'{folder}/{file}') as f:
                if expr in f.read():
                    print(file)
        except (IsADirectoryError, UnicodeDecodeError):
            continue

if __name__ == "__main__":
    find(sys.argv[1])
