CC = /usr/bin/gcc
#Using -Ofast instead of -O3 might result in faster code, but is supported only by newer GCC versions
CFLAGS = -lm -pthread -O3 -march=native -Wall -funroll-loops -Wno-unused-result

all: convert-to-word2vec scws_correlation

convert-to-word2vec : convert-to-word2vec.c
	$(CC) convert-to-word2vec.c -o convert-to-word2vec $(CFLAGS)
scws_correlation : scws_correlation.c
	$(CC) scws_correlation.c -o scws_correlation $(CFLAGS)
clean:
	rm -rf convert-to-word2vec
