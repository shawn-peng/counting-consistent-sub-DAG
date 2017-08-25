#!/bin/bash

log=$1
if [ -z "$log" ]; then
	echo "Usage: $0 <log-file> [show-recursive-level]"
fi

level=$2

if [ -z "$level" ]; then
	level=8
fi

grep "\[[1-${level}]\]" ${log}
tail -f ${log} |grep "\[[1-${level}]\]"

