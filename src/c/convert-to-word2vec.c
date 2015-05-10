//
//  convert-to-word2vec.c
//  
//
//  Created by Mehdi on 2/19/15.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <ctype.h>

typedef float real;
const long long max_w = 50;              // max length of vocabulary entries
const long long max_dimension = 500;              // max length of vocabulary entries

int main(int argc, char **argv)
{
    FILE *src;
    FILE *trg;
    char file_name[2000];
    long long words, dimensions, a, b, c, vectors, vectors_prog;
    unsigned short int max_s, emb_s, s, if_maxout, default_maxout, n;
    float *M;
    char *vocab;
    char word[max_w];
    char buffer[3];
    float vector[max_dimension];
    
    if (argc < 2) {
        printf("Default usage:\n   ./convert-to-word2vec FILE\n");
        printf("Second file format usage:\n   ./convert-to-word2vec FILE 1\n");
        printf("FILE:\tThe address of the multiple-word-vector file in text format.\n");
        return 0;
    }
    strcpy(file_name, argv[1]);
    default_maxout = 1;
    if (argc > 2) default_maxout = atoi(argv[2]);
    src = fopen(file_name, "rb");
    if (src == NULL) {
        printf("Input file not found\n");
        return -1;
    }

    // maxout!! clusterCenter(v)(s) / (1.0 * clusterCount(v)(s)):
    // this vector and next vector are the same
    if (default_maxout) {
        fscanf(src, "%lld", &words);
        fscanf(src, "%lld", &dimensions);
        
        if_maxout = 0;

        printf("Scan file for senses\n");
        vectors = 0;
        for (b = 0; b < words; b++) {
            a = 0;
            while (1) {
                word[a] = fgetc(src);
                if (feof(src) || (word[a] == ' ')) break;
                if ((a < max_w) && (word[a] != '\n')) a++;
            }
            word[a] = 0;
            if (a == 0) continue;
            
            printf("Number of words in progress: %lld / %lld\r", b, words);
            fflush(stdout);
            
            // number of embodied senses
            fscanf(src, "%hu", &emb_s);
            
            // maximum number of senses per word:
            if (max_s < emb_s) max_s = emb_s;
            
            // sense + cluster center
            emb_s *= 2;
            
            // number of sense vectors + global sense
            for (s = 0; s < emb_s + 1; s++) {
                for (c = 0; c < dimensions; c++) fscanf(src, "%f", &vector[c]);

                //dummy newline read:
                fgetc(src);
                
                // count vectors
                vectors++;
            }
        }
        fclose(src);
        src = fopen(file_name, "rb");
        fscanf(src, "%lld", &words);
        fscanf(src, "%lld", &dimensions);
    } else {
        fscanf(src, "%lld", &words);
        fscanf(src, "%lld", &dimensions);
        fscanf(src, "%hu", &max_s);
        fscanf(src, "%hu", &if_maxout);

        if (if_maxout == 0)
            max_s = max_s * 2 + 1;
        else
            max_s = max_s + 1;
        
        vectors = words * max_s;
    }

    printf("\nNumber words: %lld\n", words);
    printf("Maximum number of senses per word: %hu\n", max_s);
    printf("Number of dimensions: %lld\n", dimensions);
    printf("Maximum number of vectors: %lld\n", vectors);

    vocab = (char *)malloc(vectors * max_w * sizeof(char));
    M = (float *)malloc(vectors * dimensions * sizeof(float));

    if (M == NULL) {
        printf("Cannot allocate memory: %lld MB\n", words * dimensions * sizeof(float) / 1048576);
        return -1;
    }
    
    vectors_prog = 0;
    for (b = 0; b < words; b++) {
        a = 0;
        while (1) {
            word[a] = fgetc(src);
            if (feof(src) || (word[a] == ' ')) break;
            if ((a < max_w) && (word[a] != '\n')) a++;
        }
        word[a] = 0;

        if (a == 0) continue;

        printf("Number of words in progress: %lld / %lld %s\r", b, words, word);
        fflush(stdout);

        fscanf(src, "%hu", &emb_s);
        if (if_maxout == 0) {
            emb_s *= 2;
        }

        // number of sense vectors + global sense
        for (s = 0; s < emb_s + 1; s++) {
            // add the word-sense to vocapulary
            strcpy(&vocab[vectors_prog * max_w], word);
            
            for (c = 0; c < dimensions; c++) fscanf(src, "%f", &M[c + vectors_prog * dimensions]);
            
            if (s > 0) {
                strcpy(&vocab[vectors_prog * max_w + a], "..");
                if (if_maxout == 0) {
                    // if it's not maxout format then it has cluster centers too. cluster centers should be annotated by a "c"
                    n = sprintf(buffer, "%d", (s+1)/2);
                    strcpy(&vocab[vectors_prog * max_w + a + 2], buffer);
                    if (s %2 == 0) {
                        vocab[vectors_prog * max_w + a + 2 + n] = 'c';
                        vocab[vectors_prog * max_w + a + 3 + n] = 0;
                    }
                } else {
                    n = sprintf(buffer, "%hu", s);
                    strcpy(&vocab[vectors_prog * max_w + a + 2], buffer);
                }
            }
            
            //dummy newline read:
            fgetc(src);
            
            // next vector!
            vectors_prog++;
        }
    }
    
    fclose(src);
    strcat(file_name, ".flat.bin");
    printf("\nStoring on \"%s\"\n", file_name);
    trg = fopen(file_name, "wb");
    fprintf(trg, "%lld %lld\n", vectors, dimensions);

    for (b = 0; b < vectors_prog; b++) {
        if (vocab[b * max_w] == 0) break;
        fprintf(trg, "%s ", &vocab[b * max_w]);
            
        for (c = 0; c < dimensions; c++)
            fwrite(&M[b * dimensions + c], sizeof(real), 1, trg);
    }
    fclose(trg);
    return 0;
}
