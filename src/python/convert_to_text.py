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
write them in fast-align format <line_en> ||| <line_sv>



"""
__author__ = 'mehdi'

import sys

# CONLL FORMAT
ID, FORM, LEMMA, PLEMMA, POS, PPOS, FEAT, PFEAT, HEAD, PHEAD, DEPREL, PDEPREL = range(12)

if __name__ == "__main__":

    if len(sys.argv) < 3:
        sys.exit('Please set english conll file and swedish lemmatized file: %s <en_conll_file> <sv_lemmatized_file>' % sys.argv[0])

    en_file_name = sys.argv[1]
    sv_file_name = sys.argv[2]

    # open the english conll file, extract raw sentences,
    with open(en_file_name) as en_file:
        with open(en_file_name + ".txt", "w") as out_en_file:
            with open(en_file_name + "_raw.txt", "w") as out_en_raw_file:
                sentence = []
                raw = ""
                for line in en_file:
                    if line[0] == "\n":
                        out_en_file.write(" ".join(sentence) + "\n")
                        out_en_raw_file.write(raw.strip() + "\n")
                        sentence = []
                        raw = ""
                    else:
                        word = line.split("\t")
                        if word[PLEMMA] not in [":"] and word[PFEAT] != 'punct':
                            sentence.append(word[2] + ".." + word[3])
                            raw += " " + word[1]
                out_en_raw_file.close()
            out_en_file.close()
        en_file.close()

    with open(en_file_name + ".txt") as en_file:
        with open(sv_file_name) as sv_file:
            with open(sv_file_name + ".txt", "w") as out_sv_file:
                with open(en_file_name + sv_file_name + "_parallel.txt", "w") as out_file:
                    for line in sv_file:
                        if line[0] == "\n":
                            out_sv_file.write(" ".join(sentence) + "\n")
                            english = en_file.readline().strip()
                            swedish = " ".join(sentence)
                            if swedish and english:
                                out_file.write("%s ||| %s\n" % (english, swedish))
                            sentence = []
                        else:
                            word = line.split("\t")
                            if word[0] not in ['MID', 'MAD', 'PAD']:
                                sentence.append(word[1].strip())
                    out_file.close()
                out_sv_file.close()
            sv_file.close()
        en_file.close()


