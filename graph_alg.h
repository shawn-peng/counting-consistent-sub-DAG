/*
 * graph_alg.h
 *
 *  Created on: Mar 3, 2016
 *      Author: Shawn Peng
 */

#ifndef __GRAPH_ALG_H_
#define __GRAPH_ALG_H_

#include "dag.h"

#include <list>
#include <map>

#include <gmpxx.h>

//using namespace std;

typedef mpz_class number_t;

//sanity check
int get_consistent_subdag(NS_DAG::DAG *g, int rootid, std::list<NS_DAG::DAG> &subdags);

number_t count_consistent_subdag(NS_DAG::DAG *g, int rootid);
number_t count_consistent_subdag(NS_DAG::DAG *g, const NS_DAG::IdList &rootlist);

void free_dag(NS_DAG::DAG *g);

//print statistics
void graph_alg_print_stats();

#endif /* __GRAPH_ALG_H_ */
