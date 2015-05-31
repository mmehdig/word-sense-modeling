# -*- coding: utf-8 -*-
"""
1. Pre-process the English corpus:
read the ".conll" format mixed pipeline source file (the english version)
create two files:
a) Original raw english corpus line by line
b) POS-tagged and lemmatized english

2. Pre-process the Swedish:


3. Create the final parallel file
read lines of both swedish and english lemmatized and POS-tagged file
write them in fastalign format <line_en> ||| <line_sv>



"""
__author__ = 'mehdi'

import sys

if __name__ == "__main__":

    if len(sys.argv) < 3:
        sys.exit('Please set english conll file and swedish lemmatized file: %s <en_conll_file> <sv_lemmatized_file>' % sys.argv[0])

    en_file_name = sys.argv[1]
    sv_file_name = sys.argv[2]

    with open(en_file_name) as en_file:
        with open(en_file_name + ".l_lraw.txt", "w") as out_en_raw_file:
            raw = ""
            for line in en_file:
                if line[0] == "\n":
                    out_en_raw_file.write(raw.strip() + "\n")
                    sentence = []
                    raw = ""
                else:
                    word = line.split("\t")
                    if word[3] not in [":"] and word[7] != 'punct':
                        raw += " " + word[2]
            out_en_raw_file.close()
        en_file.close()

    sentence = []
    with open(sv_file_name) as sv_file:
        with open(sv_file_name + ".l.txt", "w") as out_sv_file:
            for line in sv_file:
                if line[0] == "\n":
                    out_sv_file.write(" ".join(sentence) + "\n")
                    sentence = []
                else:
                    word = line.split("\t")
                    if word[0] not in ['MID', 'MAD', 'PAD']:
                        sentence.append(word[1].strip())
            out_sv_file.close()
        sv_file.close()


