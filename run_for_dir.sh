#!/bin/bash

DIR=$1

if [ -z "$1" ];
then
	echo "Usage: $0 <DIR>"
	exit 1
fi

if [ ! -d "$1" ];
then
	echo "$1 not a directory."
	exit 1
fi

#filelist=`find -regex '\./data/rand/g[0-9]*\.txt'`
rm ${DIR}/*_info.txt
filelist=`find ${DIR}`

RED='\033[0;31m'
LIGHTGREEN='\033[1;32m'
NC='\033[0m' # No Color

logfile=templog.tmp

for gfile in $filelist;
do
	echo $gfile
	{ time ./graph_alg $gfile; } &> $logfile
	count=$(sed -ne 's/^Num of consistent sub-DAG: \([0-9]*\)$/\1/p' $logfile )
	count0=$(sed -ne 's/^(Sanity check)Num of consistent sub-DAG: \([0-9]*\)$/\1/p' $logfile )

	realtime=$(sed -ne 's/^real\t\([0-9]*m[0-9.]*s\)$/\1/p' test.log)
	funccalls=$(sed -ne 's/^Function count_consistent_subdag has been called \([0-9\]*\) times.$/\1/p' $logfile)

	#echo $count $count0

	if (( count != count0 ));
	then
		echo -e "${RED}[Failed]${NC} ${gfile}"
	else
		echo -e "${LIGHTGREEN}[  OK  ]${NC} ${gfile}"
	fi

	./analyze_graph $gfile > ${gfile/.txt/_info.txt}
	echo >> ${gfile/.txt/_info.txt}


	echo "Time used:" $realtime >> ${gfile/.txt/_info.txt}
	echo "Number of recursive calls:" $funccalls >> ${gfile/.txt/_info.txt}
	echo "Total count:" $count >> ${gfile/.txt/_info.txt}

done


