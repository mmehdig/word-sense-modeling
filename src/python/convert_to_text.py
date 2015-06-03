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

import sys, os


# CONLL word format:
ID, FORM, LEMMA, PLEMMA, POS, PPOS, FEAT, PFEAT, HEAD, PHEAD, DEPREL, PDEPREL = range(12)

# Swedish lemmatized format:
SV_POS, SV_TOKEN = range(2)

if __name__ == "__main__":

    if len(sys.argv) < 3:
        sys.exit('Please set english conll file and swedish lemmatized file: %s <en_conll_file> <sv_lemmatized_file>' % sys.argv[0])

    en_file_path = os.path.normpath(sys.argv[1])
    sv_file_path = os.path.normpath(sys.argv[2])

    en_file_path, en_file_name = os.path.split(en_file_path)
    sv_file_path, sv_file_name = os.path.split(sv_file_path)

    # read form source file:
    # open the English CONLL file, extract raw sentences,
    en_file = open(os.path.join(en_file_path, en_file_name))

    # sentence-by-sentence output corpus, ready to be aligned with parallel corpora:
    out_en_file = open(os.path.join(en_file_path, en_file_name + ".out.txt"), "w")

    # raw word forms corpus. (human readable)
    out_en_raw_file = open(os.path.join(en_file_path, en_file_name + ".out.raw.txt"), "w")
    sentence = []
    raw = ""
    lemma = ""

    # read from source file:
    for line in en_file:
        # each line contain a CONLL formatted word.
        # a new line means the previous sentence has ended.
        if line[0] == "\n":
            # join all words in the sentence
            out_en_file.write(" ".join(sentence) + "\n")
            out_en_raw_file.write(raw.strip() + "\n")
            sentence = []
            raw = ""
            lemma = ""
        else:
            word = line.split("\t")
            if word[LEMMA] not in [".", ",", "!", "?", ":", ";",
                                   "'", "''", '"', '""', "(", ")",
                                   "{", "}", "/", "`", "``"] and \
                word[PLEMMA] not in [":"] and \
                word[PFEAT] != 'punct':
                sentence.append(word[LEMMA] + ".." + word[PLEMMA])
                raw += " " + word[FORM]
                lemma += " " + word[LEMMA]

    # save unsaved files, and close opened file.
    out_en_raw_file.close()
    out_en_file.close()
    en_file.close()

    # open the sentence-by-sentence source file
    en_file = open(os.path.join(en_file_path, en_file_name + ".out.txt"))

    # Swedish source file
    sv_file = open(os.path.join(sv_file_path, sv_file_name))

    # The sentence-by-sentence Swedish output file:
    out_sv_file = open(os.path.join(sv_file_path, sv_file_name + ".out.txt"), "w")

    # The paralleled sentences (ready for fast-align):
    out_file = open(os.path.join(en_file_path, en_file_name + "__" + sv_file_name + ".out.parallel.txt"), "w")

    # re-initialize variables:
    sentence = []

    # read from the Swedish file:
    for line in sv_file:
        # a new line means the previous sentence has ended.
        if line[0] == "\n":
            # glue all the swedish words in one sentence and write them on the file.
            swedish = " ".join(sentence)
            out_sv_file.write(swedish + "\n")

            # read the english sentence :
            english = en_file.readline().strip()

            # if both sentences are available then write them in parallel file:
            if len(swedish) > 1 and len(english) > 1:
                out_file.write("%s ||| %s\n" % (english, swedish))

            sentence = []
        else:
            # split the line with <tab>-separator (Swedish formatted file)
            word = line.split("\t")
            if word[SV_POS] not in ['MID', 'MAD', 'PAD']:
                sentence.append(word[SV_TOKEN].strip())

    out_file.close()
    out_sv_file.close()
    sv_file.close()
    en_file.close()


