/*
 * graph_alg.h
 *
 *  Created on: Mar 3, 2016
 *      Author: Shawn Peng
 */

#ifndef __GRAPH_ALG_H_
#define __GRAPH_ALG_H_

#include "dag.h"

#include <map>

using namespace std;

using namespace NS_DAG;



double count_consistent_subdag(DAG *g, const IdList &rootlist);


#endif /* __GRAPH_ALG_H_ */
