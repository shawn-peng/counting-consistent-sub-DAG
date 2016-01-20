//Shawn Peng
//This is the main algorithm source file. Detailed explanation please refer
//to the report.

#include "dag.h"
#include "common.h"

#include <algorithm>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

using namespace NS_DAG;

#define MAX_NAME_LEN 63


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

	while (ret == 2)
	{
		int vstart, vend;
		ret = fscanf(f, "GO:%d\tGO:%d\n", &vend, &vstart);
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

void print_subdag_list(const list<DAG> &glist, PrivDataFn fn)
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
	printf("\n");
}

int print_privdata(PrivDataUnion data)
{
	return printf("%.0f", data.ddouble);
}

int print_privdata_int(PrivDataUnion data)
{
	return printf("%d", data.dint);
}

static map<int, int> parent_num_map;

static map<int, IdList> parent_map; // only save id for multiple parents

// get in-degree for all vertices
int gen_parent_num_map(DAG *g, int parent)
{
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
		parent_num_map[*iter] = g->getParentNum(*iter);
		printf("%07d:%d\n", *iter, parent_num_map[*iter]);
		gen_parent_num_map(g, *iter);
	}

	return 0;
}

int gen_parent_map(DAG *g, int parent)
{
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


int get_parents(int id, IdList &plist)
{
	typeof(parent_map.begin()) map_it = parent_map.find(id);
	if (map_it != parent_map.end())
	{
		plist.insert(plist.end(), map_it->second.begin(), map_it->second.end());
	}
	else
	{
		printf("Error in %s: parent map for id:%07d not found.\n", __FUNCTION__, id);
		exit(1);
	}
	return 0;
}

//check if all direct parents are in cur_graph
bool check_parent(DAG *g, int id)
{
	IdList &parents = parent_map[id];
	FOR_EACH_IN_CONTAINER(pidit, parents)
	{
		Vertex *v = g->findVertex(*pidit);
		if (v == NULL)
		{
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
	int flag = 1;
	int np = parent_num_map[vid];
	if (np > 1)
	{
		if (!check_parent(&cur_subdag, vid))
		{
			//this node can't be choosen
			//Because we checked before putting the node into fringe,
			//this shouldn't happen.
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
		IdList &parents = parent_map[vid];
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
		int np = parent_num_map[chid];
		if (np > 1)
		{
			//check if all direct parents are in cur_graph
			if (!check_parent(&cur_subdag, chid))
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
		fringe.push_front(Edge(rootid,*chidit));
	}

	ret = enum_possibilities(g, g0, fringe, IdList(), subdags);

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
	printf("Removing subdag with root: %07d...\n", rootid);

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
	g->removeSubdag(rootid, subdags.back());

	return 0;
}


//for tree case
double count_consistent_subdag_tree(DAG *g, int rootid)
{
	IdList children;
	Vertex *v = g->findVertex(rootid);
	assert(v);
	double total = v->getPrivData().ddouble;
	if (total != 0)
	{
		printf("num %.0f for %07d.\n", total, rootid);
		return total;
	}

	total = 1.0;
	g->getChildList(rootid, children);
	FOR_EACH_IN_CONTAINER(iter, children)
	{
		total *= (1 + count_consistent_subdag_tree(g, *iter));
	}
	PrivDataUnion data;
	data.ddouble = total;
	g->setPrivData(rootid, data);

	return total;
}


//D(u), get the smallest consistent sub-DAG for vertex u with ID(id)
int get_path_to_root(DAG *g, int id, DAG &subdag)
{
	// may need several path
	int rootid = g->getRoot();

	Vertex *v = subdag.findVertex(id);
	subdag.addVertex(id);
	
	IdList parents;
	get_parents(id, parents);

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
		subdag.addVertex(pid);
		subdag.addEdge(pid, id);
	}
	FOR_EACH_IN_CONTAINER(iter, parents)
	{
		int pid = *iter;
		if (pid != rootid)
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
	int rootid = g->getRoot();

	Vertex *v = subdag.findVertex(id);
	subdag.addVertex(id);
	
	IdList parents;
	get_parents(id, parents);

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
		DECLARE_ITERATER(excit, exclude);
		excit = find(exclude.begin(), exclude.end(), pid);
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
		if (pid != rootid)
		{
			get_path_to_root(g, pid, subdag);
		}
	}
	return 0;
}

// g(r - cutted)
double count_subdag_cutting_children(DAG *g, int rootid, const IdList &cutted)
{
	assert(g->findVertex(rootid));
	double count = g->getPrivData(rootid).ddouble;
	
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
		count /= g->getPrivData(idcutted).ddouble + 1;
	}
	return count;
}
			

// g(u|fixed), calculate the number of possibilities with fixed path
double count_comb_with_fixed_nodes(DAG *g, const DAG &fixed)
{
	// All independent nodes must in the fixed list
	IdList vs;
	fixed.getVertexList(vs);

	double total = 1.0;

	FOR_EACH_IN_CONTAINER(fixedvit, vs)
	{
		IdList fixed_children; // this would be the cutted nodes
		fixed.getChildList(*fixedvit, fixed_children);
		if (fixed_children.empty())
		{
			// this should mean that this node is the root of the subdag
			// or maybe there are multiple independent MP node in fixed
			// all treated as the same.
			total *= g->getPrivData(*fixedvit).ddouble;
			continue;
		}
		
		//g(r) *= g(f_i - fixed.children(f_i))
		//f_i : the i-th fixed vertex
		//Proof see report
		total *= count_subdag_cutting_children(
				g, *fixedvit, fixed_children);
	}
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

// similar to above function, but rather than save all possible paths
// to a list, calculate the number of consistent sub-DAG's with the
// fixed path when each path is finished. Then sum them up.
// This way we can reduce the space complexity.
double count_consistent_subdag_by_combining_indep_mpnodes(
		DAG *g, IdList nodes, IdList &exclude, int fixed,
		const DAG &fixed_path)
{
	double total = 0;

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


double count_comb_adding_subdag(DAG *g, IdList mpnodes, const DAG &subdag)
{
	// the root of subdag must be choosen
	//
	int rootid = g->getRoot();
	int srid = subdag.getRoot();
	DAG pathdag;
	get_path_to_root(g, srid, pathdag);
	pathdag.setRoot(rootid);
	//printf("--------------------------------------------\n");
	//printf("Path to root of node: %07d\n", srid);
	//pathdag.print();

	double total = 0;
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

	printf("%d independent MP nodes to combine.\n", indep_mpnodes.size());

	// calculate the number of possibilities
	IdList exclude;
	total = count_consistent_subdag_by_combining_indep_mpnodes(
			g, indep_mpnodes, exclude, srid, pathdag);

	return total;
}


//used for DAG indepency decomposition
struct PathInfo
{
	int depth;
	int depthParent;
	map<int, IdList> pathMap; //map<MPnode, children on path to MPnode>
	PathInfo() : depth(0), depthParent(0) {}
};

PathInfo *get_path_info(const PrivDataUnion &privdata)
{
	return (PathInfo *)privdata.dptr;
}

PathInfo *get_path_info(const Vertex *v)
{
	return get_path_info(v->getPrivData());
}

PathInfo *get_path_info(DAG *g, int id)
{
	return get_path_info(g->getPrivData(id));
}

int print_privdata_pathinfo(PrivDataUnion data)
{
	PathInfo *pinfo = (PathInfo *)data.dptr;
	map<int, IdList> &pmap = pinfo->pathMap;
	printf("{\n");
	FOR_EACH_IN_CONTAINER(iter, pmap)
	{
		printf("\t%07d: [\n\t\t", iter->first);
		print_id_list(iter->second);
		printf("\t]\n");
	}
	printf("}\n");
	FOR_EACH_IN_CONTAINER(iter, pmap)
	return 0;
}

void freePathInfo(void *ptr)
{
	if (ptr != NULL)
	{
		free(ptr);
	}
	return;
}

int free_nodes_pathinfo(DAG *g)
{
	IdList fringe;
	int rootid = g->getRoot();
	fringe.push_back(rootid);

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
		pinfo = (PathInfo *)privdata.dptr;
		if (pinfo == NULL)
		{
			continue;
		}
		delete pinfo;
		privdata.dptr = NULL;
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

int add_path_to_pathmap(DAG *g, int targetid, const DAG &pathdag)
{
	IdList fringe;
	int rootid = g->getRoot(); // should same to pathdag
	fringe.push_back(rootid);

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

		PathInfo *pinfo = (PathInfo *)g->getPrivData(id).dptr; // path info is saved in g
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



int gen_mpnode_pathinfo(DAG *g, IdList mpnodes)
{
	IdList fringe;
	int rootid = g->getRoot();
	fringe.push_back(rootid);

	PathInfo *pinfo = new PathInfo();
	pinfo->depth = 0;
	pinfo->depthParent = 0;

	PrivDataUnion privdata;
	privdata.dptr = (void *)pinfo;
	g->setPrivData(rootid, privdata);

	while (!fringe.empty())
	{
		int id = fringe.front();
		fringe.pop_front();

		Vertex *v = g->findVertex(id);
		assert(v);
		pinfo = (PathInfo *)v->getPrivData().dptr;
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
			pinfo = (PathInfo *)chprivdata.dptr;
			if (pinfo == NULL)
			{
				pinfo = new PathInfo();
				PrivDataUnion pd;
				pd.dptr = (void *)pinfo;
				vch->setPrivData(pd);
			}
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

struct PathNode
{
	int depth;
	Vertex *v;
};

int add_node_to_pathnode_list(list<PathNode> &pl, PathNode x)
{
	//pl is sorted
	FOR_EACH_IN_CONTAINER(iter, pl)
	{
		if (iter->depth > x.depth)
		{
			continue;
		}
		else if (iter->depth == x.depth &&
				iter->v->getId() == x.v->getId())
		{
			return 0;
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
int find_independent_subdag_root(DAG *g, int id)
{
	list<PathNode> paths_queue;

	PathNode node;

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
			PathInfo *pinfo = (PathInfo *)node.v->getPrivData().dptr;
			node.depth = pinfo->depth;
			add_node_to_pathnode_list(paths_queue, node);
			//printf("push depth %d: %07d\n", node.depth, node.v->getId());
		}
		node = paths_queue.front();
		id = node.v->getId();
		paths_queue.pop_front();
		//printf("pop depth %d: %07d\n", node.depth, node.v->getId());
	}

	return id;
}

int get_mpnodes_under_node(DAG *g, int id, IdList &nodes)
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
		nodes.push_back(iter->first);
	}
	return 0;
}

int find_root_consistent_to_vertices(DAG *g, IdList nodes)
{
	list<PathNode> paths_queue;

	PathNode node;
	
	FOR_EACH_IN_CONTAINER(iter, nodes)
	{
		Vertex *v = g->findVertex(*iter);
		node.v = v;
	
		PathInfo *pinfo = get_path_info(v);
		node.depth = pinfo->depth;
	
		add_node_to_pathnode_list(paths_queue, node);
	}
	
	node = paths_queue.front();
	int id = node.v->getId();
	paths_queue.pop_front();

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
			PathInfo *pinfo = (PathInfo *)node.v->getPrivData().dptr;
			node.depth = pinfo->depth;
			add_node_to_pathnode_list(paths_queue, node);
			//printf("push depth %d: %07d\n", node.depth, node.v->getId());
		}
		node = paths_queue.front();
		id = node.v->getId();
		paths_queue.pop_front();
		//printf("pop depth %d: %07d\n", node.depth, node.v->getId());
	}

	return id;
}


// independent decomposition
int decompose_dag(DAG *g, const IdList mpnodes, list<DAG> &subdags)
{
	int realroot = g->getRoot();
	IdList removed_mpnodes;
	gen_mpnode_pathinfo(g, mpnodes);
	FOR_EACH_IN_CONTAINER(iter, mpnodes)
	{
		int id = *iter;
		Vertex *v = g->findVertex(id);
		if (v == NULL)
		{
			continue;
		}

		printf("decomposing for node %07d.\n", id);

		int rootid = 0;
		int newrootid = id;

		IdList covered_mpnodes;
		covered_mpnodes.push_back(id);
		
		while (rootid != newrootid && newrootid != realroot)
		{
			rootid = newrootid;
			newrootid = find_root_consistent_to_vertices(
					g, covered_mpnodes);
			printf("independent root %07d for node %07d.\n", newrootid, id);

			// get map of paths to all mpnodes, check consistency of
			// subdag with root rootid
			covered_mpnodes.clear();
			get_mpnodes_under_node(g, newrootid, covered_mpnodes);
			printf("mpnodes under %07d: ", newrootid);
			print_id_list(covered_mpnodes);
		}

		if (newrootid == g->getRoot())
		{
			//return 0;
			continue;
		}
		subdags.push_back(DAG());
		g->removeSubdag(newrootid, subdags.back());
	}

	//print_subdag_list(subdags);
	return 0;
}

// g(u), calculate the number of independent sub-DAG's in a DAG g
double count_consistent_subdag_for_independent_subdag(DAG *g)
{
	IdList mpnodes;
	int rootid = g->getRoot();
	list<DAG> extend_subdags;
	DAG modified(*g);
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

	double total;
	total = count_consistent_subdag_tree(&modified, rootid);

	//modified.print(print_privdata);

	//printf("--------------------------------------------\n");

	FOR_EACH_IN_CONTAINER(iter, extend_subdags)
	{
		count_consistent_subdag_tree(&*iter, iter->getRoot());
	}
	//printf("Removed subdags:\n");
	//print_subdag_list(extend_subdags, print_privdata);

	//printf("--------------------------------------------\n");

	while (!extend_subdags.empty())
	{
		DAG &subdag = extend_subdags.front();
		int srid = subdag.getRoot();
		if (!check_parent(&modified, srid))
		{
			// move to the end of list
			extend_subdags.splice(extend_subdags.end(),
					extend_subdags, extend_subdags.begin());
			printf("subdag %07d is moved to the end.\n", srid);
			continue;
		}

		// add it to current DAG and update

		IdList parents;
		get_parents(srid, parents);
		modified.addDAGAsChildOf(parents, subdag);
		mpnodes.push_back(srid);

		//printf("Updated: \n");
		//modified.print();

		double num = count_comb_adding_subdag(&modified, mpnodes, subdag);
		printf("num for adding subdag %07d: %.0f\n", srid, num);
		total += num;

		extend_subdags.pop_front();

		printf("%d MP trees left.\n", extend_subdags.size());
	}

	printf("num for node %07d: %.0f.\n", rootid, total);
	PrivDataUnion priv;
	priv.ddouble = total;
	g->setPrivData(rootid, priv);

	return total;
}


// g(u), calculate the number of independent sub-DAG's in a DAG g
// but first try to decompose the DAG into several independent parts,
// such that we may reduce the number of independent MP vertices.
double count_consistent_subdag(DAG *g, int rootid)
{
	DAG modified(*g);

	IdList mpnodes;
	//list of subdags that are removed from current graph to form a tree
	list<DAG> decomp_subdags;

	FOR_EACH_IN_CONTAINER(iter, parent_num_map)
	{
		if (iter->second > 1)
		{
			mpnodes.push_back(iter->first);
		}
	}
	
	decompose_dag(g, mpnodes, decomp_subdags);

	printf("decomposed subdags.\n");
	g->print();
	print_subdag_list(decomp_subdags);

	free_nodes_pathinfo(g);

	//count_consistent_subdag_for_independent_subdag(g);
	// count for each
	FOR_EACH_IN_CONTAINER(iter, decomp_subdags)
	{
		DAG *subdag = &*iter;
		free_nodes_pathinfo(subdag);

		subdag->printEdges();

		double num = count_consistent_subdag_for_independent_subdag(subdag);

		// add to original dag
		int parent;
		// the subdag can have only one parent, that's how we decomposed
		int id = subdag->getRoot();

		parent = parent_map[id].front();
		// just add the root node, so that when laterly calculate
		// with count_consistent_subdag_for_independent_subdag
		// function, the mpnodes from this subdag won't be considered.
		g->addVertex(id);
		g->addEdge(parent, id);
		PrivDataUnion priv;
		priv.ddouble = num;
		g->setPrivData(id, priv);
	}

	//printf("After regenerate.\n");
	//g->print(print_privdata);
	
	
	// get combination
	double total = count_consistent_subdag_for_independent_subdag(g);
	//g->print(print_privdata);
	
	return total;
}


void free_dag(DAG *g)
{
	delete g;
}

int main(int argc, char *argv[])
{
	const char *datafile = "Graphs/mini-tree.txt";
	int rootid = 1;
	if (argc >= 2)
	{
		datafile = argv[1];
	}
	if (argc >= 3)
	{
		sscanf(argv[2], "%d", &rootid);
	}
	DAG *g = create_dag_from_file(datafile);
	g->setRoot(rootid);
	int ret;

	g->print();
	//g->print(988);

	gen_parent_num_map(g, rootid);
	gen_parent_map(g, rootid);

	list<DAG> subdags;
	if (g->getVertexNum() <= 25)
	{
		// sanity check
		ret = get_consistent_subdag(g, rootid, subdags);
		//print_subdag_list(subdags);
		printf("Num of consistent sub-DAG: %d\n", subdags.size());
	}
	double num;
	//num = count_consistent_subdag_for_independent_subdag(g);
	//printf("Num of consistent sub-DAG: %.0f\n", num);
	//printf("====================================================");
	//printf("====================================================\n");
	num = count_consistent_subdag(g, rootid);
	printf("Num of consistent sub-DAG: %.0f\n", num);
	free_dag(g);
	return 0;
}
