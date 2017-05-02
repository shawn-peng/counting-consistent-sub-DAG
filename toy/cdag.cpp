#include "util.h"
#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;

extern int num_recursive_calls;
extern int strategy;
extern int strategy_specific;
extern vector<int> topoorder;

int main(int argc, const char* argv[]) {
    srand(time(NULL));
    num_recursive_calls = 0;
    DAG dag(argv[1]);
    if (argc <= 2) {
        strategy = DECOMPOSE_BOUND;
    } else {
        string choice(argv[2]);
        if (choice.compare("topo") == 0) {
            strategy = DECOMPOSE_TOPO;
        } else if (choice.compare("rev_topo") == 0) {
            strategy = DECOMPOSE_REV_TOPO;
        } else if (choice.compare("random") == 0) {
            strategy = DECOMPOSE_RANDOM;
        } else if (choice.compare("vertex") == 0) {
            strategy = DECOMPOSE_VERTEX;
        } else if (choice.compare("mp_vertex") == 0) {
            strategy = DECOMPOSE_MP_VERTEX;
        } else if (choice.compare("edge") == 0) {
            strategy = DECOMPOSE_EDGE;
        } else if (choice.compare("bound") == 0) {
            strategy = DECOMPOSE_BOUND;
        } else if (choice.compare("cyclomatic") == 0) {
            strategy = DECOMPOSE_CYCLOMATIC;
        } else {
            cerr << "unknown strategy." << endl;
            exit(1);
        }
    }
    if (argc <= 3) {
        strategy_specific = DECOMPOSE_SUM;
    } else {
        string choice(argv[3]);
        if (choice.compare("sum") == 0) {
            strategy_specific = DECOMPOSE_SUM;
        } else if (choice.compare("max") == 0) {
            strategy_specific = DECOMPOSE_MAX;
        } else if (choice.compare("min") == 0) {
            strategy_specific = DECOMPOSE_MIN;
        } else {
            cerr << "unknonw strategy specifics." << endl;
            exit(1);
        }
    }
    // make a topological order ahead of time
    topoorder = dag.topological_order();
    cout << cdag(dag) << ",";
    cout << num_recursive_calls << endl;
    return 0;
}
