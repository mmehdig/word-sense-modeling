#!/bin/bash

SRCDIR=`dirname $0`
FAST_ALIGN_DIR="$SRCDIR/../../../fast_align"

INPUT1="$1"
INPUT2="$2"
#$INPUT1="europarl-v7.sv-en.uniq.mixedpipeline_en.conll"
#$INPUT2="europarl-v7_sv.lemmatized.txt"

# First, process parallel sentences:
python $SRCDIR/convert_to_text\.py $INPUT1 $INPUT2

# Second, align words
# executable fast_align (https://github.com/clab/fast_align)
$FAST_ALIGN_DIR/fast_align \-i $INPUT1\_\_$INPUT2\.out\.parallel\.txt \-d \-o \-v > $INPUT1\_\_$INPUT2\.out\.parallel\.forward\.align
$FAST_ALIGN_DIR/fast_align \-i $INPUT1\_\_$INPUT2\.out\.parallel\.txt \-d \-o \-v \-r > $INPUT1\_\_$INPUT2\.out\.parallel\.reverse\.align

# Finally, create the final product:
python $SRCDIR/combine_alignments\.py $INPUT1\.out\.raw\.txt $INPUT1\_\_$INPUT2\.out\.parallel\.txt $INPUT1\_\_$INPUT2\.out\.parallel\.forward\.align $INPUT1\_\_$INPUT2\.out\.parallel\.reverse\.align
