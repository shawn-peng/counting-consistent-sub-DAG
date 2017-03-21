#include "dag.h"
#include "dag_generator.h"
#include "common.h"

#include <assert.h>
#include <string>
#include <iostream>
#include <cmath>
#include <ctgmath>
#include <random>
#include <ctime>

#include <stdio.h>

using namespace std;
using namespace NS_DAG;

int get_leaves(DAG *g, IdList &l)
{
	IdList Nodes;
	g->getVertexList(Nodes);

	FOR_EACH_IN_CONTAINER(iter, Nodes)
	{
		IdList children;
		g->getChildList(*iter, children);
		if (children.size() == 0)
		{
			l.push_back(*iter);
		}
	}
	return 0;
}


int get_depth(DAG *g)
{
	IdList fringe;

	//int rootid = g->getRoot();
	//fringe.push_back(rootid);

	g->getRootList(fringe);
	FOR_EACH_IN_CONTAINER(iter, fringe)
	{
		int id = *iter;

		Vertex *pv = g->findVertex(id);
		assert(pv);

		PrivDataUnion &data = pv->getPrivData();
		data.dint = 1;
		data.type = DT_INT;
	}

	int max_depth = 0;

	// depth first traverse path dag
	while(!fringe.empty())
	{
		int id = fringe.front();
		fringe.pop_front();

		Vertex *pv = g->findVertex(id);
		assert(pv);

		PrivDataUnion &data = pv->getPrivData();
		assert(data.type == DT_INT);
		int cur_depth = data.dint;
		max_depth = max(max_depth, cur_depth);

		//printf("node %d depth: %d\n", id, cur_depth);
		//printf("max_depth: %d\n", max_depth);

		int next_depth = cur_depth + 1;

		IdList children;
		pv->getChildList(children);
		FOR_EACH_IN_CONTAINER(iter, children)
		{
			int chid = *iter;

			PrivDataUnion &chdata = g->getPrivData(chid);
			if (chdata.type == DT_INT && chdata.dint >= next_depth)
			{
				continue;
			}

			chdata.type = DT_INT;
			chdata.dint = next_depth;
			fringe.push_front(chid);
		}
	}
	return max_depth;
}

int get_mpnodes(DAG *g, IdList &mpnodes)
{
	IdList nodes;
	g->getVertexList(nodes);

	FOR_EACH_IN_CONTAINER(iter, nodes)
	{
		if (g->getParentNum(*iter) > 1)
		{
			mpnodes.push_back(*iter);
		}
	}
	return 0;
}

int print_parent_numbers(DAG *g)
{
	IdList nodes;
	g->getVertexList(nodes);

	FOR_EACH_IN_CONTAINER(iter, nodes)
	{
		printf("node %07d has %d parent(s)\n", *iter, g->getParentNum(*iter));
	}
	return 0;
}

int print_child_numbers(DAG *g)
{
	IdList nodes;
	g->getVertexList(nodes);

	FOR_EACH_IN_CONTAINER(iter, nodes)
	{
		IdList children;
		g->getChildList(*iter, children);
		printf("node %07d has %d children\n", *iter, children.size());
	}
	return 0;
}


int main(int argc, char **argv)
{
	if (argc > 2)
	{
		printf("Usage: %s <graph file>\n");
		return -1;
	}

	int nv;
	DAG *g;
	if (argc == 2)
	{
		string optstr = argv[1];
		if (optstr == "-h" || optstr == "--help")
		{
			printf("Usage: %s <graph file>\n");
			return 0;
		}
		g = create_dag_from_file(argv[1]);
		g->generateRoots();
	}
	else
	{
		printf("Usage: %s <graph file>\n");
		return -1;
	}

	//g->print();

	IdList leaves;
	get_leaves(g, leaves);
	printf("number of leaves: %d\n", leaves.size());

	int depth = get_depth(g);
	printf("depth : %d\n", depth);

	IdList mpnodes;
	get_mpnodes(g, mpnodes);
	printf("number of MP nodes: %d\n", mpnodes.size());

	printf("\n");
	print_parent_numbers(g);

	printf("\n");
	print_child_numbers(g);

	return 0;
}

