#!/bin/bash

logfile=$1

if [ "$2" == "-f" ]; then
	{ cat $logfile && tail -f $logfile; } | ./read_progress.pl
else
	cat $logfile | ./read_progress.pl
fi


