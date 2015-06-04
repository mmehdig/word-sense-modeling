# -*- coding: utf-8 -*-

__author__ = 'mehdi'

import sys, os


if __name__ == "__main__":
    if len(sys.argv) < 5:
        sys.exit('Please set raw src file, parallel file, and both reverse and forward aligns, :\n %s <src_raw_file> <parallel_file> <forward.align> <reverse.align>' % sys.argv[0])

    src_file_path = os.path.normpath(sys.argv[1])
    parallel_file_path = os.path.normpath(sys.argv[2])
    forward_file_path = os.path.normpath(sys.argv[3])
    reverse_file_path = os.path.normpath(sys.argv[4])

    src_file_path, src_file_name = os.path.split(src_file_path)
    parallel_file_path, parallel_file_name = os.path.split(parallel_file_path)
    forward_file_path, forward_file_name = os.path.split(forward_file_path)
    reverse_file_path, reverse_file_name = os.path.split(reverse_file_path)

    # open files:
    en_file = open(os.path.join(src_file_path, src_file_name))
    parallel_file = open(os.path.join(parallel_file_path, parallel_file_name))
    forward_file = open(os.path.join(forward_file_path, forward_file_name))
    reverse_file = open(os.path.join(reverse_file_path, forward_file_name))

    # output file:
    out_file = open(os.path.join(src_file_path, src_file_name + ".out.augmented.txt"), "w")

    # for each line in parallel input-file of fast-align,
    # split Swedish and English sentences.
    # Then, in parallel read mapping outputs of fast-align,
    # and for each word alignments in both reverse and forward alignment,
    # create the augmented from the source file.
    for line in parallel_file:
        english = en_file.readline().strip().split(" ")
        swedish = line.strip().split(" ||| ")[1].split(" ")
        map_forward = forward_file.readline().strip().split(" ")
        map_reverse = reverse_file.readline().strip().split(" ")

        # union of two different mappings
        # if any of the mappings had an empty string, just remove it from final union
        mappings = set(map_forward + map_reverse) - {''}
        sent_embed = dict()
        for mapping in mappings:
            i, j = tuple(int(x) for x in mapping.split("-"))
            if i in sent_embed:
                sent_embed[i].append(swedish[j])
            else:
                sent_embed[i] = set(swedish[j].split("|"))

        sentence = []
        for i, word in enumerate(english):
            emb = ""
            if i in sent_embed:
                emb = "|".join(sent_embed[i])

            if emb:
                sentence.append("%s__%s" % (word, emb))
            else:
                sentence.append(word)

        if sentence:
            out_file.write(" ".join(sentence) + "\n")

    out_file.close()
    reverse_file.close()
    forward_file.close()
    parallel_file.close()
    en_file.close()


