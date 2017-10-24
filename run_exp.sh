#!/bin/bash

#datadir="data/simulated_dags/150_5/"
#. exp_conf

nodenum=25

datadir="data/simulated_dags/${nodenum}/"

if [ -z "$1" ]; then
	echo "Usage: $0 <expname>"
	exit -1
fi

expname=$1

result_file="experiments/${nodenum}/${expname}.csv"
detail_log_dir="experiments/${nodenum}/${expname}_details/"

#alg_parameters="--pivot bound"
alg_parameters=`./get_exp_params.py ${expname}`

if [ -z "$alg_parameters" ]; then
	echo "The experiment is not setup, please add config into get_exp_params.py"
	exit 1
fi

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

