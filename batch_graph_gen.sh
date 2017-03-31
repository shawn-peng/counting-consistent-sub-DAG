#!/bin/bash

data_dir="data/simulated_dags2/"

(( nvstart = 2 ))
(( depthstart = 2 ))

if [[ ! -z "$1" ]]; then
	(( nvstart = $1 ))
fi

if [[ ! -z "$2" ]]; then
	(( depthstart = $2 ))
fi

for nv in $(seq $nvstart 100); do
	for depth in $(seq $depthstart $nv); do
		(( cnt = 0 ))
		echo "nodes: $nv , depth $depth"
		echo "please input lambdas(split by spaces):"
		read params

		echo "please input number of dags to be generated:"
		read ndags

		dir="${data_dir}${nv}_${depth}/"
		mkdir $dir -p
		while (( cnt < ndags )); do
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
done


