//Shawn Peng
//This is the main algorithm source file. Detailed explanation please refer
//to the report.

#include "graph_alg.h"

#include "dag.h"
#include "common.h"

#include <algorithm>
#include <queue>
#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <sstream>
#include <memory>

using namespace NS_DAG;

#define MAX_NAME_LEN 63

typedef double number_t;

//used for DAG indepency decomposition, this structure is for node
struct PathInfo
{
	int depth;
	int depthParent;
	map<int, IdList> pathMap; //map<MPnode, children on path to MPnode>
	PathInfo() : depth(0), depthParent(0) {}
};


typedef map<int, int> ParentNumMap;
typedef map<int, IdList> ParentMap;

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


DAG *create_dag_from_matfile(const char *filename)
{
	FILE *f = fopen(filename, "r");
	if (f == NULL)
	{
		perror(__FUNCTION__);
		exit(1);
	}

	DAG *g = new DAG();
	g->addVertex(1);

	int i = 1, j = 1;
	char c = fgetc(f);
	while (c != EOF)
	{
		switch (c)
		{
			case '0':
				j++;
				break;
			case '1':
				g->addVertex(j);
				g->addEdge(i, j);
				j++;
				break;
			case ' ':
				break;
			case '\n':
				j = 1;
				i++;
				break;
			case '\r':
				break;
			default:
				assert(0);
				exit(1);
				break;
		}
		c = fgetc(f);
	}

	if (errno)
	{
		perror(__FUNCTION__);
		exit(1);
	}


	fclose(f);
	return g;
}


DAG *create_dag_from_file(const char *filename)
{
	FILE *f = fopen(filename, "r");
	if (f == NULL)
	{
		perror(__FUNCTION__);
		exit(1);
	}

	char col1_label[MAX_NAME_LEN]; // the header wouldn't be larger than this
	char col2_label[MAX_NAME_LEN]; // the header wouldn't be larger than this
	int ret;

	ret = fscanf(f, "%s%s\n", col1_label, col2_label);

	printf("%s %s\n", col1_label, col2_label);
	if (ret != 2)
	{
		perror(__FUNCTION__);
	}

	if (col1_label[0] == '0')
	{
		// This file is a matrix representation
		fclose(f);
		return create_dag_from_matfile(filename);
	}

	DAG *g = new DAG();

	int go_prefix = true;
	int parent_first = false;

	if (strcmp(col1_label, "parent") == 0 ||
			strcmp(col1_label, "source") == 0)
	{
		parent_first = true;
	}

	while (ret == 2)
	{
		int vstart, vend;

		if (go_prefix)
		{
			ret = fscanf(f, "GO:%d\tGO:%d\n", &vend, &vstart);
		}
		else
		{
			ret = fscanf(f, "%d%d\n", &vend, &vstart);
		}

		if (ret != 2)
		{
			go_prefix = false;
			ret = fscanf(f, "%d%d\n", &vend, &vstart);
		}

		if (parent_first)
		{
			swap(vend, vstart);
		}

		if (ret != 2)
		{
			if (errno)
			{
				perror(__FUNCTION__);
			}
		}
		else
		{
			g->addVertex(vstart);
			g->addVertex(vend);

			g->addEdge(vstart, vend);

			//g->print();
		}
	}

	fclose(f);
	return g;
}

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

void print_edge_list(const EdgeList &edgelist)
{
	FOR_EACH_IN_CONTAINER(elit, edgelist)
	{
		printf("%07d-->%07d\n", elit->vstart, elit->vend);
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

int print_privdata(PrivDataUnion *data)
{
	return printf("%.0f", data->ddouble);
}

int print_privdata_int(PrivDataUnion *data)
{
	return printf("%d", data->dint);
}

int print_privdata_num(PrivDataUnion *data)
{
	//return printf("%.0f", data->ddouble);
}

int print_privdata_ptr(PrivDataUnion *data)
{
	return printf("%p", data->dptr.get());
}

int print_privdata_pathinfo(PrivDataUnion *data)
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
	//int rootid = g->getRoot();
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


//D(u), get the smallest consistent sub-DAG for vertex u with ID(id)
int get_path_to_root(DAG *g, int id, DAG &subdag)
{
	// may need several path
	//int rootid = g->getRoot();

	Vertex *v = subdag.findVertex(id);
	subdag.addVertex(id);

	IdList parents;
	get_parents(g, id, parents);

	IdList pathp;
	if (v != NULL)
	{
		v->getParentList(pathp);
	}

	// add parent(s) to path
	FOR_EACH_IN_CONTAINER(iter, parents)
	{
		int pid = *iter;
		IdList::iterator ppit = find(pathp.begin(), pathp.end(), *iter);
		if (ppit != pathp.end())
		{
			//this edge already in path
			continue;
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
		}
		else
		{
			get_path_to_root(g, pid, subdag);
		}
	}
	return 0;
}

// Similar to above, but check if any vertex in 'exclude' will be added
// to the path, if that's the case, return 1
int get_path_to_root_with_exclusion(DAG *g, int id,
		const IdList &exclude, DAG &subdag)
{
	// may need several path
	//int rootid = g->getRoot();

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

	//int rootid = g->getRoot();
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
	//int rootid = g->getRoot();
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

	//int realroot = g->getRoot();
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
		free_nodes_pathinfo(&tempdag);
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

	free_nodes_pathinfo(&pathinfo_dag);

	return 0;
}



// sanity check method
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

	int vid = edge.vend;

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
		cur_subdag.addEdge(edge.vstart, vid);
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
	g0.setRoot(rootid);

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
	IdList children;
	Vertex *v = g->findVertex(rootid);
	assert(v);
	//number_t total = v->getPrivData().ddouble;
	PrivDataUnion data = v->getPrivData();
	number_t total = getNumFromPrivData(data);
	if (total != 0)
	{
		//printf("num %.0f for %07d.\n", total, rootid);
		return total;
	}

	total = 1.0;
	g->getChildList(rootid, children);
	FOR_EACH_IN_CONTAINER(iter, children)
	{
		total *= (1 + count_consistent_subdag_tree(g, *iter));
	}
	//PrivDataUnion data;
	//data.ddouble = total;
	setNumToPrivData(data, total);
	g->setPrivData(rootid, data);

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
		total *= (1 + count_consistent_subdag_tree(g, rootid));
	}

	total -= 1;

	return total;
}


// g(r - cutted)
number_t count_consistent_subdag_cutting_children(DAG *g, int rootid, const IdList &cutted)
{
	assert(g->findVertex(rootid));
	//number_t count = g->getPrivData(rootid).ddouble;
	PrivDataUnion data = g->getPrivData(rootid);
	number_t count = getNumFromPrivData(data);

	ParentNumMap &parent_num_map = get_parent_info(g).parentNumMap;

	IdList children;
	g->getChildList(rootid, children);
	FOR_EACH_IN_CONTAINER(iter, cutted)
	{
		// the cutted nodes should all be children of root
		int idcutted = *iter;
		IdList::iterator clit =
			find(children.begin(), children.end(), idcutted);
		if (clit == children.end())
		{
			// cutted vertex/edge hasn't been added to this dag,
			// so it's already cutted. (Acctually, vertex added but
			// edge not added should never be the case.)
			continue;
		}
		if (parent_num_map[idcutted] > 1)
		{
			// multi-parent node is added to the dag in calculation
			// this also shouldn't be divided
			continue;
		}

		// we can calculate this number by divide the number of
		// not cutting by the number of cutted vertex plus 1.
		// (Proof see report)
		//count /= g->getPrivData(idcutted).ddouble + 1;
		data = g->getPrivData(idcutted);
		count /= getNumFromPrivData(data) + 1;
	}
	return count;
}


// g(u|fixed), calculate the number of possibilities with fixed path
number_t count_comb_with_fixed_nodes(DAG *g, const DAG &fixed)
{
	// All independent nodes must in the fixed list
	IdList vs;
	fixed.getVertexList(vs);

	number_t total = 1.0;

	FOR_EACH_IN_CONTAINER(fixedvit, vs)
	{
		IdList fixed_children; // this would be the cutted nodes
		fixed.getChildList(*fixedvit, fixed_children);
		if (fixed_children.empty())
		{
			// this should mean that this node is the root of the subdag
			// or maybe there are multiple independent MP node in fixed
			// all treated as the same.
			//total *= g->getPrivData(*fixedvit).ddouble;
			PrivDataUnion data = g->getPrivData(*fixedvit);
			total *= getNumFromPrivData(data);
			continue;
		}

		//g(r) *= g(f_i - fixed.children(f_i))
		//f_i : the i-th fixed vertex
		//Proof see report
		total *= count_consistent_subdag_cutting_children(
				g, *fixedvit, fixed_children);
	}

	return total;
}

// for all vertices in list 'nodes', generate the possible combinations
// and get the path(minimum consistent sub-DAG) for each combination
// add them to the list 'paths'
int gen_path_combinations(DAG *g, IdList nodes,
		IdList &exclude, int fixed,
		const DAG &fixed_path, list<DAG> &paths)
{
	if (nodes.empty())
	{
		paths.push_back(fixed_path);
		return 0;
	}

	int id = nodes.front();
	nodes.pop_front();

	// check path
	Vertex *v = fixed_path.findVertex(id);
	if (v != NULL)
	{
		assert(0);
		printf("Node %07d already in the path !\n", v->getId());
		// Then must choosing this node, but not need to add it to path
		// since it's already done
		gen_path_combinations(g, nodes, exclude, fixed, fixed_path, paths);
		return 0;
	}

	// not choosing

	// add to exclude
	exclude.push_back(id);
	gen_path_combinations(g, nodes, exclude, fixed, fixed_path, paths);
	// then pop it
	exclude.pop_back();

	if (id != fixed)
	{
		// choosing
		DAG new_path(fixed_path);
		int ret = get_path_to_root_with_exclusion(
				g, id, exclude, new_path);
		if (ret != 0)
		{
			// this node can't be choosen, because there are
			// some parent was not choosen at previous process
			// then we just need to return
			return 0;
		}


		gen_path_combinations(g, nodes, exclude, fixed, new_path, paths);
	}

	return 0;
}

void uppropagate_counts(DAG *g, int parent,
		const number_t &new_count, const number_t &old_count)
{
	Vertex *v = g->findVertex(parent);
	PrivDataUnion priv = v->getPrivData();
	//int this_old = priv.ddouble;
	//priv.ddouble /= old_count + 1;
	//priv.ddouble *= new_count + 1;
	number_t &count = getNumFromPrivData(priv);
	number_t this_old = count;
	count /= old_count + 1;
	count *= new_count + 1;
	v->setPrivData(priv);

	IdList grandplist;
	v->getParentList(grandplist);
	if (grandplist.size() == 1)
	{
		uppropagate_counts(g, grandplist.front(),
				getNumFromPrivData(priv), this_old);
	}
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

/**
 * count_consistent_subdag_by_combining_indep_mpnodes
 *
 * @desc similar to above function, but rather than save all possible paths
 * to a list, calculate the number of consistent sub-DAGs with the
 * fixed path when each path is finished. Then sum them up.
 * This way we can reduce the space complexity.
 *
 * @para g, DAG to count
 * @para nodes, MP nodes to combine, there might be some nodes that are
 * dependent
 * @para exclude, nodes that are not choosen in previous process,
 * this works as a stack
 * @para fixed, the fixed node which is what we are adding
 * @para fixed_path, the fixed path for now
 */
number_t count_consistent_subdag_by_combining_indep_mpnodes(
		DAG *g, IdList nodes, IdList &exclude, int fixed,
		const DAG &fixed_path)
{
	number_t total = 0;

	if (nodes.empty())
	{
		//fixed_path.print();
		total += count_comb_with_fixed_nodes(g, fixed_path);
		return total;
	}

	int id = nodes.front();
	nodes.pop_front();

	// check path

	Vertex *v = fixed_path.findVertex(id);
	if (v != NULL)
	{
		assert(0);
		printf("Node %07d already in the path !\n", v->getId());
		// Then must choosing this node, but not need to add it to path
		// since it's already done
		total += count_consistent_subdag_by_combining_indep_mpnodes(
				g, nodes, exclude, fixed, fixed_path);
		return total;
	}

	// not choosing
	// first remember the position of path list now

	// add to exclude
	exclude.push_back(id);
	total += count_consistent_subdag_by_combining_indep_mpnodes(
			g, nodes, exclude, fixed, fixed_path);
	// then pop it
	exclude.pop_back();

	if (id != fixed)
	{
		// choosing
		DAG new_path(fixed_path);
		int ret = get_path_to_root_with_exclusion(
				g, id, exclude, new_path);
		if (ret != 0)
		{
			// this node can't be choosen, because there are
			// some parent was not choosen at previous process
			// then we just need to return
			// Although this node can't be choosen, those possibilities
			// that not including this node should be returned.
			return total;
		}


		total += count_consistent_subdag_by_combining_indep_mpnodes(
				g, nodes, exclude, fixed, new_path);
	}

	return total;
}

// g(DAG|fixed) calculate using the cutting fixed path method
number_t count_consistent_subdag_by_cutting_fixed_path(DAG *g, int fixed, const DAG &fixed_path)
{
	// profiling
	func_calls[__FUNCTION__]++;

	// copy DAG
	DAG modified(*g);
//	modified.copyVertexPrivData(*g);

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

	// there is at least one base case, the path itself
	number_t total = 1.0;

	// Check there is something left
	if (rootlist.size() != 0)
	{
		//printf("DAG after fixed path cutted.\n");
		//modified.print(print_privdata);
		total += count_consistent_subdag(&modified, rootlist);
	}
	else
	{
		// all nodes need to be fixed
		printf("Nothing left after cut, all nodes in the DAG needs to be fixed.\n");
	}

	//delete parentInfo;
	//privdata.dptr = NULL;
	privdata.dptr.reset();
	privdata.type = DT_EMPTY;
	modified.setPrivData(privdata);

	return total;
}


number_t count_consistent_subdag_adding_subdag(DAG *g, IdList mpnodes, const DAG &subdag)
{
	// profiling
	func_calls[__FUNCTION__]++;

	// the root of subdag must be choosen(fixed)

	//int rootid = g->getRoot();

	// even for multi-root DAG case, the subdag still have only one root
	int srid = subdag.getRoot();
	DAG pathdag;
	get_path_to_root(g, srid, pathdag);
	//pathdag.setRoot(rootid);
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

	total = count_consistent_subdag_by_cutting_fixed_path(g, srid, pathdag);

	return total;
}


// g(u), calculate the number of consistent sub-DAGs in a DAG g
number_t count_consistent_subdag_for_independent_subdag(DAG *g)
{
	// profiling
	func_calls[__FUNCTION__]++;

	string vs;
	get_signature(g, vs);
	//printf("vertices: %s\n", vs.c_str());
	//printf("Hash count: %d\n", hash_table.size());
	auto pos = hash_table.find(vs);
	hash_tries++;
	if (pos != hash_table.end())
	{
		hash_hits++;
		//printf("Hash hit! (%d)\n", hash_hits);
		//printf("Hashed count (%f)\n", pos->second);
		return pos->second;
	}

	static int recursion_depth = 0;
	recursion_depth++;

	IdList mpnodes;
	IdList roots;
	g->getRootList(roots);
	//if (roots.size() > 1)
	//{
	//	printf("the independent subdag have more than one root.\n");
	//}
//	int rootid = g->getRoot();
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

	//printf("Tree after remove:\n");

	number_t total;
	total = count_consistent_subdag_tree(&modified);

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
		int srid = subdag.getRoot();

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

		//printf("Updated: \n");
		//modified.print();

		number_t num = count_consistent_subdag_adding_subdag(&modified, mpnodes, subdag);
		//printf("num for adding subdag %07d: %.0f\n", srid, num);
		total += num;

		extend_subdags.pop_front();

		if (recursion_depth <= 2)
		{
			printf("[%d] %d MP nodes left.\n", recursion_depth, extend_subdags.size());
		}
	}

//	printf("num for node %07d: %.0f.\n", rootid, total);
//	PrivDataUnion priv;
//	priv.ddouble = total;
//	g->setPrivData(rootid, priv);
	//printf("num for subdag with roots (");
	//print_id_list(roots);
	//printf("): %.0f.\n", total);

	hash_table[vs] = total;
	//printf("hash count: %d (%f)\n",
	//		hash_table.size(), (number_t)hash_hits/hash_tries);

	recursion_depth--;
	return total;
}


// g(u), calculate the number of consistent sub-DAGs in a DAG g
// but first try to decompose the DAG into several independent parts,
// such that we may reduce the number of independent MP vertices.
number_t count_consistent_subdag(DAG *g, int rootid)
{
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

	string vs;
	get_signature(&modified, vs);
	auto pos = hash_table.find(vs);
	hash_tries++;
	if (pos != hash_table.end())
	{
		hash_hits++;
		//printf("Hashed count (%f)\n", pos->second);
		return pos->second;
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
		int srid = subdag->getRoot();

		//number_t num = count_consistent_subdag_for_independent_subdag(subdag);
		number_t num = count_consistent_subdag(subdag, srid);
		printf("count is %f\n", num);

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


	// get combination
	number_t total = count_consistent_subdag_for_independent_subdag(&modified);
	//modified.print(print_privdata);

	//delete parentInfo;
	//privdata.dptr = NULL;
	privdata.dptr.reset();
	privdata.type = DT_EMPTY;
	modified.setPrivData(privdata);

	hash_table[vs] = total;
	//printf("hash count: %d (%f)\n",
	//		hash_table.size(), (number_t)hash_hits/hash_tries);

	return total;
}

//g(u), similar to previous one, but can take multiple roots
number_t count_consistent_subdag(DAG *g, const IdList &rootlist)
{
	// profiling
	func_calls[__FUNCTION__]++;

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
	auto pos = hash_table.find(vs);
	hash_tries++;
	if (pos != hash_table.end())
	{
		hash_hits++;
		//printf("Hash hit! (%d)\n", hash_hits);
		//printf("Hashed count (%f)\n", pos->second);
		return pos->second;
	}

	IdList mpnodes;
	//list of subdags that are removed from current graph to form a tree
	list<DAG> decomp_subdags;

	get_mpnodes(&modified, mpnodes);

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
		//free_nodes_pathinfo(subdag);

		//subdag->printEdges();

		IdList subroots;
		subdag->getRootList(subroots);

		//number_t num = count_consistent_subdag_for_independent_subdag(subdag);
		number_t num = count_consistent_subdag(subdag, subroots);
		//printf("count is %f\n", num);

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
		modified.setPrivData(srid, priv);

		// we should check if there is a parent for this root of subdag
		if (subroots.size() == 1 && !g->isRoot(srid))
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


	// get combination
	number_t total = count_consistent_subdag_for_independent_subdag(&modified);
	//printf("the total is %f\n", total);
	//modified.print(print_privdata);


	// parentInfo;
	//privdata.dptr = NULL;
	privdata.dptr.reset();
	privdata.type = DT_EMPTY;
	modified.setPrivData(privdata);

	hash_table[vs] = total;
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

	//g->setRoot(rootid);
	int ret;

	g->print();
	printf("Vertices: %d\n", g->getVertexNum());
	//g->print(988);
	list<DAG> subdags;
	if (g->getVertexNum() <= 35 && rootlist.size() == 1)
	{
		// sanity check
		rootid = rootlist.front();
		ret = get_consistent_subdag(g, rootid, subdags);
		//print_subdag_list(subdags);
	}

	reduce_dag(g);

	//g->printEdges();

	//return 0;

	number_t num;
	//num = count_consistent_subdag_for_independent_subdag(g);
	//printf("Num of consistent sub-DAG: %.0f\n", num);
	//printf("====================================================");
	//printf("====================================================\n");
	num = count_consistent_subdag(g, rootlist);
	printf("Num of consistent sub-DAG: %.0f\n", num);

	printf("(Sanity check)Num of consistent sub-DAG: %d\n", subdags.size());

	printf("Hash tries: %d\nHash hits: %d\nHit rates: %f\n",
			hash_tries, hash_hits, (number_t)hash_hits/hash_tries);
	printf("Hash count: %d\n", hash_table.size());

	FOR_EACH_IN_CONTAINER(iter, func_calls)
	{
		printf("Function %s has been called %d times.\n", iter->first.c_str(), iter->second);
	}
	free_dag(g);
	return 0;
}
