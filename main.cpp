
#include "graph_alg.h"

#include "dag.h"
#include "common.h"
#include "dag_generator.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

#include <iostream>

using namespace std;
using namespace NS_DAG;

int main(int argc, char *argv[])
{
	const char *datafile = "data/basic_graphs/mini-tree.txt";
	int rootid = 1;
	IdList rootlist;
	if (argc >= 2)
	{
		datafile = argv[1];
	}
	if (argc >= 3)
	{
		sscanf(argv[2], "%d", &rootid);
	}
	DAG *g = create_dag_from_file(datafile);
	g->generateRoots();
	g->getRootList(rootlist);

	//g->setRoot(rootid);
	int ret;

	g->print();
	printf("Vertices: %d\n", g->getVertexNum());
	//g->print(988);
	list<DAG> subdags;
	if (g->getVertexNum() <= 35 && rootlist.size() == 1)
	{
		// sanity check
		rootid = rootlist.front();
		ret = get_consistent_subdag(g, rootid, subdags);
		//print_subdag_list(subdags);
	}

	//reduce_dag(g);

	//g->printEdges();

	//return 0;

	number_t num;
	//num = count_consistent_subdag_for_independent_subdag(g);
	//printf("Num of consistent sub-DAG: %.0f\n", num);
	//printf("====================================================");
	//printf("====================================================\n");
	num = count_consistent_subdag(g, rootlist);
	//printf("Num of consistent sub-DAG: %.0f\n", num);
	cout << "Num of consistent sub-DAG: " << num << endl;

	printf("(Sanity check)Num of consistent sub-DAG: %d\n", subdags.size());

	graph_alg_print_stats();

	free_dag(g);
	return 0;
}

