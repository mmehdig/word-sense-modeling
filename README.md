# word-sense-modeling
These are some codes related to my master thesis. Supplementary documents and codes will be added in future.

### Annorate words with paralle corpus (python code)
More info will come here

### Convert to word2vec (C code)
This tool converts text based multiple word embedings to binary word2vec file formats. The output file can be used in [gensim](https://radimrehurek.com/gensim/) or other [word2vec](https://code.google.com/p/word2vec/) toolkits. This program has been tested on multiple sense vectors produced by [this code](https://bitbucket.org/jeevan_shankar/multi-sense-skipgram/overview) and [this paper by Neelakantan et al. 2014](https://people.cs.umass.edu/~arvind/emnlp2014.pdf)

The text file can be in two different formats:

##### Default format:
```
<Total Number of words> <Dimensionality> <Number of senses per word> <Does it have the max number of senses per word? (1 or 0)>
<word> <no. of senses>
<global context vector>
<first sense vector>
<second sense vector>
...
```

For example, the text file can be:
```
2 3 2 1
bank 3
0.2 0.1 0.3
0.1 0.2 0.2
0.3 0.3 0.3
0.1 0.1 0.1
lemon 1
0.4 0.5 0.1
0.1 0.6 0.2
```

##### Second format:
```
<Total Number of words> <Dimensionality>
<word> <no. of senses>
<global context vector>
<first sense vector>
<first sense cluster center>
<second sense vector>
<second sense cluster center>
...
```

For example, the text file can be:
```
2 3 2 1
bank 3
0.2 0.1 0.3
0.1 0.2 0.2
0.11 0.2 0.21
0.3 0.3 0.3
0.301 0.301 0.299
0.1 0.1 0.1
0.09 0.09 0.098
lemon 1
0.4 0.5 0.1
0.1 0.6 0.2
0.101 0.602 0.201
```

The output file will have global vector as exact word name. Each word-sense will become "word..<n>" which ```<n>``` is an index number. For example if there was 3 sense vectors for the word "bank" in the model, there will be "bank..1", "bank..2" and "bank..3" as 3 sense vectors, and "bank" as the context vector in the output file. If the file was in second format, with supporing cluster centroids, then in output file they will become "bank..1c", "bank..2c" and "bank..3c".
