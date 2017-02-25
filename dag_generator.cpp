
#include "dag_generator.h"

#include "dag.h"
#include "common.h"

#include <vector>
#include <random>
#include <set>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>


#define MAX_NAME_LEN 63

using namespace NS_DAG;

static std::random_device rd;     // only used once to initialise (seed) engine
static std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)

int RandInt(int min, int max)
{
	std::uniform_int_distribution<int> uni(min,max); // guaranteed unbiased
	return uni(rng);
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
	}
	g->setRoot(1);
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

	printf("%s %s\n", col1_label, col2_label);
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



