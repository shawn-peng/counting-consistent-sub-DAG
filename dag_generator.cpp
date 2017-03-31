
#include "dag_generator.h"

#include "dag.h"
#include "common.h"

#include <iostream>
#include <vector>
#include <random>
#include <set>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <boost/random.hpp>
#include <boost/nondet_random.hpp>
#include <boost/random/mersenne_twister.hpp>
//#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_int.hpp>


#define MAX_NAME_LEN 63

using namespace std;
using namespace NS_DAG;

static boost::random_device rd;     // only used once to initialise (seed) engine
static boost::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)

int RandInt(int min, int max)
{
	//boost::random::uniform_int_distribution<int> uni(min,max); // guaranteed unbiased
	boost::uniform_int<> uni(min,max); // guaranteed unbiased
	return uni(rng);
	//return 0;
}

/*
template<class fwditer>
bidiiter random_unique(fwditer begin, fwditer end, size_t num_random)
{
	size_t left = std::distance(begin, end);
	while (num_random--)
	{
		fwditer r = begin;
		std::advance(r, rand()%left);
		std::swap(*begin, *r);
		++begin;
		--left;
	}
	return begin;
}
*/

int rand_draw_from(vector<int> &candidates)
{
	int idx = RandInt(0, candidates.size()-1);
	int val = candidates[idx];
	candidates.erase(candidates.begin() + idx);
	return val;
}

DAG *create_random_dag(int n, rand_int_fn_t rand_fn)
{
	DAG *g = new DAG();
	vector<int> all_nodes;
	for (int id = 1; id <= n; id++)
	{
		g->addVertex(id);

		int np = rand_fn();
		
		int nv = all_nodes.size();
		if (np > nv)
		{
			np = nv;
		}

		std::set<unsigned int> indices;
		while (indices.size() < np)
		{
			indices.insert(RandInt(0, nv-1));
		}

		FOR_EACH_IN_CONTAINER(indit, indices)
		{
			g->addEdge(all_nodes[*indit], id);
		}

		if (np == 0)
		{
			g->addToRootList(id);
		}
		all_nodes.push_back(id);
	}
	return g;
}


DAG *try_create_random_dag_with_depth(int n, int depth, rand_int_fn_t rand_fn)
{
	if (n < depth)
	{
		return NULL;
	}

	DAG *g = new DAG;

	// gen in-degree
	//std::map<int, int> in_degrees;
	//for (int i = 1; i <= n; i++)
	//{
	//	in_degrees[i] = rand_fn();
	//}
	std::vector<int> in_degrees;
	for (int i = 0; i < n; i++)
	{
		in_degrees.push_back(rand_fn());
	}
	in_degrees[0] = 0;
	//cout << "in-degrees: ";
	//for (int i = 0; i < n; i++)
	//{
	//	cout << in_degrees[i] << ", ";
	//}
	//cout << endl;


	std::vector<int> all_nodes;
	std::set<int> remain_nodes;
	std::vector<int> added_nodes;

	for (int i = 0; i < n; i++)
	{
		all_nodes.push_back(i+1);
		remain_nodes.insert(i+1);
	}

	std::vector<int> avail_nodes;
	for (int i = 2; i <= n; i++)
	{
		avail_nodes.push_back(i);
	}


	// create the spine
	std::vector<int> spine;
	g->addVertex(1);
	g->getPrivData(1).dint = 1;
	g->addToRootList(1);
	spine.push_back(1);
	added_nodes.push_back(1);
	remain_nodes.erase(1);

	int prev_node = 1;
	for (int i = 2; i <= depth; i++)
	{
		int cur_node = rand_draw_from(avail_nodes);
		//while (g->checkVertex(cur_node))
		//{
		//	cur_node = rand_draw_from(all_nodes);
		//}

		g->addVertex(cur_node);
		g->getPrivData(cur_node).dint = i;
		g->addEdge(prev_node, cur_node);
		spine.push_back(cur_node);
		added_nodes.push_back(cur_node);
		remain_nodes.erase(cur_node);
		prev_node = cur_node;
	}

	// add the reset nodes
	//std::set<int> avail_nodes;
	int nleaves = 1;
	avail_nodes.clear();
	for (int i = 0; i < n; i++)
	{
		int id = i + 1;
		if (in_degrees[i] <= depth - nleaves && !g->checkVertex(id))
		{
			avail_nodes.push_back(id);
			remain_nodes.erase(id);
		}
	}
	//cout << "available elements: " << endl;
	//for (auto e = avail_nodes.begin(); e != avail_nodes.end(); e++)
	//{
	//	cout << *e << ", ";
	//}
	//cout << endl;

	for (int i = depth; i < n; i++)
	{
		if (avail_nodes.size() == 0)
		{
			delete g;
			return NULL;
		}

		int cur_node = rand_draw_from(avail_nodes);
		//while (g->checkVertex(cur_node))
		//{
		//	cur_node = rand_draw_from(avail_nodes);
		//}
		//cout << "rand node: " << cur_node << endl;

		g->addVertex(cur_node);

		int np = in_degrees[cur_node-1];

		std::vector<int> possible_parents;
		for (int j = 0; j < added_nodes.size(); j++)
		{
			int id = added_nodes[j];
			if (g->getPrivData(id).dint < depth)
			{
				possible_parents.push_back(id);
			}
		}
		assert(possible_parents.size() >= np);

		int cur_depth = 0;
		for (int j = 0; j < np; j++)
		{
			int pid = rand_draw_from(possible_parents);
			g->addEdge(pid, cur_node);
			int pdepth = g->getPrivData(pid).dint;
			cur_depth = max(cur_depth, pdepth+1);
		}

		//set depth
		g->getPrivData(cur_node).dint = cur_depth;
		if (cur_depth == depth)
		{
			nleaves++;
		}

		int nv = g->getVertexNum();
		//add new available nodes
		for (int j = 0; j < n; j++)
		{
			int id = j + 1;
			//if (in_degrees[j] >= nv - nleaves)
			//{
			//	cout << "node " << id << " in-degree: " <<
			//		in_degrees[j] << endl;
			//}
			if ((in_degrees[j] <= nv - nleaves) &&
				remain_nodes.count(id))
			{
				//cout << "add " << id << " to candidates" << endl;
				avail_nodes.push_back(id);
				remain_nodes.erase(id);
			}
		}

		added_nodes.push_back(cur_node);
	}

	// finish spine's open edges
	prev_node = 1;
	for (int i = 1; i < depth; i++)
	{
		int cur_node = spine[i];

		int new_np = in_degrees[cur_node-1] - 1; // already have one

		std::vector<int> possible_parents;
		for (int j = 0; j < added_nodes.size(); j++)
		{
			int id = added_nodes[j];
			if (g->getPrivData(id).dint <= i &&
				id != prev_node)
			{
				possible_parents.push_back(id);
			}
		}
		if (possible_parents.size() < new_np)
		{
			delete g;
			return NULL;
		}
		for (int j = 0; j < new_np; j++)
		{
			int pid = rand_draw_from(possible_parents);
			g->addEdge(pid, cur_node);
		}
		prev_node = cur_node;
	}

	int sum = 0;
	cerr << "in-degrees: ";
	for (int i = 0; i < n; i++)
	{
		cerr << in_degrees[i] << " ";
		sum += in_degrees[i];
	}
	cerr << endl;
	cerr << "number of edges should be " << sum << endl;
	
	return g;
}
DAG *create_random_dag_with_depth(int n, int depth, rand_int_fn_t rand_fn)
{
	DAG *g = NULL;
	int retries = 0;
	while (g == NULL)
	{
		g = try_create_random_dag_with_depth(n, depth, rand_fn);
		if (retries++ >= 100)
		{
			cerr << "inappropriate parameters." << endl;
			break;
		}
	}
	return g;
}



DAG *create_dag_from_matfile(const char *filename)
{
	FILE *f = fopen(filename, "r");
	if (f == NULL)
	{
		perror(__FUNCTION__);
		exit(1);
	}

	DAG *g = new DAG();
	g->addVertex(1);

	int i = 1, j = 1;
	char c = fgetc(f);
	while (c != EOF)
	{
		switch (c)
		{
			case '0':
				j++;
				break;
			case '1':
				g->addVertex(j);
				g->addEdge(i, j);
				j++;
				break;
			case ' ':
				break;
			case '\n':
				j = 1;
				i++;
				break;
			case '\r':
				break;
			default:
				assert(0);
				exit(1);
				break;
		}
		c = fgetc(f);
	}

	if (errno)
	{
		perror(__FUNCTION__);
		exit(1);
	}


	fclose(f);
	return g;
}


DAG *create_dag_from_file(const char *filename)
{
	FILE *f = fopen(filename, "r");
	if (f == NULL)
	{
		perror(__FUNCTION__);
		exit(1);
	}

	char col1_label[MAX_NAME_LEN]; // the header wouldn't be larger than this
	char col2_label[MAX_NAME_LEN]; // the header wouldn't be larger than this
	int ret;

	ret = fscanf(f, "%s%s\n", col1_label, col2_label);

	//printf("%s %s\n", col1_label, col2_label);
	if (ret != 2)
	{
		perror(__FUNCTION__);
	}

	if (col1_label[0] == '0')
	{
		// This file is a matrix representation
		fclose(f);
		return create_dag_from_matfile(filename);
	}

	DAG *g = new DAG();

	int go_prefix = true;
	int parent_first = false;

	if (strcmp(col1_label, "parent") == 0 ||
			strcmp(col1_label, "source") == 0)
	{
		parent_first = true;
	}

	while (ret == 2)
	{
		int vstart, vend;

		if (go_prefix)
		{
			ret = fscanf(f, "GO:%d\tGO:%d\n", &vend, &vstart);
		}
		else
		{
			ret = fscanf(f, "%d%d\n", &vend, &vstart);
		}

		if (ret != 2)
		{
			if (ret == EOF)
			{
				break;
			}
			go_prefix = false;
			ret = fscanf(f, "%d%d\n", &vend, &vstart);
		}

		if (parent_first)
		{
			swap(vend, vstart);
		}

		if (ret != 2)
		{
			if (errno)
			{
				perror(__FUNCTION__);
			}
		}
		else
		{
			g->addVertex(vstart);
			g->addVertex(vend);

			g->addEdge(vstart, vend);

			//g->print();
		}
	}

	fclose(f);
	return g;
}



