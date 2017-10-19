#!/bin/bash

n=$1

len=`echo -n $n | wc -m`

echo $n | sed -e "s/\([0-9]\)\([0-9]\{1,3\}\)[0-9]*/\1.\2e$((len-1))/"

