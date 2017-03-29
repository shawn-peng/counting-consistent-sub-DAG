
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
#include <boost/math/distributions/pareto.hpp>

using namespace std;
using namespace NS_DAG;

//static std::random_device rd;
static boost::random_device rd;
//static boost::mt19937 randGen(std::time(0)); //held constant for repeatability
static boost::mt19937 randGen(rd()); //held constant for repeatability
static boost::exponential_distribution<> dist;
//static boost::random::uniform_real_distribution<> uniformReal(1.0,10.0); //this range can be adjusted to effect values
static boost::variate_generator<boost::mt19937&, boost::exponential_distribution<> > generator(randGen, dist);

int paretoRand()
{
	double cdfComplement;
	//cdfComplement = boost::math::cdf(boost::math::complement(dist,generator()));

	cdfComplement = generator();
	//cout << "exponential : " << (int)cdfComplement << endl;
	//cout << "pareto : " << cdfComplement << endl;

	return exp(cdfComplement);
}



int main(int argc, char **argv)
{
	if (argc > 3)
	{
		printf("Usage: %s [vertex number]\n");
		return -1;
	}

	int nv;
	DAG *g;
	if (argc == 2)
	{
		string optstr = argv[1];
		if (optstr == "-h" || optstr == "--help")
		{
			printf("Usage: %s [vertex number]\n");
			return 0;
		}
		else
		{
			nv = stoi(optstr);
		}
		g = create_random_dag(nv, paretoRand);
	}
	else if (argc == 3)
	{
		string optstr = argv[1];
		if (optstr == "-c")
		{
			g = create_dag_from_file(argv[2]);
		}
	}

	//randGen.set

	//g->print();
	printf("source	target\n");
	g->printEdges();

	return 0;
}

