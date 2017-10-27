#!/bin/bash

filelist=`find data/rand -regex 'data/rand/g[0-9]*\.txt'`
#filelist=`find data/rand_reverse -regex 'data/rand/g[0-9]*\.txt'`

RED='\033[0;31m'
LIGHTGREEN='\033[1;32m'
NC='\033[0m' # No Color

logfile=test.log
lckf=${logfile}.lock
lockfile -r 0 $lckf
if [ $? -ne 0 ]; then
	echo "Another $0 is running...."
	echo "otherwise please remove $lckf and try again"
	exit 1
fi

if [ -z "$1" ]; then
	echo "Usage: $0 <args_for_alg> ..."
	exit 1
fi

alg_params="--sanity-check $*"

trap "rm -f $lckf && echo 'Quit testing' && exit 0" SIGINT

for gfile in $filelist;
do
	#echo $gfile
	./graph_alg $gfile ${alg_params} > ${logfile}
	if [[ $? != 0 ]]; then
		break;
	fi
	count=$(sed -ne 's/^Num of consistent sub-DAG: \([0-9]*\)$/\1/p' ${logfile})
	count0=$(sed -ne 's/^(Sanity check)Num of consistent sub-DAG: \([0-9]*\)$/\1/p' ${logfile})

	#realtime=$(sed -ne 's/^\([0-9]*\)$/\1/p' ${logfile})
	funccalls=$(sed -ne 's/^Function count_consistent_subdag has been called \([0-9\]*\) times.$/\1/p' ${logfile})

	#echo $count $count0

	if (( count != count0 ));
	then
		echo -e "${RED}[Failed]${NC} ${gfile}"
	else
		echo -e "${LIGHTGREEN}[  OK  ]${NC} ${gfile}"
	fi

	infofile=${gfile/.txt/_info.txt}
	./analyze_graph $gfile > ${infofile}
	echo >> ${infofile}


	echo "Number of recursive calls:" $funccalls >> ${infofile}
	echo "Total count:" $count >> ${infofile}

done

rm -f ${logfile}.lock

