#!/bin/bash

#datadir="data/simulated_dags/150_5/"
. exp_conf

result_file="experiments/methods/baseline.csv"
detail_log_dir="experiments/methods/baseline_details/"
	
mkdir -p $detail_log_dir

alg_parameters="--pivot random_mpn"

usedtimes=()
for g in `ls $datadir`; do
	echo $g
	echo "file	count	time(ms)	recursive_calls" > $result_file

	#fname=`echo $g | grep -oP "[^/]*$"`
	#echo $fname

	./graph_alg ${datadir}${g} ${alg_parameters} > ${detail_log_dir}${g}

	t=`cat ${detail_log_dir}${g} | sed -ne 's/time: \([0-9]*\)ms/\1/p'`
	
	usedtimes+=($t)
done

echo $usedtimes

