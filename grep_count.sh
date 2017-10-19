#!/bin/bash


f=$1

#t=`cat ${detail_log_dir}${f} | sed -ne 's/time: \([0-9]*\)ms/\1/p'`
#calls=`cat ${detail_log_dir}${f} | sed -ne 's/Function count_consistent_subdag_for_independent_subdag has been called \([0-9]*\) times./\1/p'`

cnt=`tail -n20 ${f} | sed -ne 's/Num of consistent sub-DAG: \([0-9]*\)/\1/p'`

echo $cnt

