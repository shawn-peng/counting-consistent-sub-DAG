#!/bin/bash

logfile=$1

{ cat $logfile && tail -f $logfile; } | ./read_progress.pl


