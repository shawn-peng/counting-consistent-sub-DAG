//Shawn Peng
//This is the main algorithm source file. Detailed explanation please refer
//to the report.

#include "graph_alg.h"

#include "dag.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#include <iostream>
#include <algorithm>
#include <queue>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <memory>

#include <gmpxx.h>


using namespace std;
using namespace NS_DAG;

static bool print_log = false;

static bool debugging = false;

static bool verify_hash = false;

static bool verify_alg = false;

static bool debug_flow = false;

static bool g_using_hash = false;

static bool using_pruning = false;

static string pivot_method = "random_mpn";

static bool allow_reverse = false;



static int recursion_depth = 0;



typedef mpz_class number_t;
//typedef double number_t;

//used for DAG indepency decomposition, this structure is for node
struct PathInfo
{
	int depth;
	int depthParent;
	map<int, IdList> pathMap; //map<MPnode, children of the roots on path to MPnode>
	PathInfo() : depth(0), depthParent(0) {}
};


typedef unordered_map<int, int> ParentNumMap;
typedef unordered_map<int, IdList> ParentMap;

struct ParentInfo
{
	ParentNumMap parentNumMap;
	ParentMap parentMap;
};

struct PathNode
{
	int id;
	int depth;
	//Vertex *v;

	PathNode() : id(-1), depth(-1) {}
	PathNode(int id, int depth) : id(id), depth(depth) {}
	
	bool operator <(const PathNode &rhs) const
	{
		if (depth < rhs.depth)
		{
			return true;
		}
		else if (depth == rhs.depth)
		{
			return id < rhs.id;
		}
		else
		{
			return false;
		}
	}
};

typedef mpq_class Rational;
typedef Rational Flow_t;
struct Flows
{
	Flow_t down_flow;
	Flow_t up_flow;
	Flow_t each_down_flow;
	Flow_t each_up_flow;
	Flows() : down_flow(0.0), up_flow(0.0),
		each_down_flow(0.0), each_up_flow(0.0){}
};


void graph_alg_enable_hashing()
{
	g_using_hash = true;
}
void graph_alg_disable_hashing()
{
	g_using_hash = false;
}

void graph_alg_enable_logging()
{
	print_log = true;
}
void graph_alg_disable_logging()
{
	print_log = false;
}

void graph_alg_enable_pruning()
{
	using_pruning = true;
}
void graph_alg_disable_pruning()
{
	using_pruning = false;
}

void graph_alg_enable_reverse()
{
	allow_reverse = true;
}
void graph_alg_disable_reverse()
{
	allow_reverse = false;
}

static unordered_map<string, number_t> hash_table;
static int hash_tries = 0;
static int hash_hits = 0;

static map<string, int> func_calls;



void print_subdag_list(const list<DAG> &glist)
{
	FOR_EACH_IN_CONTAINER(lsit, glist)
	{
		printf("----\n");
		lsit->print();
	}
}

void print_subdag_list(const list<DAG> &glist, ConstPrivDataFn fn)
{
	FOR_EACH_IN_CONTAINER(lsit, glist)
	{
		printf("----\n");
		lsit->print(fn);
	}
}

void print_id_list(const IdList &list)
{
	FOR_EACH_IN_CONTAINER(iter, list)
	{
		printf("%07d,", *iter);
	}
	return;
}

int print_privdata_num(const PrivDataUnion *data);
int print_privdata(const PrivDataUnion *data)
{
	print_privdata_num(data);
}

int print_privdata_int(const PrivDataUnion *data)
{
	return printf("%d", data->dint);
}

int print_privdata_num(const PrivDataUnion *data)
{
	void *p = data->dptr.get();
	number_t num;
	if (p)
	{
		num = *(number_t *)(p);
	}
	else
	{
		num = 0;
	}
	stringstream ss;
	ss << num;
	printf("%s", ss.str().c_str());
	return ss.str().size();
}

int print_privdata_flow(const PrivDataUnion *data)
{
	void *p = data->dptr.get();
	Flows flows;
	if (p)
	{
		flows = *(Flows *)(p);
	}
	
	stringstream ss;
	ss << "down:" << flows.down_flow << ", up:" << flows.up_flow;
	return printf("%s", ss.str().c_str());
	//return printf("down:%f, up:%f", flows.down_flow, flows.up_flow);
}

int print_privdata_ptr(const PrivDataUnion *data)
{
	return printf("%p", data->dptr.get());
}

int print_privdata_pathinfo(const PrivDataUnion *data)
{
	//shared_ptr<PathInfo> pinfo = static_pointer_cast<PathInfo>(data->dptr);
	shared_ptr<PathInfo> pinfo = static_pointer_cast<PathInfo>(data->dptr);
	map<int, IdList> &pmap = pinfo->pathMap;
	printf("{\n");
	FOR_EACH_IN_CONTAINER(iter, pmap)
	{
		printf("\t%07d: [\n\t\t", iter->first);
		print_id_list(iter->second);
		printf("\n");
		printf("\t]\n");
	}
	printf("}\n");
	//FOR_EACH_IN_CONTAINER(iter, pmap)
	return 0;
}


number_t &get_num_from_priv_data(PrivDataUnion &data)
{
	shared_ptr<void> &p = data.dptr;
	if (!p)
	{
		p = make_shared<number_t>(0);
		data.type = DT_POINTER;
	}
	void *vp = p.get();
	return *(number_t *)vp;
}

// will alloc a new object
void set_num_to_priv_data(PrivDataUnion &data, const number_t &num)
{
	//void *p = data.dptr;
	//if (p)
	//{
	//	delete p;
	//}
	//p = new number_t(num);
	//shared_ptr<number_t> &p = static_pointer_cast<number_t>(data.dptr);
	//if (!p)
	//{
	//	p = make_shared<number_t>(0);
	//	data.dptr = p;
	//}
	//*p = num;
	shared_ptr<void> &p = data.dptr;
	p = make_shared<number_t>(num);
	shared_ptr<number_t> np = static_pointer_cast<number_t>(p);
	data.type = DT_POINTER;
	//*np = num;
	return;
}

void updateNumInPrivData(PrivDataUnion &data, const number_t &num)
{
	shared_ptr<void> &p = data.dptr;
	if (!p)
	{
		p = make_shared<number_t>(0);
		data.type = DT_POINTER;
	}
	shared_ptr<number_t> np = static_pointer_cast<number_t>(p);
	*np = num;
}

template <typename T>
int setPtrToPrivData(PrivDataUnion &data, T *p)
{
	assert(data.type == DT_EMPTY || data.type == DT_POINTER);
	data.dptr = shared_ptr<void>(p);
	data.type = DT_POINTER;
	return 0;
}

void set_down_flow(DAG *g, int id, Flow_t flow)
{
	PrivDataUnion &data = g->getPrivData(id);
	shared_ptr<void> &p = data.dptr;
	if (!p)
	{
		// alloc a Flows structure
		p = make_shared<Flows>();
		data.type = DT_POINTER;
	}
	Flows &flows = *static_pointer_cast<Flows>(data.dptr);
	flows.down_flow = flow;
	return;
}

void set_up_flow(DAG *g, int id, Flow_t flow)
{
	PrivDataUnion &data = g->getPrivData(id);
	shared_ptr<void> &p = data.dptr;
	if (!p)
	{
		// alloc a Flows structure
		p = make_shared<Flows>();
		data.type = DT_POINTER;
	}
	Flows &flows = *static_pointer_cast<Flows>(data.dptr);
	flows.up_flow = flow;
	return;
}

Flow_t get_down_flow(DAG *g, int id)
{
	PrivDataUnion &data = g->getPrivData(id);
	shared_ptr<void> &p = data.dptr;
	assert(p); // we ensure that every flow has been set before we read it
	Flows &flows = *static_pointer_cast<Flows>(p);
	return flows.down_flow;
}

Flow_t get_up_flow(DAG *g, int id)
{
	PrivDataUnion &data = g->getPrivData(id);
	shared_ptr<void> &p = data.dptr;
	assert(p); // we ensure that every flow has been set before we read it
	Flows &flows = *static_pointer_cast<Flows>(p);
	return flows.up_flow;
}

void set_each_down_flow(DAG *g, int id, Flow_t flow)
{
	PrivDataUnion &data = g->getPrivData(id);
	shared_ptr<void> &p = data.dptr;
	if (!p)
	{
		// alloc a Flows structure
		p = make_shared<Flows>();
		data.type = DT_POINTER;
	}
	Flows &flows = *static_pointer_cast<Flows>(data.dptr);
	flows.each_down_flow = flow;
	return;
}

void set_each_up_flow(DAG *g, int id, Flow_t flow)
{
	PrivDataUnion &data = g->getPrivData(id);
	shared_ptr<void> &p = data.dptr;
	if (!p)
	{
		// alloc a Flows structure
		p = make_shared<Flows>();
		data.type = DT_POINTER;
	}
	Flows &flows = *static_pointer_cast<Flows>(data.dptr);
	flows.each_up_flow = flow;
	return;
}

Flow_t get_each_down_flow(DAG *g, int id)
{
	PrivDataUnion &data = g->getPrivData(id);
	shared_ptr<void> &p = data.dptr;
	assert(p); // we ensure that every flow has been set before we read it
	Flows &flows = *static_pointer_cast<Flows>(p);
	return flows.each_down_flow;
}

Flow_t get_each_up_flow(DAG *g, int id)
{
	PrivDataUnion &data = g->getPrivData(id);
	shared_ptr<void> &p = data.dptr;
	assert(p); // we ensure that every flow has been set before we read it
	Flows &flows = *static_pointer_cast<Flows>(p);
	return flows.each_up_flow;
}

Flow_t get_sum_flow(DAG *g, int id)
{
	PrivDataUnion &data = g->getPrivData(id);
	shared_ptr<void> &p = data.dptr;
	assert(p); // we ensure that every flow has been set before we read it
	Flows &flows = *static_pointer_cast<Flows>(p);
	return flows.down_flow + flows.up_flow;
}

//static map<int, int> parent_num_map;

//static map<int, IdList> parent_map; // only save id for multiple parents

ParentInfo &get_parent_info(DAG *g)
{
	ParentInfo *p = static_cast<ParentInfo *>(g->getPrivData().dptr.get());
	return *p;
}


int get_parents(DAG *g, int id, IdList &plist)
{
	ParentMap &parent_map = get_parent_info(g).parentMap;

	auto map_it = parent_map.find(id);
	if (map_it != parent_map.end())
	{
		plist.insert(plist.end(), map_it->second.begin(), map_it->second.end());
	}
	else
	{
		printf("Error in %s: parent map for id:%07d not found.\n", __FUNCTION__, id);
		assert(0);
		exit(1);
	}
	return 0;
}

//check if all direct parents are in cur_graph
bool check_parent(DAG *g, int id, ParentMap &parent_map)
{
	const IdList &parents = parent_map[id];
	FOR_EACH_IN_CONTAINER(pidit, parents)
	{
		Vertex *v = g->findVertex(*pidit);
		if (v == NULL)
		{
			// printf("parent %07d is not in the dag.\n", *pidit);
			return false;
		}
	}
	return true;
}

bool check_node(DAG *g, int id)
{
	Vertex *v = g->findVertex(id);
	if (v == NULL)
	{
		return false;
	}
	return true;
}


// get in-degree for all vertices(children), the parent's info is set outside
int gen_parent_num_map(DAG *g, int parent)
{
	ParentNumMap &parent_num_map = get_parent_info(g).parentNumMap;

	Vertex *v = g->findVertex(parent);
	if (v == NULL)
	{
		printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, parent);
		exit(-1);
	}

	IdList children;
	v->getChildList(children);

	FOR_EACH_IN_CONTAINER(iter, children)
	{
		if (parent_num_map.find(*iter) != parent_num_map.end())
		{
			// this vertex already expanded
			continue;
		}
		parent_num_map[*iter] = g->getParentNum(*iter);
		//printf("%07d:%d\n", *iter, parent_num_map[*iter]);
		gen_parent_num_map(g, *iter);
	}

	return 0;
}
int gen_parent_num_map(DAG *g, const IdList &parents)
{
	FOR_EACH_IN_CONTAINER(iter, parents)
	{
		gen_parent_num_map(g, *iter);
	}
	return 0;
}


int gen_parent_map(DAG *g, int parent)
{
	ParentMap &parent_map = get_parent_info(g).parentMap;

	Vertex *v = g->findVertex(parent);
	if (v == NULL)
	{
		printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, parent);
		g->print();
		exit(-1);
	}

	IdList children;
	v->getChildList(children);

	FOR_EACH_IN_CONTAINER(iter, children)
	{
		IdList &pl = parent_map[*iter];
		if (!pl.empty())
		{// already generated for this node and it's child
			continue;
		}
		g->getParentList(*iter, pl);
		gen_parent_map(g, *iter);
	}

	return 0;
}
int gen_parent_map(DAG *g, IdList parents)
{
	FOR_EACH_IN_CONTAINER(iter, parents)
	{
		gen_parent_map(g, *iter);
	}
	return 0;
}

PathInfo *get_path_info(const PrivDataUnion &privdata)
{
	assert(privdata.type == DT_POINTER);
	return (PathInfo *)privdata.dptr.get();
}

PathInfo *get_path_info(const Vertex *v)
{
	return get_path_info(v->getPrivData());
}

PathInfo *get_path_info(DAG *g, int id)
{
	return get_path_info(g->getPrivData(id));
}

void freePathInfo(void *ptr)
{
	if (ptr != NULL)
	{
		delete (PathInfo *)ptr;
	}
	return;
}

int free_nodes_pathinfo(DAG *g)
{
	IdList fringe;
	//int rootid = g->getFirstRoot();
	//fringe.push_back(rootid);
	g->getRootList(fringe);

	while (!fringe.empty())
	{
		int id = fringe.front();
		fringe.pop_front();

		PathInfo *pinfo;
		Vertex *v = g->findVertex(id);
		if (v == NULL)
		{
			assert(0);
			continue;
		}
		PrivDataUnion privdata = v->getPrivData();
		//pinfo = (PathInfo *)privdata.dptr;
		//if (pinfo == NULL)
		//{
		//	continue;
		//}
		//delete pinfo;
		//privdata.dptr = NULL;
		privdata.dptr.reset();
		privdata.type = DT_EMPTY;
		v->setPrivData(privdata);

		IdList children;
		v->getChildList(children);
		//Use preorder
		FOR_EACH_IN_CONTAINER_REVERSE(chiter, children)
		{
			fringe.push_front(*chiter);
		}
	}
}


// returned mpnodes are sorted
int get_mpnodes_under_node(DAG *g, int id, IdList &mpnodes)
{
	//PrivDataUnion privdata = g->getPrivData(id);
	PathInfo *pinfo = get_path_info(g, id);
	map<int, IdList> &pathmap = pinfo->pathMap;
	FOR_EACH_IN_CONTAINER(iter, pathmap)
	{
		if (g->findVertex(iter->first) == NULL)
		{
			continue;
		}
		mpnodes.push_back(iter->first);
	}
	return 0;
}

int get_mpnodes_under_nodes(DAG *g, IdList nodes, IdList &mpnodes)
{
	FOR_EACH_IN_CONTAINER(nodeit, nodes)
	{
		IdList newnodes;
		get_mpnodes_under_node(g, *nodeit, newnodes);
		// merge two sorted lists
		mpnodes.merge(newnodes);
	}
	return 0;
}

int get_mpnodes(DAG *g, IdList &mpnodes)
{
	ParentNumMap &parent_num_map = get_parent_info(g).parentNumMap;
	FOR_EACH_IN_CONTAINER(iter, parent_num_map)
	{
		if (iter->second > 1)
		{
			mpnodes.push_back(iter->first);
		}
	}
	return 0;
}

int get_signature(DAG *g, string &str)
{
	//ostringstream oss;
	//IdList roots;
	//g->getRootList(roots);
	//FOR_EACH_IN_CONTAINER(iter, roots)
	//{
	//	oss << *iter << ' ';
	//}
	//IdList mpnodes;
	//get_mpnodes(g, mpnodes);
	//FOR_EACH_IN_CONTAINER(iter, mpnodes)
	//{
	//	oss << *iter << ' ';
	//}
	//str = string(move(oss.str()));
	g->getVertexString(str);
	if (verify_alg)
	{
		if (g->isReversed())
		{
			str = "D " + str;
		}
		else
		{
			str = "U " + str;
		}
	}

	return 0;
}


int get_descendants_subdag(DAG *g, int id, DAG &subdag)
{
	if (subdag.getVertexNum() == 0)
	{
		subdag.addToRootList(id);
	}
	Vertex *v = subdag.findVertex(id);
	if (v != NULL)
	{
		return 0;
	}
	subdag.addVertex(id);

	IdList children;
	g->getChildList(id, children);

	// add parent(s) to path
	FOR_EACH_IN_CONTAINER(iter, children)
	{
		int cid = *iter;
		get_descendants_subdag(g, cid, subdag);
		subdag.addEdge(id, cid);
	}

	return 0;
}

int get_ancestors_subdag(DAG *g, int id, DAG &subdag)
{
	if (g->isRoot(id))
	{
		subdag.addToRootList(id);
	}
	Vertex *v = subdag.findVertex(id);
	if (v != NULL)
	{
		return 0;
	}
	subdag.addVertex(id);

	IdList parents;
	g->getParentList(id, parents);

	// add parent(s) to path
	FOR_EACH_IN_CONTAINER(iter, parents)
	{
		int pid = *iter;
		get_ancestors_subdag(g, pid, subdag);
		subdag.addEdge(pid, id);
	}

	return 0;
}

////D(u), get the smallest consistent sub-DAG for vertex u with ID(id)
//int get_ancestors_subdag(DAG *g, int id, DAG &subdag)
//{
//	// may need several path
//	//int rootid = g->getFirstRoot();
//
//	Vertex *v = subdag.findVertex(id);
//	subdag.addVertex(id);
//
//	IdList parents;
//	get_parents(g, id, parents);
//
//	IdList pathp;
//	if (v != NULL)
//	{
//		v->getParentList(pathp);
//	}
//
//	// add parent(s) to path
//	FOR_EACH_IN_CONTAINER(iter, parents)
//	{
//		int pid = *iter;
//		IdList::iterator ppit = find(pathp.begin(), pathp.end(), *iter);
//		if (ppit != pathp.end())
//		{
//			//this edge already in path
//			continue;
//		}
//		subdag.addVertex(pid);
//		subdag.addEdge(pid, id);
//	}
//
//	FOR_EACH_IN_CONTAINER(iter, parents)
//	{
//		int pid = *iter;
//		if (g->isRoot(pid))
//		{
//			subdag.addToRootList(pid);
//		}
//		else
//		{
//			get_ancestors_subdag(g, pid, subdag);
//		}
//	}
//	return 0;
//}

// Similar to above, but check if any vertex in 'exclude' will be added
// to the path, if that's the case, return 1
int get_path_to_root_with_exclusion(DAG *g, int id,
		const IdList &exclude, DAG &subdag)
{
	// may need several path
	//int rootid = g->getFirstRoot();

	Vertex *v = subdag.findVertex(id); // here we want to see if v exists already
	subdag.addVertex(id);

	// parents in DAG
	IdList parents;
	get_parents(g, id, parents);

	// parents in path
	IdList pathp;
	if (v != NULL)
	{
		v->getParentList(pathp);
	}

	FOR_EACH_IN_CONTAINER(iter, parents)
	{
		int pid = *iter;
		IdList::iterator ppit = find(pathp.begin(), pathp.end(), *iter);
		if (ppit != pathp.end())
		{
			//this edge already in path
			continue;
		}

		// check if this node must be excluded
		auto excit = find(exclude.begin(), exclude.end(), pid);
		if (excit != exclude.end())
		{
			// then we need to stop the process
			printf("node %07d is excluded from path.\n", pid);
			return 1;
		}

		subdag.addVertex(pid);
		subdag.addEdge(pid, id);
	}
	FOR_EACH_IN_CONTAINER(iter, parents)
	{
		int pid = *iter;
		if (g->isRoot(pid))
		{
			subdag.addToRootList(pid);
			continue;
		}
		int ret = get_path_to_root_with_exclusion(g, pid, exclude, subdag);
		if (ret != 0)
		{
			return 1;
		}
	}
	return 0;
}


int add_path_to_pathmap(DAG *g, int targetid, const DAG &pathdag)
{
	IdList fringe;

	//int rootid = g->getFirstRoot();
	//fringe.push_back(rootid);

	pathdag.getRootList(fringe);

	// breadth first traverse path dag
	while(!fringe.empty())
	{
		int id = fringe.front();
		fringe.pop_front();

		Vertex *pv = pathdag.findVertex(id);
		PrivDataUnion pvpriv = pv->getPrivData();
		if (pvpriv.dint != 0)
		{// already expanded
			continue;
		}

		//shared_ptr<PathInfo> pinfo = static_pointer_cast<PathInfo>(g->getPrivData(id).dptr); // path info is saved in g
		PathInfo *pinfo = (PathInfo *)(g->getPrivData(id).dptr.get()); // path info is saved in g
		assert(pinfo);
		map<int, IdList> &pathmap = pinfo->pathMap;

		IdList children;
		pathdag.getChildList(id, children); // children are from path

		//preorder traverse to reduce space complexity
		FOR_EACH_IN_CONTAINER(chiter, children)
		{
			pathmap[targetid].push_back(*chiter);
			fringe.push_back(*chiter);
		}

		pvpriv.dint = 1;
		pv->setPrivData(pvpriv);
	}
	return 0;
}


// depth, path for mpnodes
int gen_mpnode_pathinfo(DAG *g, IdList mpnodes)
{
	IdList fringe;
	//int rootid = g->getFirstRoot();
	//fringe.push_back(rootid);
	IdList roots;
	g->getRootList(roots);

	PathInfo *pinfo;

	FOR_EACH_IN_CONTAINER(rootit, roots)
	{
		int rootid = *rootit;
		pinfo = new PathInfo();
		pinfo->depth = 0;
		pinfo->depthParent = 0;

		PrivDataUnion privdata;
		privdata.dptr = shared_ptr<PathInfo>(pinfo);
		privdata.type = DT_POINTER;
		//setPtrToPrivData(privdata, pinfo);
		g->setPrivData(rootid, privdata);

		fringe.push_back(rootid);
	}

	// alloc path info structure, and calculate depths
	while (!fringe.empty())
	{
		int id = fringe.front();
		fringe.pop_front();

		Vertex *v = g->findVertex(id);
		assert(v);
		pinfo = (PathInfo *)v->getPrivData().dptr.get();
		assert(pinfo);
		int depth = pinfo->depth;

		IdList children;
		v->getChildList(children);

		//preorder traverse to reduce space complexity
		FOR_EACH_IN_CONTAINER_REVERSE(chiter, children)
		{
			Vertex *vch = g->findVertex(*chiter);
			assert(vch);

			PrivDataUnion chprivdata = vch->getPrivData();
			pinfo = (PathInfo *)chprivdata.dptr.get();
			if (!pinfo)
			{
				pinfo = new PathInfo();
				PrivDataUnion pd;
				pd.dptr = shared_ptr<void>(pinfo);
				pd.type = DT_POINTER;
				vch->setPrivData(pd);
			}
			// check for the shallowest path
			int cdepth = pinfo->depth;
			if (cdepth != 0 && depth+1 <= cdepth)
			//if (cdepth != 0 && cdepth <= depth+1)
			{
				continue;
			}
			pinfo->depth = depth + 1;
			fringe.push_front(*chiter);
		}
	}

	// for mpnodes, generate path map
	FOR_EACH_IN_CONTAINER(iter, mpnodes)
	{
		DAG pathdag;
		get_ancestors_subdag(g, *iter, pathdag);
		add_path_to_pathmap(g, *iter, pathdag);
	}

	//g->printVertexes(print_privdata_pathinfo);

	return 0;
}

// list approach is too slow
//int add_node_to_pathnode_queue(list<PathNode> &pl, PathNode x)
//{
//	//pl is sorted
//	//first by depth (decreasing) then by id (increasing)
//	FOR_EACH_IN_CONTAINER(iter, pl)
//	{
//		if (iter->depth > x.depth)
//		{
//			continue;
//		}
//		else if (iter->depth == x.depth)
//		{
//			if (iter->v->getId() < x.v->getId())
//			{
//				continue;
//			}
//			else if (iter->v->getId() == x.v->getId())
//			{
//				return 0;
//			}
//			else
//			{
//				pl.insert(iter, x);
//				return 0;
//			}
//		}
//		else
//		{
//			pl.insert(iter, x);
//			return 0;
//		}
//	}
//	pl.insert(pl.end(), x);
//	return 0;
//}

// Still DAG decomposition
/*int find_independent_subdag_root(DAG *g, int id)
{
	list<PathNode> paths_queue;

	PathNode node;

	ParentNumMap &parent_num_map = get_parent_info(g).parentNumMap;

	while (!paths_queue.empty() ||
			parent_num_map[id] > 1)
	{
		//VertexDepthInfo &entry = paths_queue.front();
		IdList parents;
		g->getParentList(id, parents);
		FOR_EACH_IN_CONTAINER(iter, parents)
		{
			//VertexDepthInfo node;
			node.v = g->findVertex(*iter);
			shared_ptr<PathInfo> pinfo = static_pointer_cast<PathInfo>(node.v->getPrivData().dptr);
			node.depth = pinfo->depth;
			add_node_to_pathnode_queue(paths_queue, node);
			//printf("push depth %d: %07d\n", node.depth, node.v->getId());
		}
		node = paths_queue.front();
		id = node.v->getId();
		paths_queue.pop_front();
		//printf("pop depth %d: %07d\n", node.depth, node.v->getId());
	}

	return id;
}*/

int get_topological_order(DAG *g, IdList &list)
{
	DAG modified(*g);
	
	// The following is equivalent to Kahn's algorithm.
	// Add roots to order list, then remove roots.
	// Keep doing this untill empty.
	// We do this because we cannot iterate edges directly
	// and remove them in a constant time.
	while(modified.getRootNum() > 0)
	{
		IdList fringe;
		modified.getRootList(fringe);

		FOR_EACH_IN_CONTAINER(iter, fringe)
		{
			modified.removeVertex(*iter);
		}

		list.splice(list.end(), fringe);
	}

	assert(modified.getVertexNum() == 0);
	return 0;
}

int perform_graph_flow(DAG *g)
{
	//IdList fringe;
	IdList nodes;

	// get topological order
	IdList topo;
	get_topological_order(g, topo);

	// down flow
	FOR_EACH_IN_CONTAINER(iter, topo)
	{
		int id = *iter;

		Flow_t sum_in_flow = 1.0;// 1.0 for self

		IdList parents;
		g->getParentList(id, parents);

		FOR_EACH_IN_CONTAINER(it, parents)
		{
			int pid = *it;
			Flow_t x = get_each_down_flow(g, pid);
			assert(x != 0);
			sum_in_flow += x;
		}

		set_down_flow(g, id, sum_in_flow);
		
		int n = g->getChildNum(id);
		Flow_t each_out_flow;
		if (n != 0)
		{
			each_out_flow = sum_in_flow / n;
			set_each_down_flow(g, id, each_out_flow);
		}
	}

	// up flow
	FOR_EACH_IN_CONTAINER_REVERSE(riter, topo)
	{
		int id = *riter;

		Flow_t sum_in_flow = 1.0; // 1.0 for self

		IdList children;
		g->getChildList(id, children);

		FOR_EACH_IN_CONTAINER(it, children)
		{
			int chid = *it;
			Flow_t x = get_each_up_flow(g, chid);
			assert(x != 0);
			sum_in_flow += x;
		}

		set_up_flow(g, id, sum_in_flow);
		
		int n = g->getParentNum(id);
		Flow_t each_out_flow;
		if (n != 0)
		{
			each_out_flow = sum_in_flow / n;
			set_each_up_flow(g, id, each_out_flow);
		}
	}

	return 0;
}

//int find_root_consistent_to_vertices(DAG *g, IdList nodes)
//{
//	// profiling
//	func_calls[__FUNCTION__]++;
//
//	//list<PathNode> paths_queue;
//	priority_queue<PathNode> paths_queue;
//
//	PathNode node;
//
//	FOR_EACH_IN_CONTAINER(iter, nodes)
//	{
//		Vertex *v = g->findVertex(*iter);
//		node.id = v->getId();
//
//		PathInfo *pinfo = get_path_info(v);
//		node.depth = pinfo->depth;
//
//		//add_node_to_pathnode_queue(paths_queue, node);
//		paths_queue.push(node);
//	}
//
//	node = paths_queue.top();
//	int id = node.id;
//	paths_queue.pop();
//
//	ParentNumMap &parent_num_map = get_parent_info(g).parentNumMap;
//
//	while (!paths_queue.empty() ||
//			parent_num_map[id] > 1)
//	{
//		if (parent_num_map[id] == 0)
//		{
//			// all nodes left in paths queue are root
//			id = -1;
//			break;
//		}
//		//VertexDepthInfo &entry = paths_queue.front();
//		IdList parents;
//		g->getParentList(id, parents);
//		FOR_EACH_IN_CONTAINER(iter, parents)
//		{
//			//VertexDepthInfo node;
//			//node.v = g->findVertex(*iter);
//			node.id = *iter;
//			//shared_ptr<PathInfo> pinfo = static_pointer_cast<PathInfo>(g->getPrivData().dptr);
//
//			Vertex *v = g->findVertex(*iter);
//			PathInfo *pinfo = get_path_info(v);
//
//			node.depth = pinfo->depth;
//			//add_node_to_pathnode_queue(paths_queue, node);
//			paths_queue.push(node);
//			//printf("push depth %d: %07d\n", node.depth, node.v->getId());
//		}
//		node = paths_queue.top();
//		id = node.id;
//		paths_queue.pop();
//		printf("pop depth %d: %07d\n", node.depth, node.id);
//	}
//
//	return id;
//}

int find_roots_consistent_to_vertices(DAG *g, IdList nodes, IdList &roots)
{
	// profiling
	func_calls[__FUNCTION__]++;

	//list<PathNode> paths_queue;
	set<PathNode> paths_queue;

	PathNode node;

	FOR_EACH_IN_CONTAINER(iter, nodes)
	{
		node.id = *iter;

		Vertex *v = g->findVertex(*iter);
		PathInfo *pinfo = get_path_info(v);
		node.depth = pinfo->depth;

		// node are inserted with sorting of depth
		//add_node_to_pathnode_queue(paths_queue, node);
		paths_queue.insert(node);
	}

	node = *paths_queue.rbegin();
	int id = node.id;
	paths_queue.erase(--paths_queue.end());

	ParentNumMap &parent_num_map = get_parent_info(g).parentNumMap;

	while (!paths_queue.empty() ||
			parent_num_map[id] > 1)
	{
		if (parent_num_map[id] == 0)
		{
			// all nodes left in paths queue are root
			//id = -1;
			break;
		}
		//VertexDepthInfo &entry = paths_queue.front();
		IdList parents;
		g->getParentList(id, parents);
		FOR_EACH_IN_CONTAINER(iter, parents)
		{
			node.id = *iter;
			//shared_ptr<PathInfo> pinfo = static_pointer_cast<PathInfo>(g->getPrivData().dptr);

			Vertex *v = g->findVertex(node.id);
			PathInfo *pinfo = get_path_info(v);

			node.depth = pinfo->depth;
			//add_node_to_pathnode_queue(paths_queue, node);
			paths_queue.insert(node);
			//printf("push depth %d: %07d\n", node.depth, node.v->getId());
		}
		node = *paths_queue.rbegin();
		id = node.id;
		//paths_queue.pop();
		paths_queue.erase(--paths_queue.end());
		//printf("pop depth %d: %07d\n", node.depth, node.id);
	}

	roots.push_front(id);
	//FOR_EACH_IN_CONTAINER(iter, paths_queue)
	while (!paths_queue.empty())
	{
		roots.push_front(paths_queue.rbegin()->id);
		paths_queue.erase(--paths_queue.end());
	}

	return roots.size();
}


// independent decomposition
int decompose_dag(DAG *g, const IdList &mpnodes, list<DAG> &subdags)
{
	// profiling
	func_calls[__FUNCTION__]++;

	//printf("trying decomposing dag: \n");
	//g->print();

	//int realroot = g->getFirstRoot();
	IdList realroots;
	g->getRootList(realroots);
	IdList removed_mpnodes;

	DAG pathinfo_dag(*g);
	pathinfo_dag.clearVertexPrivData();
	pathinfo_dag.setPrivData(g->getPrivData());

	gen_mpnode_pathinfo(&pathinfo_dag, mpnodes);

	FOR_EACH_IN_CONTAINER(iter, mpnodes)
	{
		int id = *iter;
		Vertex *v = pathinfo_dag.findVertex(id);
		if (v == NULL)
		{
			continue;
		}

		IdList roots;
		IdList newroots;

		IdList covered_mpnodes;

		//printf("decomposing for node %07d.\n", id);
		covered_mpnodes.push_back(id);

		newroots = covered_mpnodes;

		assert(newroots.size() == 1);

		while (roots != newroots)
		{
			roots = newroots;
			// for the first loop, there will be only one node
			//if (newroots.size() > 1)
			//{
			//	// if more than one ancestors are returned then all are roots
			//	break;
			//}

			newroots.clear();
			int ret = find_roots_consistent_to_vertices(
					&pathinfo_dag, covered_mpnodes, newroots);

			// get map of paths to all mpnodes, check consistency of
			// subdag with roots
			covered_mpnodes.clear();
			get_mpnodes_under_nodes(&pathinfo_dag, newroots, covered_mpnodes);
			//printf("mpnodes under nodes(");
			//print_id_list(newroots);
			//printf("):\n\t");
			//print_id_list(covered_mpnodes);
			//printf("\n");
		}

		// check if we can decompose
		// the only case that we can NOT decompose is that the independent
		// roots we found are the all roots in the current DAG
		// since all roots should be unique, we can just compare the size
		// of the root lists
		if (realroots.size() == roots.size())
		{
			continue;
		}

		DAG tempdag;
		pathinfo_dag.removeSubdagRootAt(newroots, tempdag);
		//<shared_ptr> auto free
		//free_nodes_pathinfo(&tempdag);
		//tempdag.clearVertexPrivData();

		subdags.push_back(DAG());
		g->removeSubdagRootAt(newroots, subdags.back());

		//// use the same parent info memory
		//subdags.back().setPrivData(g->getPrivData());
		//// recursively decompose the independent subdag, because there might
		//// be some mpnodes have smaller independent subdag
		//decompose_dag(&subdags.back(), covered_mpnodes, subdags);
	}

	//print_subdag_list(subdags);

	//<shared_ptr> auto free
	//free_nodes_pathinfo(&pathinfo_dag);

	return 0;
}



// sanity check method
typedef pair<int, int> Edge;
typedef list<Edge> EdgeList;
int enum_possibilities(DAG *g, DAG cur_subdag, list<Edge> fringe, IdList needed, list<DAG> &all_subdags)
{
	int ret;

	//print_edge_list(fringe);
	//cur_subdag.print();

	if (fringe.empty())
	{
		//cur_subdag.print();
		all_subdags.push_back(cur_subdag);
		return 0;
	}

	Edge edge = fringe.front();
	fringe.pop_front();

	int vid = edge.second;

	//not choosing this vertex

	//fringe and needed are copied for every call
	//needed list should be in constant len
	enum_possibilities(g, cur_subdag, fringe, needed, all_subdags);

	//or choosing

	//check multiple parents
	ParentInfo &parent_info = get_parent_info(g);
	int flag = 1;
	int np = parent_info.parentNumMap[vid];
	if (np > 1)
	{
		if (!check_parent(&cur_subdag, vid, parent_info.parentMap))
		{
			//this node can't be choosen
			//Because we checked before putting the node into fringe,
			//this shouldn't happen.
			printf("%07d's parent is missing.\n", vid);
			cur_subdag.print();
			assert(0);
			return 0;
		}
		if (check_node(&cur_subdag, vid))
		{
			//skip adding and expanding
			enum_possibilities(g, cur_subdag, fringe, needed, all_subdags);
			return 0;
		}
		//all parent appear, and this node is not in cur_subdag,
		//add edges from all parent to this node
		cur_subdag.addVertex(vid);
		IdList &parents = parent_info.parentMap[vid];
		FOR_EACH_IN_CONTAINER(pidit, parents)
		{
			cur_subdag.addEdge(*pidit, vid);
		}
	}
	else
	{
		if (!cur_subdag.checkVertex(vid))
		{
			cur_subdag.addVertex(vid);
		}
		//just add one Edge
		cur_subdag.addEdge(edge.first, vid);
	}

	//print_id_list(needed);

	//expand current node
	Vertex *v = g->findVertex(vid);
	assert(v);
	IdList childids;
	v->getChildList(childids);
	FOR_EACH_IN_CONTAINER_REVERSE(chidit, childids)
	{
		int chid = *chidit;
		int np = parent_info.parentNumMap[chid];
		if (np > 1)
		{
			//check if all direct parents are in cur_graph
			if (!check_parent(&cur_subdag, chid, parent_info.parentMap))
			{
				continue;
			}
		}
		fringe.push_front(Edge(vid, chid));
	}
	//fringe and needed are copied for every call
	enum_possibilities(g, cur_subdag, fringe, needed, all_subdags);

	return 0;
}
int enum_possibilities_count(DAG *g, DAG cur_subdag, EdgeList fringe, number_t &total_count)
{
	int ret;

	//print_edge_list(fringe);
	//cur_subdag.print();

	if (fringe.empty())
	{
		total_count++;
		return 0;
	}

	Edge edge = fringe.front();
	fringe.pop_front();

	int vid = edge.second;

	//not choosing this vertex

	//fringe are copied for every call
	enum_possibilities_count(g, cur_subdag, fringe, total_count);

	//or choosing

	//check multiple parents
	ParentInfo &parent_info = get_parent_info(g);
	int flag = 1;
	int np = parent_info.parentNumMap[vid];
	if (np > 1)
	{
		if (!check_parent(&cur_subdag, vid, parent_info.parentMap))
		{
			//this node can't be choosen
			//Because we checked before putting the node into fringe,
			//this shouldn't happen.
			printf("%07d's parent is missing.\n", vid);
			cur_subdag.print();
			assert(0);
			return 0;
		}
		if (check_node(&cur_subdag, vid))
		{
			//skip adding and expanding
			enum_possibilities_count(g, cur_subdag, fringe, total_count);
			return 0;
		}
		//all parent appear, and this node is not in cur_subdag,
		//add edges from all parent to this node
		cur_subdag.addVertex(vid);
		IdList &parents = parent_info.parentMap[vid];
		FOR_EACH_IN_CONTAINER(pidit, parents)
		{
			cur_subdag.addEdge(*pidit, vid);
		}
	}
	else
	{
		if (!cur_subdag.checkVertex(vid))
		{
			cur_subdag.addVertex(vid);
		}
		//just add one Edge
		cur_subdag.addEdge(edge.first, vid);
	}

	//expand current node
	Vertex *v = g->findVertex(vid);
	assert(v);
	IdList childids;
	v->getChildList(childids);
	FOR_EACH_IN_CONTAINER_REVERSE(chidit, childids)
	{
		int chid = *chidit;
		int np = parent_info.parentNumMap[chid];
		if (np > 1)
		{
			//check if all direct parents are in cur_graph
			if (!check_parent(&cur_subdag, chid, parent_info.parentMap))
			{
				continue;
			}
		}
		fringe.push_front(Edge(vid, chid));
	}
	//fringe are copied for every call
	enum_possibilities_count(g, cur_subdag, fringe, total_count);

	return 0;
}
bool check_consistent(DAG *g, const DAG &subdag)
{
	IdList nodes;
	subdag.getVertexList(nodes);
	FOR_EACH_IN_CONTAINER(iter, nodes)
	{
		IdList parents;
		g->getParentList(*iter, parents);
		FOR_EACH_IN_CONTAINER(pit, parents)
		{
			if (!subdag.checkVertex(*pit))
			{
				return false;
			}
		}
	}
	return true;
}
int enum_possibilities_count_toobrute(DAG *g, DAG cur_subdag, IdList fringe, number_t &total_count)
{
	int ret;

	if (fringe.empty())
	{
		if (check_consistent(g, cur_subdag))
		{
			total_count++;
		}
		return 0;
	}

	recursion_depth++;
	for (int di = 1; di < recursion_depth; di++)
	{
		printf("  ");
	}
	printf("[%d] %d MP nodes left.\n", recursion_depth, fringe.size());

	int node = fringe.front();
	fringe.pop_front();

	//not choosing this node
	enum_possibilities_count_toobrute(g, cur_subdag, fringe, total_count);

	//and choosing this node
	cur_subdag.addVertex(node);
	cur_subdag.addToRootList(node);

	enum_possibilities_count_toobrute(g, cur_subdag, fringe, total_count);

	recursion_depth--;
	return 0;
}
number_t get_consistent_subdag_number(DAG *g, int rootid)
{
	int ret;

	Vertex* vroot = g->findVertex(rootid);
	if (vroot == NULL)
	{
		return 0;
	}

	ParentInfo *parentInfo = new ParentInfo();
	PrivDataUnion privdata;
	privdata.dptr = shared_ptr<void>(parentInfo);
	privdata.type = DT_POINTER;
	g->setPrivData(privdata);

	ret = gen_parent_num_map(g, rootid);
	ret = gen_parent_map(g, rootid);

	DAG g0;
	g0.addVertex(rootid);
	g0.setSingleRoot(rootid);

	EdgeList fringe;

	IdList childids;
	vroot->getChildList(childids);
	FOR_EACH_IN_CONTAINER_REVERSE(chidit, childids)
	{
		if (parentInfo->parentNumMap[*chidit] > 1)
		{
			continue;
		}
		fringe.push_front(Edge(rootid,*chidit));
	}

	number_t total_count = 1;
	enum_possibilities_count(g, g0, fringe, total_count);

	return total_count;
}

//sanity check
int get_consistent_subdag(DAG *g, int rootid, list<DAG> &subdags)
{
	int ret;

	Vertex* vroot = g->findVertex(rootid);
	if (vroot == NULL)
	{
		return 0;
	}

	ParentInfo *parentInfo = new ParentInfo();
	PrivDataUnion privdata;
	privdata.dptr = shared_ptr<void>(parentInfo);
	privdata.type = DT_POINTER;
	g->setPrivData(privdata);

	ret = gen_parent_num_map(g, rootid);
	ret = gen_parent_map(g, rootid);

	DAG g0;
	g0.addVertex(rootid);
	g0.setSingleRoot(rootid);

	EdgeList fringe;

	IdList childids;
	vroot->getChildList(childids);
	FOR_EACH_IN_CONTAINER_REVERSE(chidit, childids)
	{
		if (parentInfo->parentNumMap[*chidit] > 1)
		{
			continue;
		}
		fringe.push_front(Edge(rootid,*chidit));
	}

	ret = enum_possibilities(g, g0, fringe, IdList(), subdags);

	//delete parentInfo;
	//privdata.dptr = NULL;
	privdata.dptr.reset();
	privdata.type = DT_EMPTY;
	g->setPrivData(privdata);

	return ret;
}

//find nodes with multi parents
int find_multi_parent_node(DAG *g, int rootid)
{
	IdList children;
	g->getChildList(rootid, children);

	FOR_EACH_IN_CONTAINER(iter, children)
	{
		find_multi_parent_node(g, rootid);
	}
	return 0;
}

// remove the subdag rooted with rootid into the list of subdags
int remove_subdag_to_become_trees(DAG *g, int rootid, list<DAG> &subdags)
{
	IdList fringe;

	//g->print();
	//printf("Removing subdag with root: %07d...\n", rootid);

	ParentNumMap &parent_num_map = get_parent_info(g).parentNumMap;

	g->getChildList(rootid, fringe);
	fringe.reverse();
	while (!fringe.empty()) // every recurssion has a seperate fringe
	{
		int id = fringe.front();
		fringe.pop_front();

		if (parent_num_map[id] > 1)
		{
			// we found some node with multi parents in this subdag
			// recurssively call this function to remove its subdag first
			Vertex *v = g->findVertex(id);
			if (v != NULL)
			{
				remove_subdag_to_become_trees(g, id, subdags);
			}
			// even if some node in fringe is removed,
			// we don't need to handle that.
			// 1. if it don't have multiple parents, it will be skiped
			// 2. if it has, we checked whether the node is still in graph
			// before remove, so there won't be a problem.
		}
		else
		{
			// put all children into fringe
			IdList children;
			g->getChildList(id, children);
			children.reverse();
			//print_id_list(fringe);
			//print_id_list(children);
			fringe.splice(fringe.begin(), children);
			//print_id_list(fringe);
		}
	}

	//All nodes with multi parents are removed, this should be a tree now
	subdags.push_back(DAG());
	g->removeSubdagRootAt(rootid, subdags.back());

	return 0;
}


//for tree case
number_t count_consistent_subdag_tree(DAG *g, int rootid)
{
	PrivDataUnion data = g->getPrivData(rootid);
	number_t total = get_num_from_priv_data(data);
	if (total == 0)
	{
		total = 2;
	}
	else if (total == 1)
	{
		assert(0);
	}
	else
	{
		assert(!g->isInternal(rootid));
	}

	number_t num = 1;
	IdList children;
	g->getChildList(rootid, children);
	FOR_EACH_IN_CONTAINER(iter, children)
	{
		num *= count_consistent_subdag_tree(g, *iter);
	}

	total += num - 1;

	return total;
}

//for forest case
number_t count_consistent_subdag_forest(DAG *g)
{
	number_t total = 1;
	IdList children;
	IdList roots;
	g->getRootList(roots);
	FOR_EACH_IN_CONTAINER(rootit, roots)
	{
		int rootid = *rootit;
		total *= count_consistent_subdag_tree(g, rootid);
	}

	return total;
}



void get_subproblems_splitted_by_vertex(DAG *g, int id, DAG &g1, DAG &g2)
{
	g1 = *g;
	g2 = *g;

	DAG ancestors, descendants;
	get_ancestors_subdag(&g1, id, ancestors);
	g1.removeSubdag(ancestors);
	get_descendants_subdag(&g2, id, descendants);
	g2.removeSubdag(descendants);

	return;
}
void get_subproblems_splitted_by_vertex(DAG *g, int id, std::pair<DAG, DAG> &subproblems)
{
	DAG &m1 = subproblems.first;
	DAG &m2 = subproblems.second;

	m1 = *g;
	m2 = *g;

	DAG ancestors, descendants;
	get_ancestors_subdag(&m1, id, ancestors);
	m1.removeSubdag(ancestors);
	get_descendants_subdag(&m2, id, descendants);
	m2.removeSubdag(descendants);

	return;
}

pair<int, int> analyze_subproblem_scales_MPVnum(DAG *g, int id, std::pair<DAG, DAG> &subproblems)
{
	DAG &m1 = subproblems.first;
	DAG &m2 = subproblems.second;

	get_subproblems_splitted_by_vertex(g, id, subproblems);

	//DAG m1(*g), m2(*g);
	//DAG ancestors, descendants;
	//get_ancestors_subdag(&m1, id, ancestors);
	//m1.removeSubdag(ancestors);
	//get_descendants_subdag(&m2, id, descendants);
	//m2.removeSubdag(descendants);

	int s1, s2;
	IdList mpnodesA, mpnodesD;
	m1.getMultiParentVertices(mpnodesA);
	m2.getMultiParentVertices(mpnodesD);

	s1 = mpnodesA.size();
	s2 = mpnodesD.size();

	return make_pair(s1, s2);
}

pair<int, int> analyze_subproblem_scales_Bound(DAG *g, int id, pair<DAG, DAG> &subprobs)
{
	DAG m1, m2;
	get_subproblems_splitted_by_vertex(g, id, m1, m2);

	//DAG m1(*g), m2(*g);
	//DAG ancestors, descendants;
	//get_ancestors_subdag(&m1, id, ancestors);
	//m1.removeSubdag(ancestors);
	//get_descendants_subdag(&m2, id, descendants);
	//m2.removeSubdag(descendants);

	int s1, s2;
	int e1, n1, r1;
	int e2, n2, r2;

	e1 = m1.getEdgeNum();
	n1 = m1.getVertexNum();
	r1 = m1.getRootNum();

	e2 = m2.getEdgeNum();
	n2 = m2.getVertexNum();
	r2 = m2.getRootNum();

	s1 = e1 + n1 + r1;
	s2 = e2 + n2 + r2;

	return make_pair(s1, s2);
}

pair<int, int> analyze_subproblem_scales_Bound_bidirectional(DAG *g, int id, pair<DAG, DAG> &subprobs)
{
	DAG &m1(subprobs.first), &m2(subprobs.second);
	get_subproblems_splitted_by_vertex(g, id, subprobs);

	int s1, s2;
	int e1, n1, r1;
	int e2, n2, r2;

	e1 = m1.getEdgeNum();
	n1 = m1.getVertexNum();
	r1 = m1.getRootNum();

	e2 = m2.getEdgeNum();
	n2 = m2.getVertexNum();
	r2 = m2.getRootNum();

	s1 = e1 + n1 + r1;
	s2 = e2 + n2 + r2;

	// get the scales for the reversed problems
	int rs1, rs2;
	DAG rm1(m1), rm2(m2);
	rm1.reverse();
	rm2.reverse();

	r1 = rm1.getRootNum();
	r2 = rm2.getRootNum();

	rs1 = e1 + n1 + r1;
	rs2 = e2 + n2 + r2;

	// check if reversed will be better
	if (s1 > rs1)
	{
		subprobs.first = rm1;
	}
	if (s2 > rs2)
	{
		subprobs.second = rm2;
	}

	return make_pair(min(s1,rs1), min(s2,rs2));
}


bool comp_scale_pairs_sum(std::pair<int, int> s1, std::pair<int, int> s2)
{
	return ((long long)s1.first + s1.second) < ((long long)s2.first + s2.second);
}

int pivot_by_Bound(DAG *g, std::pair<DAG, DAG> &best_sub_problems)
{
	int best_node = 0;

	IdList nodes;
	g->getVertexList(nodes);

	// find the best node to split count
	pair<int, int> min_scales = make_pair(INT_MAX, INT_MAX);
	//FOR_EACH_IN_CONTAINER(iter, mpnodes)
	FOR_EACH_IN_CONTAINER(iter, nodes)
	{
		int id = *iter;
		pair<DAG, DAG> temp_subs_problems;
		pair<int, int> scales =
			analyze_subproblem_scales_Bound(
					g, id, temp_subs_problems);
		if (scales.first < scales.second)
		{
			swap(scales.first, scales.second);
		}

		if (comp_scale_pairs_sum(scales, min_scales))
		{
			min_scales = scales;
			best_node = id;
			best_sub_problems = temp_subs_problems;
		}
	}

	return best_node;
}

int pivot_by_Bound_bidirectional(DAG *g, pair<DAG, DAG> &subprobs)
{
	int best_node = 0;
	pair<DAG, DAG> best_sub_problems;

	IdList nodes;
	g->getVertexList(nodes);

	pair<int, int> min_scales = make_pair(INT_MAX, INT_MAX);

	FOR_EACH_IN_CONTAINER(iter, nodes)
	{
		int vid = *iter;
		pair<DAG, DAG> temp_subs_problems;
		pair<int, int> scales =
			analyze_subproblem_scales_Bound_bidirectional(
					g, vid, temp_subs_problems);
		// put the larger scale at the front
		if (scales.first < scales.second)
		{
			swap(scales.first, scales.second);
		}

		if (comp_scale_pairs_sum(scales, min_scales))
		{
			min_scales = scales;
			best_node = vid;
			best_sub_problems = temp_subs_problems;
		}
	}

	return best_node;
}

int pivot_by_random(DAG *g, pair<DAG, DAG> &subprobs)
{
	int n = g->getVertexNum();
	int ind = (double)(rand()) / RAND_MAX * n;
	//printf("debug: random pivoting %d / %d.\n", ind, n);
	int best_node = 0;

	IdList nodes;
	g->getVertexList(nodes);

	FOR_EACH_IN_CONTAINER(iter, nodes)
	{
		if (ind <= 0)
		{
			best_node = *iter;
			break;
		}
		ind--;
	}
	get_subproblems_splitted_by_vertex(g, best_node, subprobs);

	return best_node;
}

int pivot_by_random_mpn(DAG *g, pair<DAG, DAG> &subprobs)
{
	IdList mpnodes;
	g->getMultiParentVertices(mpnodes);

	int n = mpnodes.size();
	int ind = (double)(rand()) / RAND_MAX * n;
	//printf("debug: random pivoting %d / %d.\n", ind, n);
	int best_node = 0;

	FOR_EACH_IN_CONTAINER(iter, mpnodes)
	{
		if (ind <= 0)
		{
			best_node = *iter;
			break;
		}
		ind--;
	}
	get_subproblems_splitted_by_vertex(g, best_node, subprobs);

	return best_node;
}

int pivot_by_vertex_degree(DAG *g, std::pair<DAG, DAG> &best_sub_problems)
{
	int best_node = 0;

	IdList nodes;
	g->getVertexList(nodes);

	int max_degree = 0;
	FOR_EACH_IN_CONTAINER(iter, nodes)
	{
		int id = *iter;
		int d = g->getDegree(id);
		if (d > max_degree)
		{
			max_degree = d;
			best_node = id;
		}
	}

	get_subproblems_splitted_by_vertex(g, best_node, best_sub_problems);

	return best_node;
}

int pivot_by_flow_bidirectional(DAG *g, pair<DAG, DAG> &subprobs)
{
	int best_node = 0;

	DAG gflow(*g);
	gflow.clearVertexPrivData();

	perform_graph_flow(&gflow);
	if (debug_flow)
	{
		printf("The flow in the graph:\n");
		gflow.print(print_privdata_flow);
	}

	// check flow result
	// the sum of all flow in roots or leaves should be equal to the number of nodes
	Flow_t sum_up, sum_down;
	IdList roots, leaves;
	gflow.getRootList(roots);
	gflow.getLeafList(leaves);

	int n = gflow.getVertexNum();
	FOR_EACH_IN_CONTAINER(iter, roots)
	{
		sum_up += get_up_flow(&gflow, *iter);
	}
	FOR_EACH_IN_CONTAINER(iter, leaves)
	{
		sum_down += get_down_flow(&gflow, *iter);
	}
	assert(sum_up == n);
	assert(sum_down == n);

	IdList nodes;
	g->getVertexList(nodes);

	Flow_t max_flow = 0;
	FOR_EACH_IN_CONTAINER(iter, nodes)
	{
		int id = *iter;

		Flow_t upflow = get_up_flow(&gflow, id);
		Flow_t downflow = get_down_flow(&gflow, id);
		Flow_t flow = max(upflow, downflow);
		if (flow > max_flow)
		{
			best_node = id;
			max_flow = flow;
		}
	}

	get_subproblems_splitted_by_vertex(g, best_node, subprobs);
	return best_node;
}

typedef int (*PivotFn)(DAG *, pair<DAG, DAG> &);
static unordered_map<string, PivotFn> pivot_methods =
{
	{"random", pivot_by_random},
	{"random_mpn", pivot_by_random_mpn},
	{"bound", pivot_by_Bound_bidirectional},
	{"degree", pivot_by_vertex_degree},
	{"flow", pivot_by_flow_bidirectional},
};
static PivotFn pivot_func = pivot_by_random;

void graph_alg_set_pivoting_method(const std::string &methodname)
{
	auto pos = pivot_methods.find(methodname);
 	if (pos == pivot_methods.end())
	{
		printf("Invalid pivoting method.\n");
		exit(-1);
	}
	pivot_method = methodname;
	pivot_func = pos->second;
}

// g(u), calculate the number of consistent sub-DAGs in a DAG g
// CAUTION: for the regenerated dag we can't use the hashed value
number_t count_consistent_subdag_for_independent_subdag(
		DAG *g, IdList pivotlist = IdList())
{
	// profiling
	func_calls[__FUNCTION__]++;

	if (debugging)
	{
		printf("counting for:\n");
		g->print(print_privdata);
	}

	string vs;
	get_signature(g, vs);
	if (g_using_hash && !verify_hash)
	{
		//printf("vertices: %s\n", vs.c_str());
		//printf("Hash count: %d\n", hash_table.size());
		auto pos = hash_table.find(vs);
		hash_tries++;
		if (pos != hash_table.end())
		{
			hash_hits++;
			//printf("Hash hit! (%d)\n", hash_hits);
			if (debugging)
			{
				cout << "Hashed count " << pos->second << endl;
			}
			return pos->second;
		}
	}

	//static int recursion_depth = 0;
	recursion_depth++;

	IdList roots;
	g->getRootList(roots);
	//if (roots.size() > 1)
	//{
	//	printf("the independent subdag have more than one root.\n");
	//}
	list<DAG> extend_subdags;

	number_t total;

	IdList mpnodes;
	g->getMultiParentVertices(mpnodes);

	if (mpnodes.size() == 0)
	{
		total = count_consistent_subdag_forest(g);
	}
	//else if (mpnodes.size() < 3)
	//{
	//	total = count_consistent_subdag_for_independent_subdag_nonrecursive(g, using_hash);
	//}
	else
	{
		if (print_log)
		{
			//if (recursion_depth <= 20)
			{
				for (int di = 1; di < recursion_depth; di++)
				{
					printf("  ");
				}
				printf("[%d] %d MP nodes left.\n",
						recursion_depth, mpnodes.size());
			}
		}

		pair<DAG, DAG> best_sub_problems;
		int id = -1;

		if (pivotlist.empty())
		{
			id = pivot_func(g, best_sub_problems);
		}
		else
		{
			while (!g->checkVertex(id))
			{
				id = pivotlist.front();
				pivotlist.pop_front();
			}
			get_subproblems_splitted_by_vertex(g, id, best_sub_problems);
		}
		//int id = pivot_by_vertex_degree(g, best_sub_problems);
		//int id = pivot_by_Bound(g, best_sub_problems);

		//printf("partitioning with MP node %d\n", id);
		//DAG ancestors, descendants;
		//get_ancestors_subdag(g, id, ancestors);
		//mA.removeSubdag(ancestors);
		//get_descendants_subdag(g, id, descendants);
		//mD.removeSubdag(descendants);

		PrivDataUnion privdata = g->getPrivData(id);
		number_t subdag_count = get_num_from_priv_data(privdata);
		if (subdag_count == 0)
		{
			subdag_count = 2;
		}

		//IdList roots1, roots2;

		DAG &mA = best_sub_problems.first;
		//mA.getRootList(roots1);
		number_t num1 = count_consistent_subdag(&mA, pivotlist);
		//roots1.clear();

		DAG &mD = best_sub_problems.second;
		//mD.getRootList(roots2);
		number_t num2 = count_consistent_subdag(&mD, pivotlist);
		//roots2.clear();

		if (verify_alg)
		{
			DAG rmA(mA);
			rmA.reverse();
			//rmA.getRootList(roots1);
			number_t num1_c = count_consistent_subdag(&rmA, pivotlist);
			if (num1 != num1_c)
			{
				cout << "found a BUG:" << endl;
				cout << "This is the subdag:" << endl;
				g->print(print_privdata_num);
				cout << "pivoting node: " << id << endl;
				cout << "g[-A]:" << endl;
				mA.print(print_privdata_num);
				cout << "g[-A].reversed:" << endl;
				rmA.print(print_privdata_num);
				cout << "count(g[-A]) = " << num1 << " but" << endl
					<< "count(g[-A].reversed) = " << num1_c
					<< endl;

				assert(0);
				exit(2);
			}

			DAG rmD(mD);
			rmD.reverse();
			//rmD.getRootList(roots2);
			number_t num2_c = count_consistent_subdag(&rmD, pivotlist);
			if (num2 != num2_c)
			{
				cout << "found a BUG:" << endl;
				cout << "This is the subdag:" << endl;
				g->print(print_privdata_num);
				cout << "pivoting node: " << id << endl;
				cout << "g[-D]:" << endl;
				mD.print(print_privdata_num);
				cout << "g[-D].reversed:" << endl;
				rmD.print(print_privdata_num);
				cout << "count(g[-D]) = " << num2 << " but" << endl
					<< "count(g[-D].reversed) = " << num2_c
					<< endl;

				assert(0);
				exit(2);
			}
		}

		if (g->isRoot(id))
		{
			total = num2 * (subdag_count - 1) + num1;
		}
		else if (g->isLeaf(id))
		{
			total = num1 * (subdag_count - 1) + num2;
		}
		else
		{
			total = num1 + num2;
		}
	}

	if (g_using_hash)
	{
		if (!verify_hash)
		{
			hash_table[vs] = total;
		}
		else
		{
			auto pos = hash_table.find(vs);
			hash_tries++;
			if (pos != hash_table.end())
			{
				// check with hashed count
				hash_hits++;
				if (debugging)
				{
					//printf("Hash hit! (%d)\n", hash_hits);
				}
				if (pos->second != total)
				{
					cout << "Error." << endl;
					cout << "Hashed count " << pos->second
						<< " compared to result count " << total << endl;
					assert(0);
				}
			}
			else
			{
				hash_table[vs] = total;
			}
		}
		//printf("hash count: %d (%f)\n",
		//		hash_table.size(), (number_t)hash_hits/hash_tries);
	}

	if (debugging)
	{
		cout << "the count for DAG:" << endl;
		g->print(print_privdata);
		cout << "is: " << total << endl;
	}

	recursion_depth--;
	return total;
}

DAG *find_subdag_has(int id, list<DAG> &subdags)
{
	FOR_EACH_IN_CONTAINER(iter, subdags)
	{
		if (iter->checkVertex(id))
		{
			return &*iter;
		}
	}
	return NULL;
}

// pruning inplace
int prune_independent_subdags(DAG *g, IdList pivotlist)
{
	DAG &modified = *g;

	IdList mpnodes;
	//list of subdags that are removed from current graph
	list<DAG> decomp_subdags;

	get_mpnodes(&modified, mpnodes);

	decompose_dag(&modified, mpnodes, decomp_subdags);

	// topological ordering
	// Such that when a subdag is counted its parent subdag hasn't been
	// counted and we can add the counted subdag as a node into its parent
	// subdag to do the following counting.
	// if the parent of some independent subdag exists in another subdag
	// that was decomposed out, then the parent subdag must sitting behind
	// this subdag in the list. Because if it's the parent subdag was
	// decomposed first then it would contain the child subdag.
	// So we don't need to do anything
	//decomp_subdags.reverse();

	if (debugging)
	{
		if (decomp_subdags.size() == 0)
		{
			printf("not decomposed.\n");
		}
		else
		{
			printf("decomposed subdags.\n");
			modified.print();
			print_subdag_list(decomp_subdags);
		}
	}

	ParentMap &parent_map = get_parent_info(&modified).parentMap;


	//count_consistent_subdag_for_independent_subdag(g);
	// count for each
	FOR_EACH_IN_CONTAINER(iter, decomp_subdags)
	{
		DAG *subdag = &*iter;
		//free_nodes_pathinfo(subdag);

		//subdag->printEdges();

		IdList subroots;
		subdag->getRootList(subroots);

		//number_t num = count_consistent_subdag_for_independent_subdag(subdag);
		number_t num = count_consistent_subdag(subdag, pivotlist);
		//printf("count is %f\n", num);

		// generate subkey
		string subkey;
		subdag->getVertexString(subkey);

		// add to original dag

		// we can use only one root to represent the subdag
		// even when there are actually multipule roots in the subdag
		int srid = subroots.front();

		// ****BUG****: didn't remove deprecated code, this operation is already done
		// in later process
		//modified.addVertex(srid);

		PrivDataUnion priv;
		//priv.ddouble = num;
		set_num_to_priv_data(priv, num);

		// we should check if there is a parent for this root of subdag
		//if (subroots.size() == 1 && !g->isRoot(srid))
		if (subroots.size() == 1 && parent_map.find(srid) != parent_map.end())
		{
			// if there is any parent, there should be only one
			// because that's the way we define independent subdag
			int parent;
			parent = parent_map[srid].front();
			// find where the parent is
			DAG *parent_subdag = find_subdag_has(parent, decomp_subdags);

			// just add the root node, so that when laterly calculate
			// with count_consistent_subdag_for_independent_subdag
			// function, the mpnodes from this subdag won't be considered.

			//if (modified.checkVertex(parent))
			if (parent_subdag == NULL)
			{// it is in the root subdag
				assert(!modified.checkVertex(srid));
				modified.addVertex(srid);
				modified.addEdge(parent, srid);
				modified.setPrivData(srid, priv);
				modified.setSubkey(srid, subkey);
			}
			else
			{// in some later subdag, which will be counted and added later
				assert(!parent_subdag->checkVertex(srid));
				parent_subdag->addVertex(srid);
				parent_subdag->addEdge(parent, srid);
				parent_subdag->setPrivData(srid, priv);
				parent_subdag->setSubkey(srid, subkey);
			}
		}
		else
		{
			// otherwise, add as an independent root
			assert(!modified.checkVertex(srid));
			modified.addVertex(srid);
			modified.addToRootList(srid);
			modified.setPrivData(srid, priv);
			modified.setSubkey(srid, subkey);
		}
	}

	if (debugging)
	{
		if (decomp_subdags.size() > 0)
		{
			printf("After pruning.\n");
			modified.print(print_privdata);
			// because we substituted a subdag to a vertex, now we can't only use the
			// nodes of the regenerated dag as the hash key to store the count.
			//using_hash = false;
			// Now this is solved by using the subkey in Vertex
			// whenever we prune a subdag to a single node, we save the key of that
			// subdag into the "subkey" member variable in the Vertex structure
		}
	}

	return 0;
}


//g(u), similar to previous one, but can take multiple roots
number_t count_consistent_subdag(DAG *g, IdList pivotlist)
{
	// profiling
	func_calls[__FUNCTION__]++;

	if (debugging)
	{
		printf("pruning and counting:\n");
		g->print(print_privdata);
	}

	DAG modified;

	if (allow_reverse)
	{
		// check scale on both direction
		int nmp = g->getMPNodeNum();
		int nmc = g->getMCNodeNum();

		if (nmp > nmc)
		{// we should count the reversed dag
			modified = *g;
			modified.reverse();
			g = &modified;
			if (debugging)
			{
				printf("counting reverse\n");
			}
		}
	}

	string vs;
	get_signature(g, vs);
	//printf("vertices: %s\n", vs.c_str());
	//printf("Hash count: %d\n", hash_table.size());
	if (g_using_hash && !verify_hash)
	{
		//printf("vertices: %s\n", vs.c_str());
		//printf("Hash count: %d\n", hash_table.size());
		auto pos = hash_table.find(vs);
		hash_tries++;
		if (pos != hash_table.end())
		{
			hash_hits++;
			//printf("Hash hit! (%d)\n", hash_hits);
			if (debugging)
			{
				cout << "Hashed count " << pos->second << endl;
			}
			return pos->second;
		}
	}

	if (using_pruning)
	{
		modified = *g;
		IdList rootlist;
		modified.getRootList(rootlist);

		ParentInfo *parentInfo = new ParentInfo();
		PrivDataUnion privdata;
		privdata.dptr = shared_ptr<void>(parentInfo);
		privdata.type = DT_POINTER;
		modified.setPrivData(privdata);

		gen_parent_num_map(&modified, rootlist);
		gen_parent_map(&modified, rootlist);

		prune_independent_subdags(&modified, pivotlist);

		// pointing to the new DAG
		g = &modified;
	}


	// get combination
	number_t total = count_consistent_subdag_for_independent_subdag(g, pivotlist);
	//printf("the total is %f\n", total);
	//cout << "the total is " << total << endl;
	//modified.print(print_privdata);


	// parentInfo;
	//privdata.dptr = NULL;
	//privdata.dptr.reset();
	//privdata.type = DT_EMPTY;
	//modified.setPrivData(privdata);

	if (g_using_hash)
	{
		if (!verify_hash)
		{
			hash_table[vs] = total;
		}
		else
		{
			auto pos = hash_table.find(vs);
			hash_tries++;
			if (pos != hash_table.end())
			{
				// check with hashed count
				hash_hits++;
				if (debugging)
				{
					//printf("Hash hit! (%d)\n", hash_hits);
				}
				if (pos->second != total)
				{
					cout << "Error." << endl;
					cout << "Hashed count " << pos->second
						<< " compared to result count " << total << endl;
					assert(0);
				}
			}
			else
			{
				hash_table[vs] = total;
			}
		}
		//printf("hash count: %d (%f)\n",
		//		hash_table.size(), (number_t)hash_hits/hash_tries);
	}
	//printf("hash count: %d (%f)\n",
	//		hash_table.size(), (number_t)hash_hits/hash_tries);

	return total;
}

/**
 * Find all path to all MP nodes, substitude all trees left with their roots
 */
int reduce_dag(DAG *g)
{
	DAG modified(*g);
	DAG pathinfo_dag(*g);
	pathinfo_dag.clearVertexPrivData();

	PrivDataUnion priv = g->getPrivData();
	if (priv.dptr == NULL)
	{
		priv.dptr = shared_ptr<void>(new ParentInfo());
		priv.type = DT_POINTER;
	}
	pathinfo_dag.setPrivData(priv);

	IdList roots;
	pathinfo_dag.getRootList(roots);

	FOR_EACH_IN_CONTAINER(rootit, roots)
	{
		gen_parent_num_map(&pathinfo_dag, *rootit);
		gen_parent_map(&pathinfo_dag, *rootit);
	}

	IdList mpnodes;
	get_mpnodes(&pathinfo_dag, mpnodes);
	gen_mpnode_pathinfo(&pathinfo_dag, mpnodes);

	DAG pathdag;
	FOR_EACH_IN_CONTAINER(nodeit, mpnodes)
	{
		get_ancestors_subdag(&pathinfo_dag, *nodeit, pathdag);
	}

	// remove this path, then what's left should be all trees
	modified.removeSubdag(pathdag);
	
	count_consistent_subdag_forest(&modified);

	roots.clear();
	modified.getRootList(roots);
	FOR_EACH_IN_CONTAINER(rootit, roots)
	{
		// remove sub tree in original dag
		int srid = *rootit;
		//g->removeSubdagRootAt(srid);
		Vertex *v = g->findVertex(srid);
		// remove children
		IdList children;
		v->getChildList(children);
		FOR_EACH_IN_CONTAINER(chit, children)
		{
			DAG tempdag;
			g->removeSubdagRootAt(*chit, tempdag);
		}
		// save count
		v->setPrivData(modified.getPrivData(srid));
	}

	//modified.print(print_privdata);
	//g->print(print_privdata);
	//exit(0);
	//free_nodes_pathinfo(&pathinfo_dag);

	//delete (ParentInfo *)priv.dptr;
	//priv.dptr = NULL;
	//priv.type = DT_EMPTY;
	//pathinfo_dag.setPrivData(priv);

	return 0;
}


void free_dag(DAG *g)
{
	delete g;
}


void graph_alg_print_stats()
{
	printf("Hash tries: %d\nHash hits: %d\nHit rates: %f\n",
			hash_tries, hash_hits, (double)hash_hits/hash_tries);
	printf("Hash count: %d\n", hash_table.size());

	FOR_EACH_IN_CONTAINER(iter, func_calls)
	{
		printf("Function %s has been called %d times.\n", iter->first.c_str(), iter->second);
	}
}

void graph_alg_clear_hash()
{
	hash_table.clear();
	hash_tries = 0;
	hash_hits = 0;

	func_calls.clear();
}

number_t estimate_upper_bound(DAG *g, int method)
{
	DAG modified(*g);

	// generate depth map (max depth)
	unordered_map<int, int> depthmap;
	IdList nodes;
	get_topological_order(&modified, nodes);
	FOR_EACH_IN_CONTAINER(iter, nodes)
	{
		int id = *iter;
		int depth = 1;
		IdList parents;
		modified.getParentList(id, parents);
		// find deepest parent
		FOR_EACH_IN_CONTAINER(pit, parents)
		{
			int d = depthmap[*pit] + 1;
			if (d > depth)
			{
				depth = d;
			}
		}
		depthmap[id] = depth;
	}

	// remove edges to become tree
	IdList mpnodes;
	modified.getMultiParentVertices(mpnodes);
	FOR_EACH_IN_CONTAINER(iter, mpnodes)
	{
		int id = *iter;
		IdList parents;
		modified.getParentList(id, parents);
		if (method == 1)
		{
			parents.pop_back();
		}
		else if (method == 2)
		{
			// find deepest parent, and don't remove the edge from it
			FOR_EACH_IN_CONTAINER(pit, parents)
			{
				if (depthmap[id] == depthmap[*pit] + 1)
				{
					parents.erase(pit);
					// this will keep the edge from removing
					break;
				}
			}
		}
		FOR_EACH_IN_CONTAINER(pit, parents)
		{
			modified.removeEdge(*pit, id);
		}
	}

	IdList roots;
	modified.getRootList(roots);
	return count_consistent_subdag(&modified);
}







