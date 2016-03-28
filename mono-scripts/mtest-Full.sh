#!/bin/bash

cd /users2/jguo/work/parser/clnndep

if [ $# -ne 2 ]; then
    echo "Usage: ./test.sh [lang] [oracle|0/1]"
    echo "Option: oracle=0 (arc-standard), 1 (nivre09)"
    exit -1
fi

lang=$1
oracle=$2
corpus=udt/multi-source/$lang

models=mono-models-swap/

f_test=$corpus/$lang-universal-test.conll
f_output=$f_test.full.rnd.out

if [ "$oracle" = "1" ]; then
    echo "Test Non-Projective(Nivre09)"
    model_dir=$models/$lang.model.rnd.nivre09
    f_conf=conf/multi/full-swap.cfg
else
    echo "Test Projective(ArcStandard)"
    model_dir=$models/$lang.model.rnd.arcstd
    f_conf=conf/multi/full.cfg
fi

#f_model=$model_dir/model.finetune.$sample.$iter
f_model=$model_dir/model

./bin/clnndep -test  $f_test \
              -model $f_model \
              -output $f_output \
              -cfg $f_conf

