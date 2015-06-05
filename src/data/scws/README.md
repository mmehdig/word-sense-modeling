### Stanford Contextual Word Similarity (SCWS)

This dataset is introduced by this paper:
```
@inproceedings{HuangEtAl2012,
author = {Eric H. Huang and Richard Socher and Christopher D. Manning and Andrew Y. Ng},
title = {Improving Word Representations via Global Context and Multiple Word Prototypes},
booktitle = {Annual Meeting of the Association for Computational Linguistics (ACL)},
year = 2012
}
```


The original file is available here: http://ai.stanford.edu/~ehhuang/

In this version we only kept average human judgment. Each line of the file contains:
```
<id>	<word1>	<POS of word1>	<word2>	<POS of word2>	<word1 in context>	<word2 in context>	<average human rating>
```

The correlation of VSM performance and average human judgment can be computed by:
```
./scws_correlation model_file < scws_modified.txt
```
