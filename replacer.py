#!/usr/bin/python3
# -*-coding:Utf-8 -*
#Deus, in adjutorium meum intende

import os
import sys

def replace(from_ : str, to_ : str):
    """Looks into every file of the 'frumul' dir
    and replace from_ string by to_ string
    """
    folder = 'frumul'
    
    for file in os.listdir(folder):
        try:
            with open(f'{folder}/{file}') as f:
                content = f.read()

            content = content.replace(from_, to_)

            with open(f'{folder}/{file}','w') as f:
                f.write(content)

        except (IsADirectoryError, UnicodeDecodeError):
            continue

if __name__ == '__main__':
    replace(sys.argv[1],sys.argv[2])



