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

number_t count_consistent_subdag(NS_DAG::DAG *g, int rootid, bool using_hash = true);
number_t count_consistent_subdag(NS_DAG::DAG *g, const NS_DAG::IdList &rootlist, bool using_hash = true);

number_t estimate_upper_bound(NS_DAG::DAG *g, int method);

int get_ancestors_subdag(NS_DAG::DAG *g, int id, NS_DAG::DAG &subdag);
int get_descendants_subdag(NS_DAG::DAG *g, int id, NS_DAG::DAG &subdag);

void free_dag(NS_DAG::DAG *g);

//print statistics
void graph_alg_print_stats();

//clear static info (NEEDED to count for different dags in one process)
void graph_alg_clear_hash();

void graph_alg_enable_hashing();
void graph_alg_disable_hashing();

void graph_alg_enable_logging();
void graph_alg_disable_logging();

void graph_alg_enable_pruning();
void graph_alg_disable_pruning();

void graph_alg_set_pivoting_method(const std::string &methodname);

#endif /* __GRAPH_ALG_H_ */
