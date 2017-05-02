#ifndef _UTIL_H_
#define _UTIL_H_

#include <vector>
#include <set>
#include <string>
#include <utility>

#define DECOMPOSE_RANDOM     0
#define DECOMPOSE_TOPO       1
#define DECOMPOSE_REV_TOPO   2
#define DECOMPOSE_BOUND      3
#define DECOMPOSE_VERTEX     4
#define DECOMPOSE_MP_VERTEX  5
#define DECOMPOSE_EDGE       6
#define DECOMPOSE_CYCLOMATIC 7

#define DECOMPOSE_SUM 0
#define DECOMPOSE_MAX 1
#define DECOMPOSE_MIN 2

// Note that the construction of an object of this class didn't perform sanity
// check for cycles. It assumes the input graph is indeed a DAG.
class DAG {
    public:
        // Status of each vertex, true/false means in-/ex-cluded in the graph.
        // The original graph should have all true in this status vector.
        // Status will be also used as the hash key
        std::vector<bool> status;
        std::vector<std::set<int>> parents;
        std::vector<std::set<int>> children;

        // this count for each vertex account for the number of cdag rooted at
        // this vertex u. However, it is only used for u s.t. D+(u) forms an
        // independent component.
        std::vector<double> counts;

    public:
        DAG() {};
        DAG(int n) : status(n, false), parents(n), children(n), counts(n) {};
        DAG(const std::string); // load a DAG from a file (edge pairs)
        const DAG& operator=(const DAG& dag) {
            if (this != &dag) {
                status   = dag.status;
                parents  = dag.parents;
                children = dag.children;
                counts   = dag.counts;
            }
            return *this;
        }

        // This function tries to prune all independent components and store its
        // counts into the "root" of that component.
        void prune();

        // The following functions shouldn't modify the structure of the graph,
        // therefore, [const].

        bool is_forest() const;
        std::vector<int> topological_order() const;

        int num_vertices() const;
        int num_multi_parent_vertices() const;
        int num_multi_child_vertices() const;
        int num_roots() const;
        int num_edges() const;
        int num_components() const;

        // DAG induce(const std::vector<int>&) const;
        // Note that the algorithm only requires the implementaion of O[-S]
        // rather than O[S].
        DAG induce_minus_S(const std::vector<int>&) const;
        // induce an independent subgraph rooted at a given vertex.
        // Note that this function assumes the subgraph is indeed indepenedant.
        // I.e., no checking is performed.
        DAG induce_independent(int) const;

        std::vector<int> roots() const;
        std::vector<int> ancestors_plus(int) const;
        std::vector<int> descendants_plus(int) const;

        // Note: this function implements heuristic strategies to pick the
        // "best" multi-parent vertex to decompose.
        std::tuple<int, DAG, DAG> decompose() const;

        // find a list of roots of indepenent component in a reverse topological
        // order, returns an empty vector if not found.
        std::vector<int> independent_roots() const;
};

double ctree(const DAG&, int); // Algorithm 1
double cforest(const DAG&); // Extention of Algorithm 1
double cdag(DAG&); // Algorithm 2

// auxiliary functions
// returns the array of transitive closure (R+) of an element w.r.t. a
// transitive relationship R (represented in a vector of sets). That is, this
// function returns union({u}, {v | u R+ v}). E.g., if R is a "parent"
// relationship, then calling this function with an element u returns A+(u),
// which is the union of {u} and its ancestor set.
std::vector<int> transitive_closure_plus(const std::vector<std::set<int>>&, int);

// union-find algorithm
void UF_union(std::vector<int>&, int, int);
int UF_find(std::vector<int>&, int);

#endif // leaving _UTIL_H_
