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


${printtitle:=true}
#printtitle=false

#filelist=`find -regex '\./data/rand/g[0-9]*\.txt'`
#rm ${DIR}/*_info.txt
filelist=`find ${DIR} -type f -name "*.txt" | sed -e '/_info.txt/d'`
#filelist=$(ls ${categories})
#filelist=$(echo ${filelist} | sed -e '/_info.txt/d')


RED='\033[0;31m'
LIGHTGREEN='\033[1;32m'
NC='\033[0m' # No Color

logfile=templog.tmp

if $printtitle; then
#print title
echo -n "\"# ID\" "
echo -n "\"# of vertices\" "
echo -n "\"# of edges\" "
echo -n "\"# of multi-parent vertices\" "
echo -n "\"level\" "
echo -n "\"depth\" "
echo -n "\"# of vertices having in-degree = 1\" "
echo -n "\"# of vertices having in-degree = 2\" "
echo -n "\"# of vertices having in-degree = 3\" "
echo -n "\"# of vertices having in-degree = 4\" "
echo -n "\"# of vertices having in-degree = 5\" "
echo -n "\"# of vertices having in-degree = 6\" "
echo -n "\"# of vertices having in-degree = 7\" "
echo -n "\"# of vertices having in-degree = 8\" "
echo -n "\"# of vertices having in-degree = 9\" "
echo -n "\"# of vertices having in-degree >= 10\" "
echo -n "\"# of vertices having out-degree = 0\" "
echo -n "\"# of vertices having out-degree = 1\" "
echo -n "\"# of vertices having out-degree = 2\" "
echo -n "\"# of vertices having out-degree = 3\" "
echo -n "\"# of vertices having out-degree = 4\" "
echo -n "\"# of vertices having out-degree = 5\" "
echo -n "\"# of vertices having out-degree = 6\" "
echo -n "\"# of vertices having out-degree = 7\" "
echo -n "\"# of vertices having out-degree = 8\" "
echo -n "\"# of vertices having out-degree = 9\" "
echo -n "\"# of vertices having out-degree >= 10\" "
echo -n "\"# of recursive calls\" "
echo -n "\"# of consistent subgraphs\" "
echo
fi

for gfile in $filelist;
do
	#echo $gfile
	{ time ./graph_alg $gfile; } &> $logfile
	count=$(sed -ne 's/^Num of consistent sub-DAG: \([0-9]*\)$/\1/p' $logfile )
	count0=$(sed -ne 's/^(Sanity check)Num of consistent sub-DAG: \([0-9]*\)$/\1/p' $logfile )

	realtime=$(sed -ne 's/^real\t\([0-9]*m[0-9.]*s\)$/\1/p' $logfile )
	funccalls=$(sed -ne 's/^Function count_consistent_subdag has been called \([0-9\]*\) times.$/\1/p' $logfile)

	info=$(./analyze_graph -s $gfile)

	categ=`echo $gfile | sed -ne "s#^${DIR}/\?\(.*\)/\([0-9]*\).txt#\1#p"`
	if [[ ! -z "$categ" ]]; then
		gi=`echo $gfile | sed -ne "s#^${DIR}/\?\(.*\)/\([0-9]*\).txt#\2#p"`
		echo ${categ}_${gi}	${info}	${funccalls}	${count}
	else
		gi=`echo $gfile | sed -ne "s#^${DIR}/\?\([0-9]*\).txt#\1#p"`
		echo ${gi}	${info}	${funccalls}	${count}
	fi

done


