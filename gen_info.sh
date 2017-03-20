#!/bin/bash

flist=`find -regex '\./data/rand/g[0-9]*\.txt'`

echo $flist

for gfile in $flist;
do
	echo $gfile
	./analyze_graph $gfile > ${gfile/.txt/_info.txt}
done

