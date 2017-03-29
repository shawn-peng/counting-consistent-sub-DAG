#!/bin/bash


if [ -z "$1" ];
then
	echo "Usage: $0 <FILE>"
	exit 1
fi

if [ ! -f "$1" ];
then
	echo "$1 not a file."
	exit 1
fi

gfile="$1"


RED='\033[0;31m'
LIGHTGREEN='\033[1;32m'
NC='\033[0m' # No Color

logfile=templog.tmp

#echo $gfile
{ time ./graph_alg $gfile; } &> $logfile &

tail -f $logfile | grep "\[[12]\]"
wait

count=$(sed -ne 's/^Num of consistent sub-DAG: \([0-9]*\)$/\1/p' $logfile )
count0=$(sed -ne 's/^(Sanity check)Num of consistent sub-DAG: \([0-9]*\)$/\1/p' $logfile )

realtime=$(sed -ne 's/^real\t\([0-9]*m[0-9.]*s\)$/\1/p' $logfile )
funccalls=$(sed -ne 's/^Function count_consistent_subdag has been called \([0-9\]*\) times.$/\1/p' $logfile)

#echo $count $count0

#if (( count != count0 ));
#then
#	echo -e "${RED}[Failed]${NC} ${gfile}"
#else
#	echo -e "${LIGHTGREEN}[  OK  ]${NC} ${gfile}"
#fi

./analyze_graph $gfile > ${gfile/.txt/_info.txt}
echo >> ${gfile/.txt/_info.txt}


echo "Time used:" $realtime >> ${gfile/.txt/_info.txt}
echo "Number of recursive calls:" $funccalls >> ${gfile/.txt/_info.txt}
echo "Total count:" $count >> ${gfile/.txt/_info.txt}

cat ${gfile/.txt/_info.txt} | sed -e "/\(parent\|children\)/d"


