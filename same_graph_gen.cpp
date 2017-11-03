#include "dag.h"
#include "dag_generator.h"
#include "graph_alg.h"
#include "common.h"

#include <string>
#include <iostream>
#include <cmath>
#include <ctgmath>
#include <random>
#include <ctime>
#include <sstream>
#include <set>

#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <boost/random.hpp>
//#include <boost/random/uniform_real_distribution.hpp>
#include <boost/nondet_random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/poisson_distribution.hpp>
#include <boost/random/gamma_distribution.hpp>

using namespace std;
using namespace NS_DAG;

static boost::random_device rd;

static boost::mt19937 randGen(rd());

typedef boost::poisson_distribution<int> Dist_t;
typedef boost::variate_generator<boost::mt19937&, Dist_t> Generator_t;

typedef boost::gamma_distribution<double> ParamDist_t;
typedef boost::variate_generator<boost::mt19937&, ParamDist_t> ParamGenerator_t;

static Dist_t dist;
static Generator_t *generator;

static ParamDist_t paramdist(2.0);
static ParamGenerator_t param_generator(randGen, paramdist);

int Rand()
{
	int x = (*generator)() + 1;
	return x;
}


static string data_dir = "data/same_count/";

static number_t g_count = 0;


int remove_redundant_edges(DAG *g)
{
	IdList nodes;
	g->getVertexList(nodes);
	FOR_EACH_IN_CONTAINER(iter, nodes)
	{
		int id = *iter;
		IdList parents;
		g->getParentList(id, parents);

		FOR_EACH_IN_CONTAINER(pit, parents)
		{
			int parent = *pit;
			DAG pA;
			get_ancestors_subdag(g, parent, pA);
			FOR_EACH_IN_CONTAINER(opit, parents)
			{
				int otherp = *opit;
				if (otherp != parent && pA.checkVertex(otherp))
				{
					g->removeEdge(otherp, id);
					//cerr << "removing " << otherp << "->" << id << endl;
				}
			}
		}
	}

	return 0;
}

void output_rand_dag(int nv, int depth)
{
	int backstdout = dup(STDOUT_FILENO);

	stringstream ss;
	ss << data_dir << nv << "/";
	string subdir = ss.str();

	DAG *g;

	long long int i = 0;
	int overlapping_times = 0;
	std::set<number_t> counts;
	while (1)
	{
		double lambda = param_generator();
		dist = Dist_t(lambda);
		generator = new Generator_t(randGen, dist);

		g = create_random_dag_with_depth(nv, depth, Rand);
		if (g == NULL)
		{
			delete generator;
			delete g;
			g = NULL;
			continue;
		}

		//IdList roots;
		//g->getRootList(roots);
		number_t x = count_consistent_subdag(g);
		graph_alg_clear_hash();

		if (g_count != x)
		{
			if (g_count == 0)
			{
				g_count = x;
			}
			else
			{
				delete generator;
				delete g;
				g = NULL;
				continue;
			}
		}

		remove_redundant_edges(g);

		struct stat st = {0};

		if (stat(subdir.c_str(), &st) == -1) {
			mkdir(subdir.c_str(), 0755);
		}
		string filename = subdir + to_string(i) + ".txt";
		cerr << filename << endl;
		FILE *of = freopen(filename.c_str(), "w", stdout);
		if (of == NULL)
		{
			perror("freopen():");
			exit(-1);
		}
		printf("source	target\n");
		g->printEdges();
		fflush(of);
		dup2(backstdout, STDOUT_FILENO);
		fclose(of);

		delete generator;

		delete g;
		g = NULL;

		i++;

		if (i > 10)
		{
			delete generator;
			delete g;
			g = NULL;
			break;
		}
	}

	return;
}

int main(int argc, char **argv)
{
	DAG *g;

	int nv;
	int depth;
	double lambda;
	vector<double> params;

	//string categ;
	stringstream ss;

	graph_alg_enable_hashing();
	graph_alg_enable_pruning();
	graph_alg_set_pivoting_method("degree");

	nv = 20;
	depth = 7;
	output_rand_dag(nv, depth);



	//delete generator;

	return 0;
}

