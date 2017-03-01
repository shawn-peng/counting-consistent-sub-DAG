#!/bin/bash

dag="$1"
level="$2"

if [ -z "$dag" -o -z "$level" ]; then
echo "Usage: $0 <mfo|cco|bpo> <level>"
exit 1
fi

./graph_alg data/gene_ontology/$dag/all_$dag$level_rel.txt > log/${dag}${level}_all.log &
./graph_alg data/gene_ontology/$dag/used_$dag$level_rel.txt > log/${dag}${level}_used.log &

