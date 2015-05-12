//
//  cws_correlation.c
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

const long long max_size = 3000;         // max length of strings
const long long max_w = 50;              // max length of string for each word entries
const unsigned short int N = 10;         // number of closest words considered in cluster selection formula
const unsigned short int n_vectors = 4;  // max number of embedded vectors per word
const char *pos_tags[] = {"n", "aj", "av", "v", ""}; // possible pos tags
const unsigned short int number_of_pos = 5; // number of postags in pos_tags

unsigned short int extraction(char *str_list[n_vectors], char strx[max_size], long long locs[n_vectors], long long n_words, char *vocab)
{
    unsigned short int n_i = 0;
    unsigned short int i;
    long long b, a, c;
    char *ch_int;
    ch_int = (char *) malloc(sizeof(char));
    
    
    for (i=0; i < n_vectors; i++) {
        //make sure locations are empty
        locs[i] = -1;
        
        str_list[i] = (char *) malloc(max_size*sizeof(char));
        
        
        // strcpy(str_list[i], strx);
        for(c = 0; c < max_w; c++) {
            str_list[i][c] = strx[c];
            if (str_list[i][c] == '\0') break;
        }

        
        if (i > 0) {
            str_list[i][c++] = '.';
            str_list[i][c++] = '.';
            
            *ch_int = (char)(((int)'0')+i);
            
            while(1) {
                str_list[i][c] = ch_int[c];
                if ((ch_int[c] == '\0') || (c == max_w)) break;
                c++;
            }
        }
        
        // find the word
        for (b = 0; b < n_words; b++) if (!strcmp(&vocab[b * max_w], str_list[i])) break;
        
        // not found?
        if (b == n_words) break;
        
        // the corresponding vector for this particular word
        locs[i] = b;
        
        // update number of embedded vectors
        n_i = i + 1;
    }
    return n_i;
}

int main(int argc, char **argv)
{
    FILE *f;
    char dummy, st1[max_w], st2[max_w], file_name[max_size];
    char context_word[max_w];
    float contex_vec1[1000], contex_vec2[1000], tmp_vec[1000];
    unsigned short int n_tmp_vec = 0;
    unsigned short int word_counter;
    unsigned short int _skip, _break;
    unsigned short int enable_pos = 0;
    unsigned short int p;

    char *st1x[max_size], *st2x[max_size];
    unsigned short int i, j;
    unsigned short int n_i, n_j, min_i = 0;
    long long i_locs[n_vectors], j_locs[n_vectors], best_i_loc, best_j_loc;
    unsigned int test_count = 0, n, m;
    float dist, len, best_dist;
    float gold_score = -1;
    float best_guess, high_sim, worst_guess;
    float X[max_size], best_Y[max_size], worst_Y[max_size], high_sim_Y[max_size], tmp;
    float sorted_X[max_size], *sorted_Y, sorted_best_Y[max_size], sorted_worst_Y[max_size], sorted_high_sim_Y[max_size];
    int ranked_Y[max_size];
    double X_avg = 0, X_sigma = 0, Y_avg = 0, Y_sigma = 0, rho = 0;
    
    
    long long n_words, size, a, b, c, d;
    float *M;
    char *vocab;
    if (argc < 2) {
        printf("Default usage:\n   ./cws_correlation FILE\n");
        printf("Enable Part of Speech Tags:\n   ./cws_correlation FILE 1\n");
        printf("FILE:   the address of the file which contains word-sense vectors\n");
        return 0;
    }
    strcpy(file_name, argv[1]);
    if (argc > 2) enable_pos = atoi(argv[2]);
    f = fopen(file_name, "rb");
    if (f == NULL) {
        printf("Input file not found\n");
        return -1;
    }
    fscanf(f, "%lld", &n_words);
    fscanf(f, "%lld", &size);
    vocab = (char *)malloc(n_words * max_w * sizeof(char));
    M = (float *)malloc(n_words * size * sizeof(float));
    if (M == NULL) {
        printf("Cannot allocate memory: %lld MB\n", n_words * size * sizeof(float) / 1048576);
        return -1;
    }
    // load all vectors from file into M (also normalize them)
    for (b = 0; b < n_words; b++) {
        a = 0;
        while (1) {
            vocab[b * max_w + a] = fgetc(f);
            if (feof(f) || (vocab[b * max_w + a] == ' ')) break;
            if ((a < max_w) && (vocab[b * max_w + a] != '\n')) a++;
        }
        vocab[b * max_w + a] = 0;
        for (a = 0; a < max_w; a++) vocab[b * max_w + a] = toupper(vocab[b * max_w + a]);
        for (a = 0; a < size; a++) fread(&M[a + b * size], sizeof(float), 1, f);
        len = 0;
        for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
        len = sqrt(len);
        for (a = 0; a < size; a++) M[a + b * size] /= len;
    }
    fclose(f);
    printf("W1\tW2\tGold\tGuess\n");
    while (1) {
        // read the first word
        //<id> <word1> <POS of word1> <word2> <POS of word2> <word1 in context> <word2 in context> <average human rating> <10 individual human ratings>

        // <id> DUMMY!!
        
        if (feof(stdin)) break;
        
        c = 0;
        while (1) {
            dummy = fgetc(stdin);
            if (feof(stdin) || (dummy == '\t') || (c == max_w)) break;
            c++;
        }
        
        // <word1>
        c = 0;
        while (1) {
            st1[c] = toupper(fgetc(stdin));
            if (feof(stdin) || (st1[c] == '\t') || (c == max_w)) break;
            c++;
        }
        st1[c] = '\0';
        
        // <POS of word1> 
        if (enable_pos) {
            st1[c++] = '.';
            st1[c++] = '.';
            while (1) {
                st1[c] = toupper(fgetc(stdin));
                if (feof(stdin) || (st1[c] == '\t') || (c == max_w)) break;
                c++;
            }
            st1[c] = '\0';
        } else {
            // DUMMY!!
            c = 0;
            while (1) {
                dummy = fgetc(stdin);
                if (feof(stdin) || (dummy == '\t') || (c == max_w)) break;
                c++;
            }
        }
        
        // <word2>
        c = 0;
        while (1) {
            st2[c] = toupper(fgetc(stdin));
            if (feof(stdin) || (st2[c] == '\t') || (c == max_w)) break;
            c++;
        }
        st2[c] = '\0';
        // <POS of word2>
        if (enable_pos) {
            st2[c++] = '.';
            st2[c++] = '.';
            while (1) {
                st2[c] = toupper(fgetc(stdin));
                if (feof(stdin) || (st2[c] == '\t') || (c == max_w)) break;
                c++;
            }
            st2[c] = '\0';
        } else {
            // DUMMY!!
            c = 0;
            while (1) {
                dummy = fgetc(stdin);
                if (feof(stdin) || (dummy == '\t') || (c == max_w)) break;
                c++;
            }
        }

        printf("\n%s\t%s", st1, st2);

        // <word1 in context>
        // initialize contex vector
        for (a = 0; a < size; a++) contex_vec1[a] = 0;

        // read each word, find the context vector, update the context_vector.
        word_counter = 0;
        _skip = 0;
        _break = 0;
        while (1) {
            if (_break) break;
            
            // read a new word:
            c = 0;
            while (1) {
                context_word[c] = toupper(fgetc(stdin));
                if (feof(stdin) || (context_word[c] == ' ') || (context_word[c] == '\t') || (c == max_w)) break;
                c++;
            }

            if (c == 0) break;
            
            // not found?
            if (context_word[c] == '\t') _break = 1;
            context_word[c] = '\0';

            // skip the tagged word
            if (!strcmp(context_word, "<b>")) {
                _skip = 1;
                continue;
            } else if (_skip || !strcmp(context_word, "</b>")) {
                _skip = 0;
                continue;
            }
            if (enable_pos) {
                // initialize temprorary vector
                for (a = 0; a < size; a++) tmp_vec[a] = 0;

                n_tmp_vec = 0;
                for (p = 0; p < number_of_pos; p++) {
                    context_word[c] = '.';
                    context_word[c+1] = '.';
                    d = 0;
                    while (1) {
                        context_word[c+2+d] = pos_tags[p][d];
                        if ((pos_tags[p][d] == '\0') || (c+2+d == max_w)) break;
                        d++;
                    }
                    context_word[c+2+d] = '\0';

                    // find the word
                    for (b = 0; b < n_words; b++)
                        if (!strcmp(&vocab[b * max_w], context_word)) break;
                    // not found?
                    if (b == n_words) continue;

                    // add the word context vector to overall context_vector
                    for (a = 0; a < size; a++) tmp_vec[a] += M[a + b * size];

                    n_tmp_vec++;
                }

                // there is an average vector for this context word!
                if (n_tmp_vec){
                    // normalize as one word vector:
                    len = 0;
                    for (a = 0; a < size; a++) len += tmp_vec[a] * tmp_vec[a];
                    len = sqrt(len);
                    // add to the context vector:
                    for (a = 0; a < size; a++) contex_vec1[a] += tmp_vec[a] / len;
                } else {
                    continue;
                }
                
            } else {
                // find the word
                for (b = 0; b < n_words; b++)
                    if (!strcmp(&vocab[b * max_w], context_word)) break;
                // not found?
                if (b == n_words) continue;

                // add the word context vector to overall context_vector
                for (a = 0; a < size; a++) contex_vec1[a] += M[a + b * size];
            }
                        

            word_counter++;
        }

        // normalize the context vector:
        len = 0;
        for (a = 0; a < size; a++) len += contex_vec1[a] * contex_vec1[a];
        len = sqrt(len);
        for (a = 0; a < size; a++) contex_vec1[a] /= len;

        
        // <word2 in context>
        // initialize contex vector
        for (a = 0; a < size; a++) contex_vec2[a] = 0;
        
        // read each word, find the context vector, update the context_vector.
        word_counter = 0;
        _skip = 0;
        _break = 0;
        while (1) {
            if (_break) break;

            // read a new word:
            c = 0;
            while (1) {
                context_word[c] = toupper(fgetc(stdin));
                if (feof(stdin) || (context_word[c] == ' ') || (context_word[c] == '\t') || (c == max_w)) break;
                c++;
            }
            
            if (c == 0) break;
            
            // not found?
            if (context_word[c] == '\t') _break = 1;
            context_word[c] = '\0';
            
            // skip the tagged word
            if (!strcmp(context_word, "<b>")) {
                _skip = 1;
                continue;
            } else if (_skip || !strcmp(context_word, "</b>")) {
                _skip = 0;
                continue;
            }
            
            if (enable_pos) {
                // initialize temprorary vector
                for (a = 0; a < size; a++) tmp_vec[a] = 0;

                n_tmp_vec = 0;
                for (p = 0; p < number_of_pos; p++) {
                    context_word[c] = '.';
                    context_word[c+1] = '.';
                    d = 0;
                    while (1) {
                        context_word[c+2+d] = pos_tags[p][d];
                        if ((pos_tags[p][d] == '\0') || (c+2+d == max_w)) break;
                        d++;
                    }
                    context_word[c+2+d] = '\0';

                    // find the word
                    for (b = 0; b < n_words; b++)
                        if (!strcmp(&vocab[b * max_w], context_word)) break;
                    // not found?
                    if (b == n_words) continue;

                    // add the word context vector to overall context_vector
                    for (a = 0; a < size; a++) tmp_vec[a] += M[a + b * size];

                    n_tmp_vec++;
                }

                // there is an average vector for this context word!
                if (n_tmp_vec){
                    // normalize as one word vector:
                    len = 0;
                    for (a = 0; a < size; a++) len += tmp_vec[a] * tmp_vec[a];
                    len = sqrt(len);
                    // add to the context vector:
                    for (a = 0; a < size; a++) contex_vec2[a] += tmp_vec[a] / len;
                } else {
                    continue;
                }
                
            } else {
                // find the word
                for (b = 0; b < n_words; b++)
                    if (!strcmp(&vocab[b * max_w], context_word)) break;
                
                // not found?
                if (b == n_words) continue;

                // add the word context vector to overall context_vector
                for (a = 0; a < size; a++) contex_vec2[a] += M[a + b * size];
            }

            word_counter++;
        }
        
        // normalize the context vector:
        len = 0;
        for (a = 0; a < size; a++) len += contex_vec2[a] * contex_vec2[a];
        len = sqrt(len);
        for (a = 0; a < size; a++) contex_vec2[a] /= len;
        
        // <average human rating>
        scanf("%f", &gold_score);
        
        // uppercase first word
        for (a = 0; a < strlen(st1); a++) st1[a] = toupper(st1[a]);
        for (a = 0; a < strlen(st2); a++) st2[a] = toupper(st2[a]);
        
        // find all possible embedded vectors for each word. (word, word..1, word..2, ...)
        n_i = extraction(st1x, st1, i_locs, n_words, vocab);
        n_j = extraction(st2x, st2, j_locs, n_words, vocab);
        
        // count it as an invalid question if no sense-vector was in the model:
        if ((n_i == 0) || (n_j == 0)) {
            printf("\t%f\t-\n", gold_score);
            continue;
        }        
        
        // select one of the senses for word1 which is closes to the context1
        best_dist = 0;
        for (i=min_i; i < n_i; i++) {
            dist = 0;
            for (a = 0; a < size; a++) dist += contex_vec1[a] * M[a + i_locs[i] * size];
            dist = fabsf(dist);
            
            if (best_dist < dist) {
                best_dist = dist;
                best_i_loc = i_locs[i];
            }
        }
        
        // select one of the senses for word2 which is closes to the context2
        best_dist = 0;
        for (j=min_i; j < n_j; j++) {
            dist = 0;
            for (a = 0; a < size; a++) dist += contex_vec2[a] * M[a + j_locs[j] * size];
            dist = fabsf(dist);
            
            if (best_dist < dist) {
                best_dist = dist;
                best_j_loc = j_locs[j];
            }
        }
        
        dist = 0;
        for (a = 0; a < size; a++) dist += M[a + best_j_loc * size] * M[a + best_i_loc * size];
        dist = fabsf(10 * dist);

        // print the report:
        printf("\t%f\t%f\n", gold_score, dist);
        printf("%s\t%s\n", &vocab[best_i_loc * max_w], &vocab[best_j_loc * max_w]);
        X[test_count] = gold_score;
        best_Y[test_count] = dist;
        
        test_count++;
    }
    
    
    // rho = COV(X, Y) /(Sigma(X)*Sigma(Y))
    for (n=0; n < test_count; n++) X_avg += X[n];
    X_avg /= test_count;
    
    for (n=0; n < test_count; n++) X_sigma += (X_avg - X[n]) * (X_avg - X[n]);
    X_sigma = sqrt(X_sigma);
    
    for (n=0; n < test_count; n++) Y_avg += best_Y[n];
    Y_avg /= test_count;
    
    for (n=0; n < test_count; n++) Y_sigma += (Y_avg - best_Y[n]) * (Y_avg - best_Y[n]);
    Y_sigma = sqrt(Y_sigma);
    
    for (n=0; n < test_count; n++) rho += (X_avg - X[n]) * (Y_avg - best_Y[n]);
    
    rho = rho / X_sigma / Y_sigma;
    
    printf("____________________________________\n");
    printf("Number of seen pairs: %u\n", test_count);
    printf("Correlation (best): %f\n", rho);
    
    // Spearman:
    // 1. sort X (reorder Y in parallel)
    for (n=0; n < test_count; n++) {
        tmp = X[n];
        if (n==0) {
            sorted_X[n] = tmp;
            sorted_best_Y[n] = best_Y[n];
            sorted_worst_Y[n] = worst_Y[n];
            sorted_high_sim_Y[n] = high_sim_Y[n];
        } else {
            for (m=n; m > 0; m--) {
                if (tmp > sorted_X[m-1]) {
                    sorted_X[m] = sorted_X[m-1];
                    sorted_X[m-1] = tmp;
                    
                    sorted_best_Y[m] = sorted_best_Y[m-1];
                    sorted_best_Y[m-1] = best_Y[n];
                    
                } else {
                    break;
                }
            }
        }
    }
    
    // 2. sort Y and keep their X ranks in ranked_Y
    sorted_Y = sorted_best_Y;
    for (n=0; n < test_count; n++) {
        tmp = sorted_Y[n];
        if (n==0) {
            sorted_Y[n] = tmp;
            ranked_Y[n] = n;
        } else {
            for (m=n; m > 0; m--) {
                if (tmp > sorted_Y[m-1]) {
                    sorted_Y[m] = sorted_Y[m-1];
                    sorted_Y[m-1] = tmp;
                    
                    ranked_Y[m] = ranked_Y[m-1];
                    ranked_Y[m-1] = n;
                } else {
                    break;
                }
            }
        }
    }
    
    // 3. sum of square of distance of ranks:
    dist = 0;
    for (n=0; n < test_count; n++) {
        dist += (ranked_Y[n] - n) * (ranked_Y[n] - n);
    }
    rho = 1 - 6 * dist / test_count / (test_count * test_count - 1);
    
    printf("Searman Correlation (best): %f\n", rho);
    
    return 0;
}
