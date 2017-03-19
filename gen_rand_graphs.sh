#!/bin/bash

NODE_NUM=25

NUM=5000

for i in $(seq 1 $END);
do
	./graph_gen.sh ${NODE_NUM} > data/rand/g${i}.txt;
done

