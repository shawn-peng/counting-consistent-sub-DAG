#!/bin/bash

#EXP=$1

detaildirs=`ls experiments/pivots/ | grep _details`

exps=${detaildirs//_details/}

for EXP in $exps; do
echo $EXP

result_file="experiments/pivots/${EXP}.csv"
detail_log_dir="experiments/pivots/${EXP}_details/"


echo "file	count	time(ms)	recursive_calls" > $result_file

usedtimes=()
totaltime=0
totalcalls=0

((i = 0))
for f in `ls $detail_log_dir`; do
	echo -ne "$f\r"

	#fname=`echo $f | grep -oP "[^/]*$"`
	#echo $fname

	t=`cat ${detail_log_dir}${f} | sed -ne 's/time: \([0-9]*\)ms/\1/p'`
	calls=`cat ${detail_log_dir}${f} | sed -ne 's/Function count_consistent_subdag_for_independent_subdag has been called \([0-9]*\) times./\1/p'`

	cnt=`cat ${detail_log_dir}${f} | sed -ne 's/Num of consistent sub-DAG: \([0-9]*\)/\1/p'`

	echo "$f	$cnt	$t	$calls" >> $result_file
	
	usedtimes+=($t)
	((totaltime+=t))
	((totalcalls+=calls))

	echo -ne "\033[K"
done

((avgtime = totaltime/1000))
echo $totaltime
#echo $avgtime

((avgcalls = totalcalls/1000))
echo $totalcalls
#echo $avgcalls

done
exit

