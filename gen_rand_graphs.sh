#!/bin/bash

NODE_NUM=100

NUM=1000

DATADIR="data/simulated_dags/100/"

for i in $(seq 1 $NUM);
do
	echo $i
	./graph_gen ${NODE_NUM} > ${DATADIR}g${i}.txt;
done

