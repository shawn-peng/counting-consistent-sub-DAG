//Shawn Peng
//This is the DAG data structure source file, defining methods for the DAG class,
//the Vertex class and the Edge class.

#include "dag.h"
#include "common.h"

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

using namespace std;

namespace NS_DAG
{

Vertex::Vertex(int vid) : id(vid)
{
}

int Vertex::getId() const
{
	return id;
}

void Vertex::addEdge(int vstart, int vend)
{
	edges.push_back(Edge(vstart,vend));
	return;
}

void Vertex::addParent(int id)
{
	parents.push_back(id);
}

int Vertex::removeEdge(int vend)
{
	FOR_EACH_IN_CONTAINER(iter, edges)
	{
		if (iter->vend == vend)
		{
			edges.erase(iter);
			return 0;
		}
	}
	return 1;
}

int Vertex::removeParent(int parent)
{
	FOR_EACH_IN_CONTAINER(iter, parents)
	{
		if (*iter == parent)
		{
			parents.erase(iter);
			return 0;
		}
	}
	return 1;
}

int Vertex::getParentNum()
{
	// this should be in constant length
	return parents.size();
}

int Vertex::getParentList(IdList &list) const
{
	list.insert(list.end(), parents.begin(), parents.end());
	return 0;
}

void Vertex::print(int depth) const
{
	printf("%07d", id);
	if (depth == 0)
	{
		printf("\n");
	}

	if (!edges.empty())
	{
		printf("--");
	}
	else
	{
		printf("\n");
	}
	//for (int i = 0; i < depth; i++)
	//{
	//	printf("|--%d
	FOR_EACH_IN_CONTAINER(iter, edges)
	{
		printf("|--%07d\n",iter->vend);
	}

	return;
}

#define PRINT_OFFSET(off) do {\
	for (int i = 0; i < off; i++)\
	{\
		printf("            ");\
	}}while(0)

void Vertex::printId() const
{
	//PRINT_OFFSET(off);
	printf("%07d", id);
	return;
}

void Vertex::getChildList(IdList &list) const
{
	FOR_EACH_IN_CONTAINER(iter, edges)
	{
		list.push_back(iter->vend);
	}
	return;
}
void Vertex::setPrivData(PrivDataUnion data)
{
	privdata = data;
	return;
}
	
PrivDataUnion Vertex::getPrivData() const
{
	return privdata;
}

DAG::DAG(const DAG &other)
{
	*this = other;
	this->rebuildIndex();
	return ;
}

DAG &DAG::operator =(const DAG &other)
{
	vertices = other.vertices;
	// rootid = other.rootid;
	roots = other.roots;

	this->rebuildIndex();
	return *this;
}


int DAG::rebuildIndex()
{
	FOR_EACH_IN_CONTAINER(vit, vertices)
	{
		vindex[vit->getId()] = vit;
	}
	return 0;
}


int DAG::addVertex(int id)
{
	VertexMapIter vi = vindex.find(id);
	if (vi != vindex.end())
	{
		return 0;
	}

	vertices.push_back(Vertex(id));
	vindex[id] = --vertices.end();
	return 0;
}

int DAG::addEdge(int vstart, int vend)
{
	VertexMapIter vsi = vindex.find(vstart);
	VertexMapIter vei = vindex.find(vend);

	if (vsi == vindex.end() || vei == vindex.end())
	{
		return -1;
	}

	vsi->second->addEdge(vstart, vend);
	vei->second->addParent(vstart);

	return 0;
}

int DAG::addDAGAsChildOf(int parent, const DAG &other)
{
	DAG tmpdag(other);

	int otherroot = other.getRoot();
	transferSubdag(tmpdag, *this, otherroot);

	int ret = addEdge(parent,otherroot);
	if (ret)
	{
		printf("Error in %s(): failed to add the other's root as child of %d.\n", __FUNCTION__, parent);
		exit(-1);
		return ret;
	}

	return 0;
}

int DAG::addDAGAsChildOf(const IdList &parents, const DAG &other)
{
	DAG tmpdag(other);

	int otherroot = other.getRoot();
	transferSubdag(tmpdag, *this, otherroot);

	int ret;
	FOR_EACH_IN_CONTAINER(pit, parents)
	{
		ret = addEdge(*pit,otherroot);
		if (ret)
		{
			printf("Error in %s(): failed to add the other's root as child of %d.\n", __FUNCTION__, pit);
			exit(-1);
			return ret;
		}
	}

	return 0;
}

Vertex *DAG::findVertex(int id) const
{
	typeof(vindex.begin()) vi = vindex.find(id);
	if (vi != vindex.end())
	{
		return &*(vi->second);
	}
	else
	{
		return NULL;
	}
}


int DAG::getVertexList(IdList &list) const
{
	FOR_EACH_IN_CONTAINER(iter, vindex)
	{
		list.push_back(iter->first);
	}
	return 0;
}

int DAG::getChildList(int id, IdList &list) const
{
	Vertex *v = findVertex(id);
	if (v == NULL)
	{
		printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, id);
		exit(-1);
	}
	v->getChildList(list);
}

//int DAG::getAllParent(int id, IdList parents)
//{
//	Vertex *v = findVertex(id);
//	if (v == NULL)
//	{
//		printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, id);
//		exit(-1);
//	}
//	return 0;
//}

int DAG::getParentNum(int id)
{
	Vertex *v = findVertex(id);
	if (v == NULL)
	{
		printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, id);
		exit(-1);
	}
	return v->getParentNum();
}

int DAG::getParentList(int id, IdList &list) const
{
	Vertex *v = findVertex(id);
	if (v == NULL)
	{
		printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, id);
		exit(-1);
	}
	return v->getParentList(list);
}


void DAG::setRoot(int id)
{
	roots.clear();
	roots.push_back(id);
	return;
}

int DAG::getRoot() const
{
	return roots.front();
}

void DAG::addToRootList(int id)
{
	DECLARE_ITERATER(pos, roots);
	pos = find(roots.begin(), roots.end(), id);
	if (pos != roots.end())
	{
		return;
	}

	roots.push_back(id);
	return;
}

void DAG::removeFromRootList(int id)
{
	DECLARE_ITERATER(pos, roots);
	pos = find(roots.begin(), roots.end(), id);
	if (pos == roots.end())
	{
		printf("%07d is not a root.", id);
		return;
	}

	roots.erase(pos);
	return;
}

void DAG::getRootList(IdList &list) const
{
	list.insert(list.end(), roots.begin(), roots.end());
	return;
}

int DAG::getVertexNum() const
{
	return vertices.size();
}

int DAG::setPrivData(int id, PrivDataUnion data)
{
	Vertex *v = findVertex(id);
	if (v == NULL)
	{
		printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, id);
		return 1;
	}

	v->setPrivData(data);
	return 0;
}

PrivDataUnion DAG::getPrivData(int id) const
{
	Vertex *v = findVertex(id);
	if (v == NULL)
	{
		printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, id);
		return PrivDataUnion();
	}

	return v->getPrivData();
}

void DAG::printSubdag(const Vertex &v, int depth) const
{
	IdList vlist;

	v.printId();

	v.getChildList(vlist);

	if (!vlist.empty())
	{
		printf("--");
	}
	
	FOR_EACH_IN_CONTAINER(iter, vlist)
	{
		if (iter != vlist.begin())
		{
			PRINT_OFFSET(depth);
			printf("         "); // 7 + 2
		}
		printf("|--");
		Vertex *vchild = findVertex(*iter);
		if (vchild == NULL)
		{
			printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, *iter);
			exit(-1);
		}
		printSubdag(*vchild, depth+1);
	}

	if (vlist.empty())
	{
		printf("\n");
	}
}


void DAG::printSubdag(const Vertex &v, int depth, PrivDataFn fn) const
{
	IdList vlist;
	int layeroff = 0;

	v.printId();
	printf("(");
	int privlen = fn(v.getPrivData());
	printf(")");
	layeroff += 2 + privlen;

	v.getChildList(vlist);

	if (!vlist.empty())
	{
		printf("--");
	}

	layeroff += 9;
	string loffsp(layeroff, ' ');
	
	string doffsp(depth * (3 + layeroff), ' ');
	
	FOR_EACH_IN_CONTAINER(iter, vlist)
	{
		if (iter != vlist.begin())
		{
			printf("%s", doffsp.c_str());
			printf("%s", loffsp.c_str()); // 7 + 2
		}
		printf("|--");
		Vertex *vchild = findVertex(*iter);
		if (vchild == NULL)
		{
			printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, *iter);
			exit(-1);
		}
		printSubdag(*vchild, depth+1, fn);
	}

	if (vlist.empty())
	{
		printf("\n");
	}
}


void DAG::print() const
{
	FOR_EACH_IN_CONTAINER(idit, roots)
	{
		Vertex *v = findVertex(*idit);
		assert(v);

		printSubdag(*v, 0);
	}
	return;
}

void DAG::print(int id) const
{
	Vertex *v = findVertex(id);
	if (v == NULL)
	{
		printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, id);
		return;
	}
	
	printSubdag(*v, 0);
	return;
}

void DAG::print(PrivDataFn fn) const
{
	FOR_EACH_IN_CONTAINER(idit, roots)
	{
		int id = *idit;

		Vertex *v = findVertex(id);
		if (v == NULL)
		{
			printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, id);
			return;
		}

		printSubdag(*v, 0, fn);
	}
	return;
}

void DAG::print(int id, PrivDataFn fn) const
{
	Vertex *v = findVertex(id);
	if (v == NULL)
	{
		printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, id);
		return;
	}
	
	printSubdag(*v, 0, fn);
	return;
}

void DAG::printVertexes(PrivDataFn fn) const
{
	if (fn == NULL)
	{
		FOR_EACH_IN_CONTAINER(iter, vindex)
		{
			iter->second->printId();
		}
	}
	else
	{
		FOR_EACH_IN_CONTAINER(iter, vindex)
		{
			iter->second->printId();
			fn(iter->second->getPrivData());
		}
	}
	return;
}

void DAG::printEdges() const
{
	FOR_EACH_IN_CONTAINER(iter, vindex)
	{
		IdList children;
		iter->second->getChildList(children);
		FOR_EACH_IN_CONTAINER(chiter, children)
		{
			iter->second->printId();
			printf(", %07d\n", *chiter);
		}
	}
	return;
}

int DAG::removeVertex(int id)
{
	//remove all edges from parents?
	Vertex *v = findVertex(id);
	if (v == NULL)
	{
		printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, id);
		exit(1);
	}

	IdList parents;
	v->getParentList(parents);
	FOR_EACH_IN_CONTAINER(iter, parents)
	{
		removeEdge(*iter, id);
	}

	VertexMapIter miter = vindex.find(id);
	if (miter == vindex.end())
	{
		return 1;
	}

	VertexIter pos = miter->second;
	vertices.erase(pos);
	return 0;
}

int DAG::removeEdge(int idstart, int idend)
{
	Vertex *v = findVertex(idstart);
	if (v == NULL)
	{
		printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, idstart);
		exit(1);
	}
	v->removeEdge(idend);

	v = findVertex(idend);
	if (v == NULL)
	{
		printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, idend);
		exit(1);
	}
	v->removeParent(idstart);

	return 0;
}


int DAG::removeSubdag(int rootid, DAG &subdag)
{
	//check all children not have multiple parents
	if (!isSubdagTree(rootid))
	{
		assert(0);
		return 1;
	}

	IdList parents;

	Vertex *v = findVertex(rootid);
	if (v == NULL)
	{
		printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, rootid);
		exit(1);
	}

	v->getParentList(parents);
	FOR_EACH_IN_CONTAINER(iter, parents)
	{
		removeEdge(*iter, rootid);
	}
	//removeVertex(rootid);

	transferSubdag(*this, subdag, rootid);
	subdag.setRoot(rootid);

	return 0;
}

int DAG::transferSubdag(DAG &src, DAG &dst, int id)
{
	//move all vertices in src subdag into a dst subdag
	VertexMapIter miter = src.vindex.find(id);
	if (miter == src.vindex.end())
	{
		return 1;
	}
	VertexIter pos = miter->second;

	IdList children;
	pos->getChildList(children);

	FOR_EACH_IN_CONTAINER(iter, children)
	{
		transferSubdag(src, dst, *iter);
	}

	dst.vertices.splice(dst.vertices.end(), src.vertices, pos);
	dst.vindex[id] = --dst.vertices.end();
	src.vindex.erase(id);

	return 0;
}

int DAG::transferNode(DAG &src, DAG &dst, VertexIter id)
{
}

bool DAG::isSubdagTree(int rootid)
{
	IdList children;
	Vertex *v = findVertex(rootid);
	if (v == NULL)
	{
		printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, rootid);
		exit(1);
	}

	v->getChildList(children);
	FOR_EACH_IN_CONTAINER(iter, children)
	{
		if (!isSubdagTree(*iter))
		{
			return false;
		}
	}
	return true;
}
	


};


