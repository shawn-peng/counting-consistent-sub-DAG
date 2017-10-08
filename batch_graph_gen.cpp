#include "dag.h"
#include "dag_generator.h"
#include "graph_alg.h"

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


//static string data_dir = "data/simulated_dags2/";
static string data_dir = "data/simulated_dags/";

void output_rand_dag(int nv, int depth)
{
	//cout << "nodes: " << nv << ", depth: " << depth << endl;
	//cout << "please input lambdas(end with 0):" << endl;
	//while (1)
	//{
	//	cin >> lambda;
	//	if (lambda == 0)
	//	{
	//		break;
	//	}
	//	params.push_back(lambda);
	//}

	//cout << "please input number of dags to be generated:" << endl;

	int backstdout = dup(STDOUT_FILENO);

	stringstream ss;
	ss << data_dir << nv << "_" << depth << "/";
	string subdir = ss.str();

	DAG *g;

	//for (int i = 0; i < ndags; i++)
	long long int i = 1;
	int overlapping_times = 0;
	std::set<number_t> counts;
	while (1)
	{
		double lambda = param_generator();
		if (depth == 2)
		{// all nodes should only have one parent, except the root which has none
			lambda = 0.0000001;
		}
		cerr << "lambda=" << lambda << endl;
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

		if (nv < 15 || depth <= 3 || depth >= nv-3)
		{
			IdList roots;
			g->getRootList(roots);
			number_t x = count_consistent_subdag(g, roots);
			graph_alg_clear_hash();

			if (counts.count(x))
			{
				cerr << "dag count(" << x << ") overlaps." << endl;
				overlapping_times++;

				delete generator;
				delete g;
				g = NULL;
				if (overlapping_times > max(10, (int)counts.size()))
				{
					cerr << "overlapping_times: " << overlapping_times << endl <<
						"dag generated: " << counts.size() << endl;
					break;
				}

				continue;
			}

			counts.insert(x);
		}
		//if (counts.size() > 1000)
		if (i > 1000)
		{
			cerr << "overlapping_times: " << overlapping_times << endl <<
				"dag generated: " << counts.size() << endl;
			delete generator;
			delete g;
			g = NULL;
			break;
		}

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

	for (int nv = 150; nv <= 150; nv++)
	{
		//for (int depth = 2; depth <= nv; depth++)
		for (int depth = 5; depth <= 5; depth++)
		{
			output_rand_dag(nv, depth);
		}
	}



	//delete generator;

	return 0;
}

