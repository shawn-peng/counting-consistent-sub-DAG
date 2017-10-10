#!/bin/bash

NODE_NUM=100

NUM=1000

for i in $(seq 1 $NUM);
do
	echo $i
	./graph_gen ${NODE_NUM} > data/rand/g${i}.txt;
done

