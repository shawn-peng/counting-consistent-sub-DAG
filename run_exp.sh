#!/bin/bash

#datadir="data/simulated_dags/150_5/"
#. exp_conf

if [ -z "$1" ]; then
	echo "Usage: $0 <expname> [nodenum]"
	exit -1
fi

if [ -z "$2" ]; then
	nodenum=100
else
	nodenum=$2
fi

datadir="data/simulated_dags/${nodenum}/"

expname=$1

result_file="experiments/${nodenum}/${expname}.csv"
detail_log_dir="experiments/${nodenum}/${expname}_details/"

#alg_parameters="--pivot bound"
alg_parameters=`./get_exp_params.py ${expname}`

if [ -z "$alg_parameters" ]; then
	echo "The experiment is not setup, please add config into get_exp_params.py"
	exit 1
fi

if [ -f "$result_file" ]; then
	rm -f $result_file
fi
if [ -d "$detail_log_dir" ]; then
	rm -f ${detail_log_dir}/*
fi

mkdir -p $detail_log_dir

echo "file	count	time(ms)	recursive_calls" > $result_file

usedtimes=()
for g in `ls $datadir`; do
	echo -n $g

	#fname=`echo $g | grep -oP "[^/]*$"`
	#echo $fname

	./graph_alg ${datadir}${g} ${alg_parameters} > ${detail_log_dir}${g} &
	algpid=$!

	trap "echo -e '\nTimeout' ; kill $algpid ; rm -f ${detail_log_dir}${g} ; exit 2" SIGINT SIGTERM

	wait $algpid
	trap - SIGINT SIGTERM

	t=`cat ${detail_log_dir}${g} | sed -ne 's/time: \([0-9]*\)ms/\1/p'`
	cnt=`cat ${detail_log_dir}${g} | ./grep_count.sh | ./to_float.sh`
	calls=`cat ${detail_log_dir}${g} | sed -ne 's/Function count_consistent_subdag_for_independent_subdag has been called \([0-9]*\) times./\1/p'`

	echo "$g	$cnt	$t	$calls" >> $result_file

	echo "		$cnt	$t	$calls"
	
	usedtimes+=($t)
done

#echo $usedtimes

