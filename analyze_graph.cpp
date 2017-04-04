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


int get_min_depth(DAG *g)
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

	int dag_depth = 0;

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

		//printf("node %d depth: %d\n", id, cur_depth);
		//printf("dag_depth: %d\n", dag_depth);

		int next_depth = cur_depth + 1;

		IdList children;
		pv->getChildList(children);
		FOR_EACH_IN_CONTAINER(iter, children)
		{
			int chid = *iter;

			PrivDataUnion &chdata = g->getPrivData(chid);
			if (chdata.type == DT_INT && chdata.dint <= next_depth)
			{
				continue;
			}

			chdata.type = DT_INT;
			chdata.dint = next_depth;
			fringe.push_front(chid);
		}
	}

	IdList nodes;
	g->getVertexList(nodes);
	FOR_EACH_IN_CONTAINER(iter, nodes)
	{
		int cur_depth = g->getPrivData(*iter).dint;
		dag_depth = max(dag_depth, cur_depth);
	}
	return dag_depth;
}

int get_max_depth(DAG *g)
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
	if (argc > 3)
	{
		printf("Usage: %s [-s] <graph file>\n", argv[0]);
		return -1;
	}

	bool short_fmt = false;
	DAG *g;
	if (argc == 2)
	{
		string optstr = argv[1];
		if (optstr == "-h" || optstr == "--help")
		{
			printf("Usage: %s [-s] <graph file>\n", argv[0]);
			return 0;
		}
		g = create_dag_from_file(argv[1]);
		g->generateRoots();
	}
	else if (argc == 3)
	{
		string optstr = argv[1];
		//printf("optstr %s\n", optstr.c_str());
		if (optstr == "-s")
		{
			short_fmt = true;
		}
		else
		{
			printf("Usage: %s [-s] <graph file>\n", argv[0]);
			return -1;
		}
		g = create_dag_from_file(argv[2]);
		g->generateRoots();
	}
	else
	{
		printf("Usage: %s [-s] <graph file>\n", argv[0]);
		return -1;
	}

	//g->print();

	IdList leaves;
	get_leaves(g, leaves);

	int nv = g->getVertexNum();
	
	int ne = g->getEdgeNum();

	int level = get_max_depth(g);

	int depth = get_min_depth(g);

	IdList mpnodes;
	get_mpnodes(g, mpnodes);

	map<int, int> in_degree_stat;
	map<int, int> out_degree_stat;
	IdList nodes;
	g->getVertexList(nodes);
	FOR_EACH_IN_CONTAINER(iter, nodes)
	{
		int id = g->getParentNum(*iter);
		if (id < 10)
		{
			in_degree_stat[id]++;
		}
		else
		{// >= 10 all count as 10
			in_degree_stat[10]++;
		}
		int od = g->getChildNum(*iter);
		if (od < 10)
		{
			out_degree_stat[od]++;
		}
		else
		{// >= 10 all count as 10
			out_degree_stat[10]++;
		}
	}
	//printf("in-degrees: ");
	//FOR_EACH_IN_CONTAINER(iter, nodes)
	//{
	//	printf("%d ", g->getParentNum(*iter));
	//}
	//printf("\n");
	//printf("out-degrees: ");
	//FOR_EACH_IN_CONTAINER(iter, nodes)
	//{
	//	printf("%d ", g->getChildNum(*iter));
	//}
	//printf("\n");

	//printf("\n");
	//print_parent_numbers(g);

	//printf("\n");
	//print_child_numbers(g);


	if (short_fmt)
	{// short format
		printf("%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d\n",
				nv, ne, mpnodes.size(), level, depth,
				in_degree_stat[1],
				in_degree_stat[2],
				in_degree_stat[3],
				in_degree_stat[4],
				in_degree_stat[5],
				in_degree_stat[6],
				in_degree_stat[7],
				in_degree_stat[8],
				in_degree_stat[9],
				in_degree_stat[10],
				out_degree_stat[0],
				out_degree_stat[1],
				out_degree_stat[2],
				out_degree_stat[3],
				out_degree_stat[4],
				out_degree_stat[5],
				out_degree_stat[6],
				out_degree_stat[7],
				out_degree_stat[8],
				out_degree_stat[9],
				out_degree_stat[10]);
	}
	else
	{// long format
		printf("# of vertices : %d\n", nv);
		printf("# of edges: %d\n", ne);
		//printf("# of leaves: %d\n", leaves.size());
		printf("# of multi-parent vertices : %d\n", mpnodes.size());
		printf("level : %d\n", level);
		printf("depth : %d\n", depth);

		printf("\n");
		FOR_EACH_IN_CONTAINER(iter, in_degree_stat)
		{
			if (iter->first == 10)
			{
				printf("# of vertices having in-degree>=%d : %d\n",
						iter->first, iter->second);
			}
			else
			{
				printf("# of vertices having in-degree=%d : %d\n",
						iter->first, iter->second);
			}
		}

		printf("\n");
		FOR_EACH_IN_CONTAINER(iter, out_degree_stat)
		{
			if (iter->first == 10)
			{
				printf("# of vertices having out-degree>=%d : %d\n",
						iter->first, iter->second);
			}
			else
			{
				printf("# of vertices having out-degree=%d : %d\n",
						iter->first, iter->second);
			}
		}
	}



	return 0;
}

