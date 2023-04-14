#!/bin/bash

if [ -z "$1" -o -z "$2" ]; then
	echo "Usage: $0 <ontology> <used|all>"
	exit 1
fi

ontology=$1
used=$2


for f in data/ontologies/levels/${ontology}/${used}_${ontology}*_rel.txt; do
	echo $f;
	./estimate_bound $f | ./grep_bound.sh |./to_float.sh ;
done

