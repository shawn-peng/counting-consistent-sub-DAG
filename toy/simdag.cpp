// This function is used to generate a set of simulated DAGs with in-degree of
// each term sampled from a fixed descrete distribution.
// 
// Usage:
// ./simdag <k> <n> <file> <odir>
//
// Input:
// 1. <k> Number of output ontologies
// 2. <n> The size of the DAG, i.e., the number of vertices
// 3. <file> A file specifies the in-degree distribution, which has the format:
//    <in-degree> <probability>
// 4. <odir> Output directory.
//
// Remarks:
// 1. If the sum of probability does not sum up to 1, they will be normalized
// and rescaled proportionally.
// 2. The generated ontology (DAG) always has a single root (in-degree = 0)

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

vector<pair<int,int>> generate_indegree(int n, const vector<double>& cmf) {
    vector<pair<int,int>> indegree(n);
    indegree[0] = make_pair(0, 0); // vertex 0 is always the root
    for (int i = 1; i < n; ++i) {
        double p = static_cast<double>(rand()) / RAND_MAX;
        int l = 0;
        while (p >= cmf[l]) {
            // note that p shall never be greater than or equal to the last
            // value of cmf. see "hack".
            ++l;
        }
        indegree[i] = make_pair(i, l);
    }
    return indegree;
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    // read the number of outputs
    int k = stoi(argv[1]);
    // read the number of vertices
    int n = stoi(argv[2]);
    // read file
    ifstream ifs(argv[3], ifstream::in);
    string buf;
    vector<double> histo(100);
    while (ifs.good()) {
        getline(ifs, buf);
        if (buf.empty()) continue;
        istringstream iss(buf);
        int d;
        double h;
        iss >> d >> h;
        if (d > histo.size()) {
            histo.resize(d);
        }
        histo[d] = h;
    }
    ifs.close();
    // normalize
    double psum = 0;
    int last = 0; // last index that is not zero
    for (auto i = 0; i < histo.size(); ++i) {
        if (histo[i] > 0) {
            last = i;
            psum += histo[i];
        }
    }
    histo.resize(last+1);
    vector<double> cmf(histo.size(), 0.0); // "cummulative mass function"
    // cmf[i] = "the prob. of having in-degree <= i"
    // note that cmf[0] = 0 for convencience, there is always exactly one vertex
    // with 0 in-degree.
    for (auto i = 1; i < cmf.size(); ++i) {
        cmf[i] = histo[i]/psum + cmf[i-1];
    }
    cmf.back() = 1.01; // hack: make it slightly larger than 1
    // read the output folder
    string odir(argv[4]);

    // generate and save
    int n_succ = 0;
    int n_fail = 0;
    while (n_succ < k) {
        vector<pair<int,int>> indegree = generate_indegree(n, cmf);
        string ofile = odir + "/" + to_string(n_succ) + ".csv";
        ofstream ofs(ofile, ofstream::out);
        ofs << "parent\tchild\n";

        int count = 0; // number of vertices added into the DAG
        vector<pair<int,int>> candidate;
        vector<int> picked;
        while (count < n) {
            // enpool all nodes with indegree <= count
            int got = 0;
            for (auto i = 0; i < indegree.size(); ++i) {
                if (indegree[i].second <= count) {
                    swap(indegree[i], indegree[got++]);
                }
            }
            candidate.insert(candidate.begin(), indegree.begin(), indegree.begin()+got);
            indegree.erase(indegree.begin(), indegree.begin()+got);
            // pick a candidate -> picked
            if (candidate.empty()) {
                break; // infeasible indegree distribution
            }
            int j = rand() % candidate.size();
            pair<int,int> curr = candidate[j];
            candidate.erase(candidate.begin()+j);
            random_shuffle(picked.begin(), picked.end());
            for (auto i = 0; i < curr.second; ++i) {
                ofs << picked[i] << "\t" << curr.first << "\n";
            }
            picked.push_back(curr.first);
            count += 1;
        }
        ofs.close();
        if (count == n) {
            n_succ += 1;
        } else {
            n_fail += 1;
            if (n_fail > 2*k) { // tolerance: maximum number of fails
                exit(1);
            }
        }
    }
    return 0;
}
