# -*- coding: utf-8 -*-

__author__ = 'mehdi'

import sys


if __name__ == "__main__":
    if len(sys.argv) < 5:
        sys.exit('Please set raw src file, parallel file, and both reverse and forward aligns, :\n %s <src_raw_file> <parallel_file> <forward.align> <reverse.align>' % sys.argv[0])

    src_file_name = sys.argv[1]
    parallel_file_name = sys.argv[2]
    forward_file_name = sys.argv[3]
    reverse_file_name = sys.argv[4]

    with open(src_file_name) as en_file:
        with open(parallel_file_name) as parallel_file:
            with open(forward_file_name) as forward_file:
                with open(reverse_file_name) as reverse_file:
                    with open(parallel_file_name + "embed_align", "w") as out_file:
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
                                    sent_embed[i] = [swedish[j]]

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


