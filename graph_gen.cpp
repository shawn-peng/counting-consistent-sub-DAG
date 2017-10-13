
#include "dag.h"
#include "dag_generator.h"

#include <string>
#include <iostream>
#include <cmath>
#include <ctgmath>
#include <random>
#include <ctime>

#include <stdio.h>

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

//int Rand()
//{// Pareto
//	double cdfComplement;
//	cdfComplement = generator();
//	//cout << "exponential : " << (int)cdfComplement << endl;
//	//cout << "pareto : " << cdfComplement << endl;
//
//	return exp(cdfComplement);
//}
int Rand()
{
	int x = (*generator)() + 1;
	//cout << "rand: " << x << endl;
	return x;
}



int main(int argc, char **argv)
{
	if (argc > 4)
	{
		printf("Usage: %s [vertex number [depth [dist param]]]\n",
				argv[0]);
		return -1;
	}

	int nv;
	DAG *g;
	if (argc == 2)
	{
		string optstr = argv[1];
		if (optstr == "-h" || optstr == "--help")
		{
			printf("Usage: %s [vertex number [depth [dist param]]]\n", argv[0]);
			return 0;
		}
		else
		{
			nv = stoi(optstr);
		}

		double lambda = param_generator();
		dist = Dist_t(lambda);
		generator = new Generator_t(randGen, dist);

		g = create_random_dag(nv, Rand);
	}
	else if (argc == 3)
	{
		string optstr = argv[1];
		if (optstr == "-c")
		{
			g = create_dag_from_file(argv[2]);
		}
		else
		{
			nv = stoi(optstr);
			int depth = stoi(argv[2]);

			generator = new Generator_t(randGen, dist);
			g = create_random_dag_with_depth(nv, depth, Rand);
		}
	}
	else if (argc == 4)
	{
		nv = stoi(argv[1]);
		int depth = stoi(argv[2]);
		double param1 = atof(argv[3]);
		dist = Dist_t(param1);
		generator = new Generator_t(randGen, dist);
		g = create_random_dag_with_depth(nv, depth, Rand);
	}

	//randGen.set

	//g->print();
	printf("source	target\n");
	g->printEdges();

	delete generator;
	return 0;
}

