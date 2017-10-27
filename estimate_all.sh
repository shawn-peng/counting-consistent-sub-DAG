
ontologies="mfo cco bpo"

for onto in $ontologies; do

	for gfile in `ls data/ontologies/levels/$onto/used*.txt`; do
		echo $gfile
		./estimate_bound $gfile
	done
done

