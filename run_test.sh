#!/bin/bash

filelist=`find -regex '\./data/rand/g[0-9]\.txt'`

RED='\033[0;31m'
LIGHTGREEN='\033[1;32m'
NC='\033[0m' # No Color

for gfile in $filelist;
do
	#echo $gfile
	./graph_alg $gfile > test.log
	count=$(sed -ne 's/^Num of consistent sub-DAG: \([0-9]*\)$/\1/p' test.log)
	count0=$(sed -ne 's/^(Sanity check)Num of consistent sub-DAG: \([0-9]*\)$/\1/p' test.log)

	echo $count $count0

	if (( count != count0 ));
	then
		echo -e "${RED}[ Failed ]${NC} ${gfile}"
	else
		echo -e "${LIGHTGREEN}[  OK  ]${NC} ${gfile}"
	fi
done


