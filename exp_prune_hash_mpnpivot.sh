#!/bin/bash

#datadir="data/simulated_dags/150_5/"
. exp_conf

result_file="experiments/pivots/prune_hash_mpnpivot.csv"
detail_log_dir="experiments/pivots/prune_hash_mpnpivot_details/"

alg_parameters="--prune --hash --pivot random_mpn"
	
mkdir -p $detail_log_dir

echo "file	count	time(ms)	recursive_calls" > $result_file

usedtimes=()
for g in `ls $datadir`; do
	echo $g

	#fname=`echo $g | grep -oP "[^/]*$"`
	#echo $fname

	./graph_alg ${datadir}${g} ${alg_parameters} > ${detail_log_dir}${g}

	t=`cat ${detail_log_dir}${g} | sed -ne 's/time: \([0-9]*\)ms/\1/p'`
	
	usedtimes+=($t)
done

echo $usedtimes

