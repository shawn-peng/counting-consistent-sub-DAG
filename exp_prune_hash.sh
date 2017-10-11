#!/bin/bash

#datadir="data/simulated_dags/150_5/"
. exp_conf

result_file="experiments/methods/prune_hash.csv"
detail_log_dir="experiments/methods/prune_hash_details/"

alg_parameters="--prune --hash --pivot random_mpn"
	
mkdir -p $detail_log_dir

usedtimes=()
echo "file	count	time(ms)	recursive_calls" > $result_file
for g in `ls $datadir`; do
	echo $g

	#fname=`echo $g | grep -oP "[^/]*$"`
	#echo $fname

	./graph_alg ${datadir}${g} ${alg_parameters} > ${detail_log_dir}${g}

	t=`cat ${detail_log_dir}${g} | sed -ne 's/time: \([0-9]*\)ms/\1/p'`
	cnt=`cat ${detail_log_dir}${g} | sed -ne 's/: \([0-9]*\)ms/\1/p'`

	echo "$g		$t" >> $result_file
	echo $t
	
	usedtimes+=($t)
done

echo $usedtimes

