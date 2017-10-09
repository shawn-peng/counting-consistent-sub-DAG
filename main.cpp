
#include "graph_alg.h"

#include "dag.h"
#include "common.h"
#include "dag_generator.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ratio>
#include <tuple>

using namespace std;
using namespace std::chrono;
using namespace NS_DAG;

static const bool verify_count = false;

template<class DurationIn>
tuple<> break_down_durations(DurationIn d) {
	return tuple<>();
}

template<class DurationOut, class... Durations, class DurationIn>
tuple<DurationOut, Durations ...> break_down_durations(DurationIn d) {
	tuple<DurationOut, Durations ...> retval;
	auto x = std::chrono::duration_cast<DurationOut>(d);
	d -= std::chrono::duration_cast<DurationIn>(x);
	return tuple_cat(make_tuple(x), break_down_durations<Durations ...>(d));
}

/*template<class... Durations>
tuple<decltype(Durations::period) ...> get_ratios()
{
	//return tuple<Durations::ratio;
	return tuple<decltype(Durations::period) ...>();
}

template<class Ratio>
int log_10_round_up(Ratio r)
{
	return 2;
}

template<class... Durations>
std::string format_durations( std::tuple<Durations...> d ) {
	std::size_t values[]={(std::size_t)std::get<Durations>(d).count()...};
	auto ratios = get_ratios<Durations ...>();

	std::stringstream ss;
	ss << std::setfill('0');
	ss << values[0];

	for (std::size_t const& v:values) {
		std::size_t i = &v-values;
		if (i==0) continue;
		ss << "::" << std::setw( log_10_round_up(ratios[i-1]) ) << values[i];
	}
	return ss.str();
}*/



int main(int argc, char *argv[])
{
	const char *datafile = "data/basic_graphs/mini-tree.txt";
	int rootid = 1;
	IdList rootlist;
	if (argc >= 2)
	{
		datafile = argv[1];
	}
	if (argc >= 3)
	{
		sscanf(argv[2], "%d", &rootid);
	}
	DAG *g = create_dag_from_file(datafile);
	g->generateRoots();
	g->getRootList(rootlist);

	//g->setSingleRoot(rootid);
	int ret;

	g->print();
	printf("Vertices: %d\n", g->getVertexNum());
	//g->print(988);
	list<DAG> subdags;
	number_t check_num;
	if (verify_count)
	{
		if (g->getVertexNum() <= 35)
		{
			if (rootlist.size() == 1)
			{
				// sanity check
				rootid = rootlist.front();
				ret = get_consistent_subdag(g, rootid, subdags);
				//print_subdag_list(subdags);
			}
			else
			{
				DAG new_g(*g);
				new_g.addVertex(0);
				FOR_EACH_IN_CONTAINER(iter, rootlist)
				{
					new_g.addEdge(0, *iter);
				}
				new_g.setSingleRoot(0);// set as the only root
				//rootid = rootlist.front();
				printf("DAG with virtual root:\n");
				new_g.print();
				ret = get_consistent_subdag(&new_g, 0, subdags);
				// remove the subdag with only the virtual root
				subdags.pop_front();
			}
			check_num = subdags.size() + 1;
		}
		else
		{
			DAG rg(*g);
			rg.reverse();
			IdList rroots;
			rg.getRootList(rroots);
			check_num = count_consistent_subdag(&rg, rroots);
			graph_alg_clear_hash();
		}
	}

	//reduce_dag(g);

	//g->printEdges();

	//return 0;
	
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	number_t num;
	//num = count_consistent_subdag_for_independent_subdag(g);
	//printf("Num of consistent sub-DAG: %.0f\n", num);
	//printf("====================================================");
	//printf("====================================================\n");
	num = count_consistent_subdag(g, rootlist);

	high_resolution_clock::time_point t2 = high_resolution_clock::now();


	//printf("Num of consistent sub-DAG: %.0f\n", num);
	cout << "Num of consistent sub-DAG: " << num << endl;

	//printf("(Sanity check)Num of consistent sub-DAG: %d\n", subdags.size());
	cout << "(Sanity check)Num of consistent sub-DAG: " << check_num << endl;

	graph_alg_print_stats();
	graph_alg_clear_hash();

	auto duration = duration_cast<microseconds>(t2 - t1);
	//auto clean_duration = break_down_durations<chrono::minutes, chrono::seconds>(duration);
	//std::cout << std::get<0>(clean_duration).count() << "mins" << std::get<1>(clean_duration).count() << "secs" << endl;
	auto clean_duration = break_down_durations<chrono::seconds>(duration);
	std::cout << "time: " << std::get<0>(clean_duration).count() << "secs" << endl;
	//cout << format_durations(clean_duration);

	free_dag(g);
	return 0;
}


