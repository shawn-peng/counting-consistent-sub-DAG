
#ifndef __DAG_GENERATOR_H_
#define __DAG_GENERATOR_H_

#include "dag.h"

typedef int (*rand_int_fn_t)();

/**
 * n, vertex number
 * rand_fn, function to generate rand for the number of parents
 */
NS_DAG::DAG *create_random_dag(int n, rand_int_fn_t rand_fn);
NS_DAG::DAG *create_random_dag_with_depth(int n, int depth, rand_int_fn_t rand_fn);

NS_DAG::DAG *create_dag_from_matfile(const char *filename);
NS_DAG::DAG *create_dag_from_file(const char *filename);

#endif


