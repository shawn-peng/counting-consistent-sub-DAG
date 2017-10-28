#!/bin/bash

if [ -z "$1" -o -z "$2" ]; then
	echo "Usage: $0 <exp_list_file> <nodenum>"
	exit 1
fi

nodenum=$2

#waiting_time=$((60 * 60 * 5))
waiting_time=$((60))

for exp in `cat $1`; do
	echo $exp

	timeout 5h ./run_exp.sh $exp $nodenum

done

