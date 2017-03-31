#!/bin/bash

data_dir="data/simulated_dags2/"

for nv in $(seq 2 100); do
	for depth in $(seq 2 $nv); do
		(( cnt = 0 ))
		echo "nodes: $nv , depth $depth"
		echo "please input lambdas(split by spaces):"
		read params

		echo "please input number of dags to be generated:"
		read ndags

		dir="${data_dir}${nv}_${depth}/"
		mkdir $dir -p
		for lambda in $params; do
			(( cnt ++ ))
			file="${dir}${cnt}.txt"
			echo $file
			./graph_gen $nv $depth $lambda > $file
			if (( cnt >= ndags )); then
				break;
			fi
		done
	done
done


