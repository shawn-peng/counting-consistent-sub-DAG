#!/bin/bash


RED='\033[0;31m'
LIGHTGREEN='\033[1;32m'
NC='\033[0m' # No Color




for i in $(seq 1 5000); do
	#echo $gfile
	gfile="data/rand/g${i}.txt"
	rgfile="data/rand_reverse/g${i}.txt"

	./graph_alg $gfile > test.log
	./graph_alg $rgfile > rtest.log
	count0=$(sed -ne 's/^Num of consistent sub-DAG: \([0-9]*\)$/\1/p' test.log)
	count=$(sed -ne 's/^Num of consistent sub-DAG: \([0-9]*\)$/\1/p' rtest.log)

	#realtime=$(sed -ne 's/^\([0-9]*\)$/\1/p' test.log)
	funccalls=$(sed -ne 's/^Function count_consistent_subdag has been called \([0-9\]*\) times.$/\1/p' test.log)

	#echo $count $count0

	if (( count != count0 ));
	then
		echo -e "${RED}[Failed]${NC} ${gfile}"
	else
		echo -e "${LIGHTGREEN}[  OK  ]${NC} ${gfile}"
	fi
done


