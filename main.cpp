
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
#include <chrono>

using namespace std;
using namespace std::chrono;
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

	//g->setSingleRoot(rootid);
	int ret;

	g->print();
	printf("Vertices: %d\n", g->getVertexNum());
	//g->print(988);
	list<DAG> subdags;
	number_t check_num;
	//if (g->getVertexNum() <= 35)
	//{
	//	if (rootlist.size() == 1)
	//	{
	//		// sanity check
	//		rootid = rootlist.front();
	//		ret = get_consistent_subdag(g, rootid, subdags);
	//		//print_subdag_list(subdags);
	//	}
	//	else
	//	{
	//		DAG new_g(*g);
	//		new_g.addVertex(0);
	//		FOR_EACH_IN_CONTAINER(iter, rootlist)
	//		{
	//			new_g.addEdge(0, *iter);
	//		}
	//		new_g.setSingleRoot(0);// set as the only root
	//		//rootid = rootlist.front();
	//		printf("DAG with virtual root:\n");
	//		new_g.print();
	//		ret = get_consistent_subdag(&new_g, 0, subdags);
	//		// remove the subdag with only the virtual root
	//		subdags.pop_front();
	//	}
	//	check_num = subdags.size() + 1;
	//}
	//else
	//{
	//	DAG rg(*g);
	//	rg.reverse();
	//	IdList rroots;
	//	rg.getRootList(rroots);
	//	check_num = count_consistent_subdag(&rg, rroots);
	//	graph_alg_clear_hash();
	//}

	//reduce_dag(g);

	//g->printEdges();

	//return 0;
	
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	number_t num;
	//num = count_consistent_subdag_for_independent_subdag(g);
	//printf("Num of consistent sub-DAG: %.0f\n", num);
	//printf("====================================================");
	//printf("====================================================\n");
	num = count_consistent_subdag(g, rootlist);

	high_resolution_clock::time_point t2 = high_resolution_clock::now();


	//printf("Num of consistent sub-DAG: %.0f\n", num);
	cout << "Num of consistent sub-DAG: " << num << endl;

	//printf("(Sanity check)Num of consistent sub-DAG: %d\n", subdags.size());
	cout << "(Sanity check)Num of consistent sub-DAG: " << check_num << endl;

	graph_alg_print_stats();
	graph_alg_clear_hash();

	auto duration = duration_cast<microseconds>(t2 - t1).count();
	cout << duration;

	free_dag(g);
	return 0;
}


