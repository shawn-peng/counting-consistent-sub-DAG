#!/bin/bash

EXP=$1

result_file="experiments/methods/${1}.csv"
detail_log_dir="experiments/methods/${1}_details/"


echo "file	count	time(ms)	recursive_calls" > $result_file

usedtimes=()
totaltime=0
totalcalls=0

for f in `ls $detail_log_dir`; do
	echo $f

	#fname=`echo $f | grep -oP "[^/]*$"`
	#echo $fname

	t=`cat ${detail_log_dir}${f} | sed -ne 's/time: \([0-9]*\)ms/\1/p'`
	calls=`cat ${detail_log_dir}${f} | sed -ne 's/Function count_consistent_subdag_for_independent_subdag has been called \([0-9]*\) times./\1/p'`

	cnt=`cat ${detail_log_dir}${f} | sed -ne 's/Num of consistent sub-DAG: \([0-9]*\)/\1/p'`

	echo "$f	$cnt	$t	$calls" >> $result_file
	
	usedtimes+=($t)
	((totaltime+=t))
	((totalcalls+=calls))
done

((avgtime = totaltime/1000))
echo $totaltime
echo $avgtime

((avgcalls = totalcalls/1000))
echo $totalcalls
echo $avgcalls


