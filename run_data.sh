#!/bin/bash

dag="$1"
level="$2"
categ="$3"

if [ -z "$dag" -o -z "$level" ]; then
echo "Usage: $0 <mfo|cco|bpo> <level> [used|all]"
exit 1
fi

if [ -z "$categ" ]; then
	#run both
	{ time ./graph_alg data/gene_ontology/$dag/all_${dag}${level}_rel.txt; } &> log/${dag}${level}_all.log &
	{ time ./graph_alg data/gene_ontology/$dag/used_${dag}${level}_rel.txt; } &> log/${dag}${level}_used.log &
elif [[ "$categ" == "used" ]]; then
	{ time ./graph_alg data/gene_ontology/$dag/used_${dag}${level}_rel.txt; } &> log/${dag}${level}_used.log &
elif [[ "$categ" == "all" ]]; then
	{ time ./graph_alg data/gene_ontology/$dag/all_${dag}${level}_rel.txt; } &> log/${dag}${level}_all.log &
fi

