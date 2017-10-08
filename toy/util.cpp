#include "util.h"
#include <unordered_map>
#include <tuple>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <climits>
#include <algorithm>

using namespace std;

// global hash table for results
unordered_map<vector<bool>, double> results;

int num_recursive_calls;
int strategy;
int strategy_specific;
vector<int> topoorder;

// construct a DAG from a file of edge list
DAG::DAG(const std::string filename) {
    // This function assumes the input file is formatted a two-column tab
    // splitted file with <parent> <child>, that is, each row of the input file
    // specifies an edge in the DAG. Note that the order of input edges shouln't
    // matter.
    set<string> keywords;
    keywords.insert("parent");
    keywords.insert("child");
    ifstream ifs(filename, ifstream::in);
    string buf;
    vector<pair<int,int>> edges;
    unordered_map<string,int> registered_vertex;
    int count = 0; // the number of registered vertices
    // read input
    while (ifs.good()) {
        getline(ifs, buf);
        if (buf.empty()) continue; // skip blank lines
        istringstream iss(buf);
        string parent, child;
        int pid, cid;
        iss >> parent >> child;
        if (keywords.find(parent) != keywords.end()) {
            // parents cannot be named as any of the keywords, otherwise
            // skipped.
            continue;
        }
        if (registered_vertex.find(parent) != registered_vertex.end()) {
            pid = registered_vertex.at(parent);
        } else {
            // register a new entry for this new vertex
            registered_vertex[parent] = count;
            pid = count;
            count ++;
        }
        if (registered_vertex.find(child) != registered_vertex.end()) {
            cid = registered_vertex.at(child);
        } else {
            registered_vertex[child] = count;
            cid = count;
            count ++;
        }
        edges.push_back(make_pair(pid, cid));
    }
    // create an DAG object
    status = vector<bool>(count, true);
    parents = children = vector<set<int>>(count);
    counts = vector<double>(count, 2); // initialize counts to be 2
    for (auto const& edge : edges) {
        int src, dst;
        tie(src, dst) = edge;
        children[src].insert(dst);
        parents[dst].insert(src);
    }
    ifs.close();
}

// prune the graph
void DAG::prune() {
    vector<int> iroots = independent_roots();
    for (auto const& iroot : iroots) {
        DAG dag = induce_independent(iroot);

        // prune D(iroot), update the counts on iroot
        // note that this process assume D(iroot) is an independent component,
        // that is there is not in-coming edges from the rest of graph into
        // D(iroot).

        double icount = 2;
        // double icount = cdag(dag);
        queue<int> q;
        q.push(iroot);
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            if (v != iroot) {
                status[v] = false;
                parents[v].clear();
            }
            for (auto const& c : children[v]) {
                q.push(c);
            }
            children[v].clear();
            counts[v] = 2; // reset to initial, although shouldn't matter
        }
        counts[iroot] = icount; // update its count

    }
}

// implements the operation of vertex-induced subgraph by V-S
DAG DAG::induce_minus_S(const std::vector<int>& S) const {
    DAG dag = *this;
    // remove vertices in S and all edges links from/to them
    for (auto const& vertex: S) {
        dag.status[vertex] = false;
        for (auto const& p : dag.parents[vertex]) {
            dag.children[p].erase(dag.children[p].find(vertex));
        }
        dag.parents[vertex].clear();
        for (auto const& c : dag.children[vertex]) {
            dag.parents[c].erase(dag.parents[c].find(vertex));
        }
        dag.children[vertex].clear();
        dag.counts[vertex] = 2; // reset to initial value, although it shouldn't matter
    }
    return dag;
}

// implements the operation of vertex-induced subgraph by D+(r)
DAG DAG::induce_independent(int r) const {
    DAG dag(status.size());
    dag.status[r] = true;
    dag.children[r] = children[r];
    // copy D(r) into the induced subgraph
    queue<int> q;
    for (auto const& c : children[r]) {
        q.push(c);
    }
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (auto const& c : children[v]) {
            q.push(c);
        }
        dag.status[v] = true;
        dag.parents[v] = parents[v];
        dag.children[v] = children[v];
        dag.counts[v] = counts[v];
    }
    return dag;
}

// returns an array of roots
std::vector<int> DAG::roots() const {
    vector<int> r;
    for (size_t i = 0; i < status.size(); ++i) {
        if (status[i] && parents[i].empty()) {
            r.push_back(i);
        }
    }
    return r;
}

// returns the list of ancestors of a vertex u and u itself
std::vector<int> DAG::ancestors_plus(int u) const {
    return transitive_closure_plus(parents, u);
}

// returns the list of descendants of a vertex u and u itself
std::vector<int> DAG::descendants_plus(int u) const {
    return transitive_closure_plus(children, u);
}

// returns true if it doesn't have multi-parent vertices
bool DAG::is_forest() const {
    for (size_t i = 0; i < status.size(); ++i) {
        if (status[i] && parents[i].size() > 1) {
            return false;
        }
    }
    return true;
}

// returns a topological order of the DAG
std::vector<int> DAG::topological_order() const {
    vector<int> order;
    unordered_map<int,int> parent_count;
    queue<int> q;
    for (size_t i = 0; i < status.size(); ++i) {
        if (status[i]) {
            parent_count[i] = parents[i].size();
            if (parents[i].empty()) {
                q.push(i);
            }
        }
    }
    while (!q.empty()) {
        int u = q.front();
        order.push_back(u);
        q.pop();
        for (auto const& child : children[u]) {
            parent_count[child] -= 1;
            if (parent_count.at(child) == 0) {
                q.push(child);
            }
        }
    }
    return order;
}

// returns the number of vertices
int DAG::num_vertices() const {
    int count = 0;
    for (size_t i = 0; i < status.size(); ++i) {
        if (status[i]) {
            count += 1;
        }
    }
    return count;
}

// returns the number of multi-parent vertices
int DAG::num_multi_parent_vertices() const {
    int count = 0;
    for (size_t i = 0; i < status.size(); ++i) {
        if (status[i] && (parents[i].size() > 1)) {
            count += 1;
        }
    }
    return count;
}

// returns the number of multi-child vertices
int DAG::num_multi_child_vertices() const {
    int count = 0;
    for (size_t i = 0; i < status.size(); ++i) {
        if (status[i] && (children[i].size() > 1)) {
            count += 1;
        }
    }
    return count;
}

// returns the number of roots
int DAG::num_roots() const {
    int count = 0;
    for (size_t i = 0; i < status.size(); ++i) {
        if (status[i] && parents[i].empty()) {
            count += 1;
        }
    }
    return count;
}

// returns the number of edges
int DAG::num_edges() const {
    int count = 0;
    for (size_t i = 0; i < status.size(); ++i) {
        if (status[i]) {
            count += children[i].size();
        }
    }
    return count;
}

// returns the number of connected components
int DAG::num_components() const {
    vector<int> p(status.size(), -1);
    vector<pair<int, int>> edges;
    for (size_t i = 0; i < status.size(); ++i) {
        if (status[i]) {
            p[i] = i;
            for (auto const& c : children[i]) {
                edges.push_back(make_pair(i, c));
            }
        }
    }
    for (auto const& edge : edges) {
        UF_union(p, edge.first, edge.second);
    }
    set<int> cc_index;
    for (size_t i = 0; i < p.size(); ++i) {
        if (p[i] >= 0) {
            cc_index.insert(UF_find(p, i));
        }
    }
    return cc_index.size();
}

// decomposes the graph into two subgraphs.
std::tuple<int, DAG, DAG> DAG::decompose() const {
    // (1) induced by all vertices but D+(u)
    // (2) induced by all vertices but A+(u)
    int best_u = -1;
    switch (strategy) {
        case DECOMPOSE_TOPO:
            {
                for (size_t i = 0; i < topoorder.size(); ++i) {
                    if (status[topoorder[i]] && parents[topoorder[i]].size() > 1) {
                        best_u = topoorder[i];
                        break;
                    }
                }
            }
            break;
        case DECOMPOSE_REV_TOPO:
            {
                for (size_t i = topoorder.size()-1; i >= 0; --i) {
                    if (status[topoorder[i]] && parents[topoorder[i]].size() > 1) {
                        best_u = topoorder[i];
                        break;
                    }
                }
            }
            break;
        case DECOMPOSE_VERTEX:
            {
                int best_statistic = INT_MAX;
                for (size_t i = 0; i < status.size(); ++i) {
                    if (status[i] && parents[i].size() > 1) {
                        DAG wo_D_plus = induce_minus_S(descendants_plus(i));
                        DAG wo_A_plus = induce_minus_S(ancestors_plus(i));
                        int statistic_wo_D_plus = wo_D_plus.num_vertices();
                        int statistic_wo_A_plus = wo_A_plus.num_vertices();
                        int statistic;
                        if (strategy_specific == DECOMPOSE_MAX) {
                            statistic = std::max(statistic_wo_D_plus, statistic_wo_A_plus);
                        } else if (strategy_specific == DECOMPOSE_MIN) {
                            statistic = std::min(statistic_wo_D_plus, statistic_wo_A_plus);
                        } else {
                            statistic = statistic_wo_D_plus + statistic_wo_A_plus;
                        }
                        if (best_statistic > statistic) {
                            best_statistic = statistic;
                            best_u = i;
                        }
                    }
                }
            }
            break;
        case DECOMPOSE_MP_VERTEX:
            {
                int best_statistic = INT_MAX;
                for (size_t i = 0; i < status.size(); ++i) {
                    if (status[i] && parents[i].size() > 1) {
                        DAG wo_D_plus = induce_minus_S(descendants_plus(i));
                        DAG wo_A_plus = induce_minus_S(ancestors_plus(i));
                        int statistic_wo_D_plus = wo_D_plus.num_multi_parent_vertices();
                        int statistic_wo_A_plus = wo_A_plus.num_multi_parent_vertices();
                        int statistic;
                        if (strategy_specific == DECOMPOSE_MAX) {
                            statistic = std::max(statistic_wo_D_plus, statistic_wo_A_plus);
                        } else if (strategy_specific == DECOMPOSE_MIN) {
                            statistic = std::min(statistic_wo_D_plus, statistic_wo_A_plus);
                        } else {
                            statistic = statistic_wo_D_plus + statistic_wo_A_plus;
                        }
                        if (best_statistic > statistic) {
                            best_statistic = statistic;
                            best_u = i;
                        }
                    }
                }
            }
            break;
        case DECOMPOSE_EDGE:
            {
                int best_statistic = INT_MAX;
                for (size_t i = 0; i < status.size(); ++i) {
                    if (status[i] && parents[i].size() > 1) {
                        DAG wo_D_plus = induce_minus_S(descendants_plus(i));
                        DAG wo_A_plus = induce_minus_S(ancestors_plus(i));
                        int statistic_wo_D_plus = wo_D_plus.num_edges();
                        int statistic_wo_A_plus = wo_A_plus.num_edges();
                        int statistic;
                        if (strategy_specific == DECOMPOSE_MAX) {
                            statistic = std::max(statistic_wo_D_plus, statistic_wo_A_plus);
                        } else if (strategy_specific == DECOMPOSE_MIN) {
                            statistic = std::min(statistic_wo_D_plus, statistic_wo_A_plus);
                        } else {
                            statistic = statistic_wo_D_plus + statistic_wo_A_plus;
                        }
                        if (best_statistic > statistic) {
                            best_statistic = statistic;
                            best_u = i;
                        }
                    }
                }
            }
            break;
        case DECOMPOSE_RANDOM:
            {
                vector<int> mpvs;
                for (size_t i = 0; i < status.size(); ++i) {
                    if (status[i] && parents[i].size() > 1) {
                        mpvs.push_back(i);
                    }
                }
                if (mpvs.empty()) break; // shouldn't happen though
                best_u = mpvs[rand() % mpvs.size()];
            }
            break;
        case DECOMPOSE_BOUND:
            {
                int best_statistic = INT_MAX;
                for (size_t i = 0; i < status.size(); ++i) {
                    if (status[i] && parents[i].size() > 1) {
                        DAG wo_D_plus = induce_minus_S(descendants_plus(i));
                        DAG wo_A_plus = induce_minus_S(ancestors_plus(i));
                        int statistic_wo_D_plus = wo_D_plus.num_edges() + wo_D_plus.num_roots() - wo_D_plus.num_vertices();
                        int statistic_wo_A_plus = wo_A_plus.num_edges() + wo_A_plus.num_roots() - wo_A_plus.num_vertices();
                        int statistic;
                        if (strategy_specific == DECOMPOSE_MAX) {
                            statistic = std::max(statistic_wo_D_plus, statistic_wo_A_plus);
                        } else if (strategy_specific == DECOMPOSE_MIN) {
                            statistic = std::min(statistic_wo_D_plus, statistic_wo_A_plus);
                        } else {
                            statistic = statistic_wo_D_plus + statistic_wo_A_plus;
                        }
                        if (best_statistic > statistic) {
                            best_statistic = statistic;
                            best_u = i;
                        }
                    }
                }
            }
            break;
        case DECOMPOSE_CYCLOMATIC:
            {
                int best_statistic = INT_MAX;
                for (size_t i = 0; i < status.size(); ++i) {
                    if (status[i] && parents[i].size() > 1) {
                        DAG wo_D_plus = induce_minus_S(descendants_plus(i));
                        DAG wo_A_plus = induce_minus_S(ancestors_plus(i));
                        int statistic_wo_D_plus = wo_D_plus.num_edges() + wo_D_plus.num_components() - wo_D_plus.num_vertices();
                        int statistic_wo_A_plus = wo_A_plus.num_edges() + wo_A_plus.num_components() - wo_A_plus.num_vertices();
                        int statistic;
                        if (strategy_specific == DECOMPOSE_MAX) {
                            statistic = std::max(statistic_wo_D_plus, statistic_wo_A_plus);
                        } else if (strategy_specific == DECOMPOSE_MIN) {
                            statistic = std::min(statistic_wo_D_plus, statistic_wo_A_plus);
                        } else {
                            statistic = statistic_wo_D_plus + statistic_wo_A_plus;
                        }
                        if (best_statistic > statistic) {
                            best_statistic = statistic;
                            best_u = i;
                        }
                    }
                }
            }
            break;
        default:
            cerr << "strategy not implemented." << endl;
            exit(1);
    }
    if (best_u == -1) {
        cerr << "cannot find the best vertex for decomposition." << endl;
        exit(1);
    }
    DAG d = induce_minus_S(descendants_plus(best_u));
    DAG a = induce_minus_S(ancestors_plus(best_u));
    return make_tuple(best_u, d, a);
    // return make_tuple(best_u, induce_minus_S(descendants_plus(best_u)), induce_minus_S(ancestors_plus(best_u)));
}

// returns a vector of vertices as the root of independent components
std::vector<int> DAG::independent_roots() const {
    vector<int> rev_topoorder;
    unordered_map<int, double> accumulated_flow;
    unordered_map<int, set<int>> descendants;
    for (auto it = topoorder.end(); it != topoorder.begin(); --it) {
        int v = *(it-1);
        if (status[v]) {
            rev_topoorder.push_back(v);
            accumulated_flow[v] = 0.0;
            descendants[v]      = set<int>();
        }
    }
    for (auto const& v : rev_topoorder) {
        double d = static_cast<double>(parents[v].size());
        for (auto const& p : parents[v]) {
            accumulated_flow[p] += ((accumulated_flow[v] + 1.0)/d);
            descendants[p].insert(v);
            descendants[p].insert(descendants[v].begin(), descendants[v].end());
        }
    }
    vector<int> iroots;
    for (auto const& v : rev_topoorder) {
        if ((!descendants[v].empty()) && parents[v].size() > 0 && std::abs(static_cast<double>(descendants[v].size()) - accumulated_flow[v]) < 1e-8) {
            iroots.push_back(v);
        }
    }
    return iroots;
}

// count cdags given a DAG
double cdag(DAG& dag) {
    num_recursive_calls += 1;
    // search for hashed results
    unordered_map<vector<bool>, double>::const_iterator it = results.find(dag.status);
    if (it != results.end()) {
        return (*it).second;
    } else {
        // if (dag.num_multi_child_vertices() < dag.num_multi_parent_vertices()) {
        //     swap(dag.parents, dag.children);
        // }
        // dag.prune(); // identify and prune independent subgraphs into leaves
        double result;
        if (dag.is_forest()) {
            result = cforest(dag);
        } else {
            DAG wo_D_plus, wo_A_plus;
            int pivot;
            tie(pivot, wo_D_plus, wo_A_plus) = dag.decompose();
            result = cdag(wo_D_plus) + cdag(wo_A_plus) * (dag.counts[pivot] - 1);
            // result = cdag(wo_D_plus) + cdag(wo_A_plus);
        }
        results[dag.status] = result; // hashing result for this dag
        return result;
    }
}

// count cdags given a forest
double cforest(const DAG& dag) {
    vector<int> roots = dag.roots();
    double count = 1;
    for (auto const& root : roots) {
        count *= ctree(dag, root);
    }
    return count;
}

// count cdags given a tree
double ctree(const DAG& dag, int r) {
    if (dag.children[r].empty()) {
        return dag.counts[r];
    } else {
        double count = 1;
        for (auto const& child : dag.children[r]) {
            count *= ctree(dag, child);
        }
        return count + 1;
    }
}

std::vector<int> transitive_closure_plus(const std::vector<std::set<int>>& R, int u) {
    set<int> closure;
    closure.insert(u);
    set<int> pool = R[u];
    while (!pool.empty()) {
        set<int>::iterator it = pool.begin();
        int next = (*it);
        pool.erase(it);
        pool.insert(R[next].begin(), R[next].end());
        closure.insert(next);
    }
    return vector<int>(closure.begin(), closure.end());
}

void UF_union(std::vector<int>& parent, int src, int dst) {
    int src_root = UF_find(parent, src);
    int dst_root = UF_find(parent, dst);
    parent[parent[dst_root]] = src_root;
}

int UF_find(std::vector<int>& parent, int index) {
    int root = index;
    while (parent[root] != root) {
        root = parent[root];
    }
    int i = index;
    while (parent[i] != i) {
        i = parent[i];
        parent[i] = root;
    }
    return root;
}
