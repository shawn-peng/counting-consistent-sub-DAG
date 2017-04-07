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

nstart=2
nend=100

if [ ! -z "$2" ];
then
	nstart=$2
fi
if [ ! -z "$3" ];
then
	nend=$3
fi
if [ ! -z "$4" ];
then
	dstart=$4
fi

#${printtitle:=true}
${printtitle:=false}

if [ ! -z "$dstart" ]; then
	categories=$(ls $(seq -f "${DIR}/${nstart}_%.0f" $dstart $nstart) -d)
else
	categories=$(ls $(seq -f "${DIR}/%.0f_*" $nstart $nend) -d)
fi


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
exit
fi

export printtitle=false

for categ_dir in $categories;
do
	#echo -n ${categ}_

	#echo $categ_dir
	categ=$(echo $categ_dir | sed -ne "s@${DIR}/\?\([0-9]*_[0-9]*\)@\1@p")
	#categ=`echo $categ_dir | sed -e "s@${DIR}@@"`

	./run_for_dir2.sh ${categ_dir} |\
		sed -s "s/^/${categ}_/"
done


