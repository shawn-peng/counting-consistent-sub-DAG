
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

static const bool print_log = false;

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

	DAG *g = create_dag_from_file(datafile);
	g->generateRoots();
	g->getRootList(rootlist);

	//g->setSingleRoot(rootid);
	int ret;

	if (print_log)
	{
		g->print();
	}

	printf("Vertices: %d\n", g->getVertexNum());

	list<DAG> subdags;
	number_t check_num;

	
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	number_t num = estimate_upper_bound(g, 1);
	number_t num2 = estimate_upper_bound(g, 2);

	high_resolution_clock::time_point t2 = high_resolution_clock::now();


	//printf("Num of consistent sub-DAG: %.0f\n", num);
	cout << "(Estimated)Num of consistent sub-DAG: " << num << endl;
	cout << "(Estimated)Num of consistent sub-DAG: " << num2 << endl;

	graph_alg_clear_hash();

	auto duration = duration_cast<microseconds>(t2 - t1);
	//auto clean_duration = break_down_durations<chrono::minutes, chrono::seconds>(duration);
	//std::cout << std::get<0>(clean_duration).count() << "mins" << std::get<1>(clean_duration).count() << "secs" << endl;
	auto clean_duration = break_down_durations<chrono::milliseconds>(duration);
	//std::cout << "time: " << std::get<0>(clean_duration).count() << "secs" << endl;
	std::cout << "time: " << std::get<0>(clean_duration).count() << "ms" << endl;
	//cout << format_durations(clean_duration);

	free_dag(g);
	return 0;
}


