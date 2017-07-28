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

#include <iostream>
#include <algorithm>
#include <queue>
#include <map>
#include <unordered_map>
#include <sstream>
#include <memory>
#include <limits.h>

#include <gmpxx.h>


static const bool debugging = false;

static const bool verify_hash = false;

using namespace NS_DAG;

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
	int depth;
	Vertex *v;
};


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


number_t &getNumFromPrivData(PrivDataUnion &data)
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
void setNumToPrivData(PrivDataUnion &data, const number_t &num)
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
	return 0;
}


int get_descendents_subdag(DAG *g, int id, DAG &subdag)
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
		get_descendents_subdag(g, cid, subdag);
		subdag.addEdge(id, cid);
	}

	return 0;
}

int get_path_to_root(DAG *g, int id, DAG &subdag)
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
		get_path_to_root(g, pid, subdag);
		subdag.addEdge(pid, id);
	}

	return 0;
}

////D(u), get the smallest consistent sub-DAG for vertex u with ID(id)
//int get_path_to_root(DAG *g, int id, DAG &subdag)
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
//			get_path_to_root(g, pid, subdag);
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
			if (cdepth != 0 && cdepth <= depth+1)
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
		get_path_to_root(g, *iter, pathdag);
		add_path_to_pathmap(g, *iter, pathdag);
	}

	//g->printVertexes(print_privdata_pathinfo);

	return 0;
}


int add_node_to_pathnode_queue(list<PathNode> &pl, PathNode x)
{
	//pl is sorted
	//first by depth (decreasing) then by id (increasing)
	FOR_EACH_IN_CONTAINER(iter, pl)
	{
		if (iter->depth > x.depth)
		{
			continue;
		}
		else if (iter->depth == x.depth)
		{
			if (iter->v->getId() < x.v->getId())
			{
				continue;
			}
			else if (iter->v->getId() == x.v->getId())
			{
				return 0;
			}
			else
			{
				pl.insert(iter, x);
				return 0;
			}
		}
		else
		{
			pl.insert(iter, x);
			return 0;
		}
	}
	pl.insert(pl.end(), x);
	return 0;
}

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

int find_root_consistent_to_vertices(DAG *g, IdList nodes)
{
	// profiling
	func_calls[__FUNCTION__]++;

	list<PathNode> paths_queue;

	PathNode node;

	FOR_EACH_IN_CONTAINER(iter, nodes)
	{
		Vertex *v = g->findVertex(*iter);
		node.v = v;

		PathInfo *pinfo = get_path_info(v);
		node.depth = pinfo->depth;

		add_node_to_pathnode_queue(paths_queue, node);
	}

	node = paths_queue.front();
	int id = node.v->getId();
	paths_queue.pop_front();

	ParentNumMap &parent_num_map = get_parent_info(g).parentNumMap;

	while (!paths_queue.empty() ||
			parent_num_map[id] > 1)
	{
		if (parent_num_map[id] == 0)
		{
			// all nodes left in paths queue are root
			id = -1;
			break;
		}
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
}

int find_roots_consistent_to_vertices(DAG *g, IdList nodes, IdList &roots)
{
	// profiling
	func_calls[__FUNCTION__]++;

	list<PathNode> paths_queue;

	PathNode node;

	FOR_EACH_IN_CONTAINER(iter, nodes)
	{
		Vertex *v = g->findVertex(*iter);
		node.v = v;

		PathInfo *pinfo = get_path_info(v);
		node.depth = pinfo->depth;

		// node are inserted with sorting of depth
		add_node_to_pathnode_queue(paths_queue, node);
	}

	node = paths_queue.front();
	int id = node.v->getId();
	paths_queue.pop_front();

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

	roots.push_back(id);
	FOR_EACH_IN_CONTAINER(iter, paths_queue)
	{
		roots.push_back(iter->v->getId());
	}

	return roots.size();
}


// independent decomposition
int decompose_dag(DAG *g, const IdList &mpnodes, list<DAG> &subdags)
{
	// profiling
	func_calls[__FUNCTION__]++;

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
			//printf("independent root %07d for node %07d.\n", newrootid, id);

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
		cur_subdag.addVertex(vid);
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
	number_t total = getNumFromPrivData(data);
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

//for multi-root tree case
number_t count_consistent_subdag_tree(DAG *g)
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


void clear_nonleaf_counts(DAG *g, int id)
{
	IdList children;
	g->getChildList(id, children);
	if (children.size() != 0)
	{
		PrivDataUnion data = g->getPrivData(id);
		number_t &num = getNumFromPrivData(data);
		if (num == 0)
		{
			// already cleaned following another path
			return;
		}
		num = 0;
		//setNumToPrivData(data, num);
		g->setPrivData(id, data);
		FOR_EACH_IN_CONTAINER(chit, children)
		{
			clear_nonleaf_counts(g, *chit);
		}
	}
}
/**
 * @desc after cutting the fixed path, some MP nodes might become SP nodes,
 * and it hasn't be counted into its parent. So we need to update this part
 * of parents and ancestors.
 *
 * @param g, DAG after cut
 * @param old_mpnodes, those node have multiple parents before cut
 */
void update_info_after_cut(DAG *g, const IdList &old_mpnodes)
{
	ParentInfo &parent_info = get_parent_info(g);

	parent_info.parentNumMap.clear();
	parent_info.parentMap.clear();
	// actually not necessary

	IdList roots;
	g->getRootList(roots);

	FOR_EACH_IN_CONTAINER(rootit, roots)
	{
		int rootid = *rootit;
		gen_parent_num_map(g, rootid);
		gen_parent_map(g, rootid);

		// clear counts for all non-leaf nodes
		clear_nonleaf_counts(g, rootid);
	}

	//FOR_EACH_IN_CONTAINER(oldit, old_mpnodes)
	//{
	//	int id = *oldit;
	//	if (parent_info.parentNumMap[id] == 1)
	//	{
	//		// there is only one parent
	//		int pid = parent_info.parentMap[id].front();
	//		number_t num = g->getPrivData(id).ddouble;
	//		// old is 0 because it's MP node and it's not included in
	//		// parents' counts
	//		uppropagate_counts(g, pid, num, 0);
	//	}
	//}

	return;
}

void clear_ancestor_counts(DAG *g, int id)
{
	PrivDataUnion data = g->getPrivData(id);
	number_t num = getNumFromPrivData(data);
	if (num == 0)
	{
		return;
	}

	num = 0;
	setNumToPrivData(data, num);
	g->setPrivData(id, data);

	IdList parents;
	g->getParentList(id, parents);
	FOR_EACH_IN_CONTAINER(iter, parents)
	{
		clear_ancestor_counts(g, *iter);
	}
	return;
}


// g(DAG|fixed) calculate using the cutting fixed path method
number_t count_consistent_subdag_by_cutting_fixed_path(DAG *g, int fixed, const DAG &fixed_path, bool using_hash = true)
{
	// profiling
	func_calls[__FUNCTION__]++;

	// copy DAG
	DAG modified(*g);
//	modified.copyVertexPrivData(*g);
	//modified.clearVertexPrivData();
	//modified.print(print_privdata_ptr);

	// create parent info structure
	ParentInfo *parentInfo = new ParentInfo();
	PrivDataUnion privdata;
	privdata.dptr = shared_ptr<void>(parentInfo);
	privdata.type = DT_POINTER;
	modified.setPrivData(privdata);

	// cut fixed path
	modified.removeSubdag(fixed_path);

	// recalculate precalculated info (including parent info and counts)
	IdList old_mpnodes;
	get_mpnodes(g, old_mpnodes);
	update_info_after_cut(&modified, old_mpnodes);

	IdList rootlist;
	modified.getRootList(rootlist);

	number_t total;

	// Check there is something left
	if (rootlist.size() != 0)
	{
		//printf("DAG after fixed path cutted.\n");
		//modified.print(print_privdata);
		total = count_consistent_subdag(&modified, rootlist, using_hash);
	}
	else
	{
		// all nodes need to be fixed
		printf("Nothing left after cut, all nodes in the DAG needs to be fixed.\n");
		total = 1;
	}

	privdata.dptr.reset();
	privdata.type = DT_EMPTY;
	modified.setPrivData(privdata);

	return total;
}


number_t count_consistent_subdag_adding_subdag(DAG *g, IdList mpnodes, const DAG &subdag, bool using_hash = true)
{
	// profiling
	func_calls[__FUNCTION__]++;

	// the root of subdag must be choosen(fixed)

	//int rootid = g->getFirstRoot();

	// even for multi-root DAG case, the subdag still have only one root
	int srid = subdag.getFirstRoot();
	DAG pathdag;
	get_path_to_root(g, srid, pathdag);
	//pathdag.setSingleRoot(rootid);
	//printf("--------------------------------------------\n");
	//printf("Fixed path to root of node: %07d\n", srid);
	//pathdag.print();

	number_t total = 0;
	IdList indep_mpnodes;
	list<DAG> indepmp_pathcombs;

	// get all independent MP nodes in current dag
	FOR_EACH_IN_CONTAINER(mpniter, mpnodes)
	{
		Vertex *v = pathdag.findVertex(*mpniter);
		if (v == NULL)
		{
			indep_mpnodes.push_back(*mpniter);
		}
	}

	//printf("%d independent MP nodes to combine.\n", indep_mpnodes.size());
	//printf("%d independent MP nodes in dag now.\n", indep_mpnodes.size());

	// calculate the number of possibilities
	//IdList exclude;
	//total = count_consistent_subdag_by_combining_indep_mpnodes(
	//		g, indep_mpnodes, exclude, srid, pathdag);

	total = count_consistent_subdag_by_cutting_fixed_path(g, srid, pathdag, using_hash);

	return total;
}

static int recursion_depth = 0;

// g(u), calculate the number of consistent sub-DAGs in a DAG g
// CAUTION: for the regenerated dag we can't use the hashed value
number_t count_consistent_subdag_for_independent_subdag_nonrecursive(DAG *g, bool using_hash = true)
{
	// profiling
	func_calls[__FUNCTION__]++;

	string vs;
	get_signature(g, vs);
	if (using_hash && !verify_hash)
	{
		//printf("vertices: %s\n", vs.c_str());
		//printf("Hash count: %d\n", hash_table.size());
		auto pos = hash_table.find(vs);
		hash_tries++;
		if (pos != hash_table.end())
		{
			hash_hits++;
			//printf("Hash hit! (%d)\n", hash_hits);
			//printf("Hashed count (%f)\n", pos->second);
			//cout << "Hashed count " << pos->second << endl;
			return pos->second;
		}
	}

	//static int recursion_depth = 0;
	recursion_depth++;

	IdList mpnodes;
	IdList roots;
	g->getRootList(roots);
	//if (roots.size() > 1)
	//{
	//	printf("the independent subdag have more than one root.\n");
	//}
//	int rootid = g->getFirstRoot();
	list<DAG> extend_subdags;

	DAG modified(*g);
	// use the same parent info memory
	modified.setPrivData(g->getPrivData());
	// use same tree case count
//	modified.copyVertexPrivData(*g);

	ParentNumMap &parent_num_map = get_parent_info(g).parentNumMap;
	FOR_EACH_IN_CONTAINER(iter, parent_num_map)
	{
		if (iter->second > 1)
		{
			int vid = iter->first;
			Vertex *v = modified.findVertex(vid);
			if (v != NULL)
			{
				remove_subdag_to_become_trees(
						&modified, vid, extend_subdags);
			}
		}
	}

	number_t total;
	total = count_consistent_subdag_tree(&modified);

	//printf("Tree after remove:\n");
	//modified.print(print_privdata);


	//printf("--------------------------------------------\n");

	FOR_EACH_IN_CONTAINER(iter, extend_subdags)
	{
		count_consistent_subdag_tree(&*iter);
	}
	//printf("Removed subdags:\n");
	//print_subdag_list(extend_subdags, print_privdata);

	//printf("--------------------------------------------\n");

	ParentMap &parent_map = get_parent_info(g).parentMap;

	while (!extend_subdags.empty())
	{
		DAG &subdag = extend_subdags.front();
		int srid = subdag.getFirstRoot();

		if (!check_parent(&modified, srid, parent_map))
		{
			// move to the end of list
			extend_subdags.splice(extend_subdags.end(),
					extend_subdags, extend_subdags.begin());
			//printf("subdag %07d is moved to the end.\n", srid);
			continue;
		}

		//printf("Adding subdag %07d...\n", srid);

		// add it to current DAG and update

		IdList parents;
		get_parents(g, srid, parents);
		//modified.addDAGAsChildOf(parents, subdag);
		modified.transplantAsChildOf(parents, subdag);
		// each subdag added only has one mpnode
		mpnodes.push_back(srid);

		// clear counts for the ancestors of the new added mp node
		clear_ancestor_counts(&modified, srid);

		//printf("after subdag added: \n");
		//modified.print();

		number_t num = count_consistent_subdag_adding_subdag(&modified, mpnodes, subdag, using_hash);
		//printf("num for adding subdag %07d: %.0f\n", srid, num);
		total += num;

		extend_subdags.pop_front();

		if (recursion_depth <= 20)
		{
			for (int di = 1; di < recursion_depth; di++)
			{
				printf("  ");
			}
			printf("[%d] %d MP nodes left.\n", recursion_depth, extend_subdags.size());
		}
	}

	//cout << "the total is " << total << endl;

//	printf("num for node %07d: %.0f.\n", rootid, total);
//	PrivDataUnion priv;
//	priv.ddouble = total;
//	g->setPrivData(rootid, priv);
	//printf("num for subdag with roots (");
	//print_id_list(roots);
	//printf("): %.0f.\n", total);

	if (using_hash)
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

	recursion_depth--;
	return total;
}

pair<int, int> analyze_subproblem_scales_MPV(DAG *g, int id)
{
	DAG m1(*g), m2(*g);

	DAG ancestors, descendents;
	get_path_to_root(&m1, id, ancestors);
	m1.removeSubdag(ancestors);
	get_descendents_subdag(&m2, id, descendents);
	m2.removeSubdag(descendents);
	//m2.removeSubdagRootAt(id);

	int s1, s2;
	IdList mpnodesA, mpnodesD;
	m1.getMultiParentVertices(mpnodesA);
	m2.getMultiParentVertices(mpnodesD);

	s1 = mpnodesA.size();
	s2 = mpnodesD.size();

	return make_pair(s1, s2);
}

pair<int, int> analyze_subproblem_scales_Bound(DAG *g, int id)
{
	DAG m1(*g), m2(*g);

	DAG ancestors, descendents;
	get_path_to_root(&m1, id, ancestors);
	m1.removeSubdag(ancestors);
	get_descendents_subdag(&m2, id, descendents);
	m2.removeSubdag(descendents);
	//m2.removeSubdagRootAt(id);

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

pair<int, int> analyze_subproblem_scales_Bound_double_direction(DAG *g, int id, pair<DAG, DAG> &subprobs)
{
	DAG m1(*g), m2(*g);

	DAG ancestors, descendents;
	get_path_to_root(&m1, id, ancestors);
	m1.removeSubdag(ancestors);
	get_descendents_subdag(&m2, id, descendents);
	m2.removeSubdag(descendents);
	//m2.removeSubdagRootAt(id);

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

	int rs1, rs2;

	DAG rm1(m1), rm2(m2);
	rm1.reverse();
	rm2.reverse();

	r1 = rm1.getRootNum();
	r2 = rm2.getRootNum();

	rs1 = e1 + n1 + r1;
	rs2 = e2 + n2 + r2;

	if (s1 < rs1)
	{
		subprobs.first = m1;
	}
	else
	{
		subprobs.first = rm1;
	}
	if (s2 < rs2)
	{
		subprobs.second = m2;
	}
	else
	{
		subprobs.second = rm2;
	}

	return make_pair(min(s1,rs1), min(s2,rs2));
}

bool comp_scale_pairs_sum(std::pair<int, int> s1, std::pair<int, int> s2)
{
	return ((long long)s1.first + s1.second) < ((long long)s2.first + s2.second);
}

// g(u), calculate the number of consistent sub-DAGs in a DAG g
// CAUTION: for the regenerated dag we can't use the hashed value
number_t count_consistent_subdag_for_independent_subdag(DAG *g, bool using_hash = true)
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
	if (using_hash && !verify_hash)
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

	IdList nodes;
	g->getVertexList(nodes);

	if (mpnodes.size() == 0)
	{
		total = count_consistent_subdag_tree(g);
	}
	//else if (mpnodes.size() < 3)
	//{
	//	total = count_consistent_subdag_for_independent_subdag_nonrecursive(g, using_hash);
	//}
	else
	{
		if (recursion_depth <= 20)
		{
			for (int di = 1; di < recursion_depth; di++)
			{
				printf("  ");
			}
			printf("[%d] %d MP nodes left.\n",
					recursion_depth, mpnodes.size());
		}

		pair<int, int> min_scales = make_pair(INT_MAX, INT_MAX);
		//int min_scales = INT_MAX;
		int best_node = 0;
		pair<DAG, DAG> best_subs_problems;
		// find the best node to split count
		//FOR_EACH_IN_CONTAINER(iter, mpnodes)
		FOR_EACH_IN_CONTAINER(iter, nodes)
		{
			int vid = *iter;
			pair<DAG, DAG> temp_subs_problems;
			pair<int, int> scales =
				analyze_subproblem_scales_Bound_double_direction(
						g, vid, temp_subs_problems);
			//if (scales.first < scales.second)
			//{
			//	swap(scales.first, scales.second);
			//}

			if (comp_scale_pairs_sum(scales, min_scales))
			{
				min_scales = scales;
				best_node = vid;
				best_subs_problems = temp_subs_problems;
			}
		}

		//DAG mA(*g), mD(*g);
		int id = best_node;

		//printf("partitioning with MP node %d\n", id);
		//DAG ancestors, descendents;
		//get_path_to_root(g, id, ancestors);
		//mA.removeSubdag(ancestors);
		//get_descendents_subdag(g, id, descendents);
		//mD.removeSubdag(descendents);

		IdList roots1, roots2;
		DAG &mA = best_subs_problems.first;
		DAG &mD = best_subs_problems.second;
		mA.getRootList(roots1);
		mD.getRootList(roots2);
		number_t num1 = count_consistent_subdag(&mA, roots1, using_hash);
		DAG rmA(mA);
		rmA.reverse();
		rmA.getRootList(roots1);
		number_t num1_c = count_consistent_subdag(&rmA, roots1, using_hash);
		if (num1 != num1_c)
		{
			cout << "found a BUG:" << endl;
			cout << "This is the subdag:" << endl;
			g->print(print_privdata_num);
			cout << "pivoting node: " << best_node << endl;
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

		number_t num2 = count_consistent_subdag(&mD, roots2, using_hash);
		DAG rmD(mD);
		rmD.reverse();
		rmD.getRootList(roots2);
		number_t num2_c = count_consistent_subdag(&rmD, roots2, using_hash);
		if (num2 != num2_c)
		{
			cout << "found a BUG:" << endl;
			cout << "This is the subdag:" << endl;
			g->print(print_privdata_num);
			cout << "pivoting node: " << best_node << endl;
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

		total = num1 + num2;
	}

	if (using_hash)
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


// g(u), calculate the number of consistent sub-DAGs in a DAG g
// but first try to decompose the DAG into several independent parts,
// such that we may reduce the number of independent MP vertices.
number_t count_consistent_subdag(DAG *g, int rootid, bool using_hash)
{
	//DAG rm_root(*g);
	//rm_root.removeVertex(rootid);
	IdList rootlist;
	//rm_root.getRootList(rootlist);
	////return count_consistent_subdag(&rm_root, rootlist) + 1;
	rootlist.push_back(rootid);
	return count_consistent_subdag(g, rootlist, using_hash);
	//number_t num = count_consistent_subdag(&rm_root, rootlist);
	//num += 1;
	//cout << "count with removing root: " << num << endl;
	//return num;
	
	/*
	// profiling
	func_calls[__FUNCTION__]++;

	DAG modified(*g);
//	modified.copyVertexPrivData(*g);

	ParentInfo *parentInfo = new ParentInfo();
	PrivDataUnion privdata;
	privdata.dptr = shared_ptr<void>(parentInfo);
	privdata.type = DT_POINTER;
	modified.setPrivData(privdata);

	gen_parent_num_map(&modified, rootid);
	gen_parent_map(&modified, rootid);

	if (using_hash)
	{
		string vs;
		get_signature(&modified, vs);
		auto pos = hash_table.find(vs);
		hash_tries++;
		if (pos != hash_table.end())
		{
			hash_hits++;
			//printf("Hashed count (%f)\n", pos->second);
			cout << "Hashed count " << pos->second << endl;
			return pos->second;
		}
	}

	IdList mpnodes;
	//list of subdags that are removed from current graph to form a tree
	list<DAG> decomp_subdags;

	ParentNumMap &parent_num_map = get_parent_info(&modified).parentNumMap;
	FOR_EACH_IN_CONTAINER(iter, parent_num_map)
	{
		if (iter->second > 1)
		{
			mpnodes.push_back(iter->first);
		}
	}

	decompose_dag(&modified, mpnodes, decomp_subdags);

	//printf("decomposed subdags.\n");
	//modified.print();
	//print_subdag_list(decomp_subdags);

	ParentMap &parent_map = get_parent_info(&modified).parentMap;

	//count_consistent_subdag_for_independent_subdag(g);
	// count for each
	FOR_EACH_IN_CONTAINER(iter, decomp_subdags)
	{
		DAG *subdag = &*iter;
		free_nodes_pathinfo(subdag);

		//subdag->printEdges();

		// the subdag can have only one parent, that's how we decomposed
		int srid = subdag->getFirstRoot();

		//number_t num = count_consistent_subdag_for_independent_subdag(subdag);
		number_t num = count_consistent_subdag(subdag, srid);
		//printf("count is %f\n", num);
		cout << "count is " << num << endl;

		// add to original dag

		int parent = parent_map[srid].front();

		// just add the root node, so that when laterly calculate
		// with count_consistent_subdag_for_independent_subdag
		// function, the mpnodes from this subdag won't be considered.
		modified.addVertex(srid);
		PrivDataUnion priv;
		//priv.ddouble = num;
		setNumToPrivData(priv, num);
		modified.setPrivData(srid, priv);

		// we should check if there is a parent for this root of subdag
		if (!g->isRoot(srid))
		{
			// if there is any parent, there should be only one
			// because that's the way we define independent subdag
			int parent;
			parent = parent_map[srid].front();
			modified.addEdge(parent, srid);
		}
		else
		{
			// otherwise, add to the root list
			modified.addToRootList(srid);
		}
	}

	//printf("After regenerate.\n");
	//modified.print(print_privdata);
	bool try_hash;
	if (decomp_subdags.size() > 0)
	{
		try_hash = false;
	}


	// get combination
	number_t total = count_consistent_subdag_for_independent_subdag(&modified, try_hash);
	//modified.print(print_privdata);
	//cout << "the total is " << total << endl;

	//delete parentInfo;
	//privdata.dptr = NULL;
	privdata.dptr.reset();
	privdata.type = DT_EMPTY;
	modified.setPrivData(privdata);

	if (using_hash)
	{
		hash_table[vs] = total;
		//printf("hash count: %d (%f)\n",
		//		hash_table.size(), (number_t)hash_hits/hash_tries);
	}
	//printf("hash count: %d (%f)\n",
	//		hash_table.size(), (number_t)hash_hits/hash_tries);

	return total;
	*/
}

//g(u), similar to previous one, but can take multiple roots
number_t count_consistent_subdag(DAG *g, const IdList &rootlist, bool using_hash)
{
	//if (rootlist.size() == 1)
	//{
	//	// single root case
	//	return count_consistent_subdag(g, rootlist.front());
	//}
	//else if (rootlist.size() == 0)
	//{
	//	return 0;
	//}

	// profiling
	func_calls[__FUNCTION__]++;

	if (debugging)
	{
		printf("pruning and counting:\n");
		g->print(print_privdata);
	}

	DAG modified(*g);
//	modified.copyVertexPrivData(*g);

	ParentInfo *parentInfo = new ParentInfo();
	PrivDataUnion privdata;
	privdata.dptr = shared_ptr<void>(parentInfo);
	privdata.type = DT_POINTER;
	modified.setPrivData(privdata);

	gen_parent_num_map(&modified, rootlist);
	gen_parent_map(&modified, rootlist);

	string vs;
	get_signature(&modified, vs);
	//printf("vertices: %s\n", vs.c_str());
	//printf("Hash count: %d\n", hash_table.size());
	if (using_hash && !verify_hash)
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

	IdList mpnodes;
	//list of subdags that are removed from current graph to form a tree
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
		number_t num = count_consistent_subdag(subdag, subroots, using_hash);
		//printf("count is %f\n", num);

		// generate subkey
		string subkey;
		subdag->getVertexString(subkey);

		// add to original dag

		// we can use only one root to represent the subdag
		// even when there are actually multipule roots in the subdag
		int srid = subroots.front();

		// just add the root node, so that when laterly calculate
		// with count_consistent_subdag_for_independent_subdag
		// function, the mpnodes from this subdag won't be considered.
		modified.addVertex(srid);
		PrivDataUnion priv;
		//priv.ddouble = num;
		setNumToPrivData(priv, num);

		// we should check if there is a parent for this root of subdag
		if (subroots.size() == 1 && !g->isRoot(srid))
		{
			// if there is any parent, there should be only one
			// because that's the way we define independent subdag
			int parent;
			parent = parent_map[srid].front();
			// find where the parent is
			DAG *parent_subdag = find_subdag_has(parent, decomp_subdags);
			//if (modified.checkVertex(parent))
			if (parent_subdag == NULL)
			{// it is in the root subdag
				modified.addVertex(srid);
				modified.addEdge(parent, srid);
				modified.setPrivData(srid, priv);
				modified.setSubkey(srid, subkey);
			}
			else
			{// in some later subdag, which will be counted and added later
				parent_subdag->addVertex(srid);
				parent_subdag->addEdge(parent, srid);
				parent_subdag->setPrivData(srid, priv);
				parent_subdag->setSubkey(srid, subkey);
			}
		}
		else
		{
			// otherwise, add as an independent root
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
			printf("After regenerate.\n");
			modified.print(print_privdata);
			// because we substituted a subdag to a vertex, now we can't only use the
			// nodes of the regenerated dag as the hash key to store the count.
			//using_hash = false;
			// Now this is solved by using the subkey in Vertex
			// whenever we prune a subdag to a single node, we save the key of that
			// subdag into the "subkey" member variable in the Vertex structure
		}
	}


	// get combination
	number_t total = count_consistent_subdag_for_independent_subdag(&modified, using_hash);
	//printf("the total is %f\n", total);
	//cout << "the total is " << total << endl;
	//modified.print(print_privdata);


	// parentInfo;
	//privdata.dptr = NULL;
	privdata.dptr.reset();
	privdata.type = DT_EMPTY;
	modified.setPrivData(privdata);

	if (using_hash)
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
		get_path_to_root(&pathinfo_dag, *nodeit, pathdag);
	}

	// remove this path, then what's left should be all trees
	modified.removeSubdag(pathdag);
	
	count_consistent_subdag_tree(&modified);

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

