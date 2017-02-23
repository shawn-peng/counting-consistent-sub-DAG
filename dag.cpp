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
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

using namespace std;

namespace NS_DAG
{

PrivDataUnion::DataUnion::DataUnion() :
	dlonglong(0)
{}

//PrivDataUnion::DataUnion::~DataUnion()
//{}

PrivDataUnion::PrivDataUnion() :
	type(DT_EMPTY),
	dlonglong(_data.dlonglong),
	dlong(_data.dlong),
	dint(_data.dint),
	dfloat(_data.dfloat),
	ddouble(_data.ddouble),
	dptr()
	//dptr(_data.dptr)
{}

PrivDataUnion::PrivDataUnion(const PrivDataUnion &o) :
	type(DT_EMPTY),
	dlonglong(_data.dlonglong),
	dlong(_data.dlong),
	dint(_data.dint),
	dfloat(_data.dfloat),
	ddouble(_data.ddouble),
	dptr()
{
	*this = o;
}

PrivDataUnion &PrivDataUnion::operator =(const PrivDataUnion &o)
{
	//if (type == DT_POINTER)
	//{
	//	//(*destroy)(this);
	//	dptr = NULL;
	//}
	type = o.type;
	_data = o._data;
	dptr = o.dptr;
	//switch (type)
	//{
	//	case DT_EMPTY:
	//		dlonglong = 0;
	//		break;
	//	case DT_LONGLONG:
	//		dlonglong = o.dlonglong;
	//		break;
	//	case DT_LONG:
	//		dlong = o.dlong;
	//		break;
	//	case DT_INT:
	//		dint = o.dint;
	//		break;
	//	case DT_FLOAT:
	//		dfloat = o.dfloat;
	//		break;
	//	case DT_DOUBLE:
	//		ddouble = o.ddouble;
	//		break;
	//	case DT_POINTER:
	//		dptr = o.dptr;
	//		break;
	//	default:
	//		assert(0);
	//		break;
	//}
}

PrivDataUnion::~PrivDataUnion()
{
	//if (type == DT_POINTER)
	//{
	//	//(*destroy)(this);
	//}
}

Vertex::Vertex(int vid) : id(vid)
{
}

Vertex::Vertex(const Vertex &other)
{
	*this = other;
	//privdata.dptr = NULL;
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
void Vertex::setPrivData(const PrivDataUnion &data)
{
	privdata = data;
	return;
}

PrivDataUnion &Vertex::getPrivData()
{
	return privdata;
}

const PrivDataUnion &Vertex::getPrivData() const
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

int DAG::transplantAsChildOf(const IdList &parents, DAG &other)
{
	int otherroot = other.getRoot();
	transferSubdag(other, *this, otherroot);

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
	auto vi = vindex.find(id);
	if (vi != vindex.end())
	{
		return &*(vi->second);
	}
	else
	{
		return NULL;
	}
}

bool DAG::checkVertex(int id) const
{
	Vertex *v = findVertex(id);
	return v != NULL;
}

int DAG::getVertexNum() const
{
	return vertices.size();
}

int DAG::getVertexList(IdList &list) const
{
	FOR_EACH_IN_CONTAINER(iter, vindex)
	{
		list.push_back(iter->first);
	}
	return 0;
}

int DAG::getVertexString(string &str) const
{
	ostringstream oss;
	FOR_EACH_IN_CONTAINER(iter, vindex)
	{
		oss << iter->first << ' ';
	}
	str = string(move(oss.str()));
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

// assume the roots list is very small, we use a sort insert
void DAG::addToRootList(int id)
{
	//auto pos = find(roots.begin(), roots.end(), id);
	//if (pos != roots.end())
	//{
	//	return;
	//}
	if (isRoot(id))
	{
		return;
	}

	//roots.push_back(id);
	FOR_EACH_IN_CONTAINER(iter, roots)
	{
		if (id < *iter)
		{
			roots.insert(iter, id);
			return;
		}
	}
	roots.push_back(id);
	return;
}

void DAG::removeFromRootList(int id)
{
	auto pos = find(roots.begin(), roots.end(), id);
	if (pos == roots.end())
	{
		printf("%07d is not a root.\n", id);
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

bool DAG::isRoot(int id) const
{
	auto pos = find(roots.begin(), roots.end(), id);
	return pos != roots.end();
}

int DAG::generateRoots()
{
	int cnt = 0;
	FOR_EACH_IN_CONTAINER(iter, vindex)
	{
		if (iter->second->getParentNum() == 0)
		{
			addToRootList(iter->first);
			cnt++;
		}
	}
	return cnt;
}


void DAG::setPrivData(const PrivDataUnion &data)
{
	privdata = data;
	return;
}

PrivDataUnion &DAG::getPrivData()
{
	return privdata;
}

const PrivDataUnion &DAG::getPrivData() const
{
	return privdata;
}

int DAG::setPrivData(int id, const PrivDataUnion &data)
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

PrivDataUnion &DAG::getPrivData(int id)
{
	Vertex *v = findVertex(id);
	if (v == NULL)
	{
		printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, id);
		exit(1);
	}

	return v->getPrivData();
}

const PrivDataUnion &DAG::getPrivData(int id) const
{
	const Vertex *v = findVertex(id);
	if (v == NULL)
	{
		printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, id);
		exit(1);
	}

	return v->getPrivData();
}

void DAG::copyVertexPrivData(const DAG &other)
{
	FOR_EACH_IN_CONTAINER(vit, vertices)
	{
		int id = vit->getId();
		vit->setPrivData(other.getPrivData(id));
	}
	return;
}

void DAG::clearVertexPrivData()
{
	FOR_EACH_IN_CONTAINER(vit, vertices)
	{
		int id = vit->getId();
		vit->setPrivData(PrivDataUnion());
	}
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


void DAG::printSubdag(const Vertex &v, int depth, ConstPrivDataFn fn) const
{
	IdList vlist;
	int layeroff = 0;

	v.printId();
	printf("(");
	int privlen = fn(&v.getPrivData());
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

void DAG::print(ConstPrivDataFn fn) const
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

void DAG::print(int id, ConstPrivDataFn fn) const
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

void DAG::printVertexes(ConstPrivDataFn fn) const
{
	if (fn == NULL)
	{
		FOR_EACH_IN_CONTAINER(iter, vindex)
		{
			iter->second->printId();
			printf(", ");
		}
	}
	else
	{
		FOR_EACH_IN_CONTAINER(iter, vindex)
		{
			iter->second->printId();
			printf("[");
			fn(&iter->second->getPrivData());
			printf("], ");
		}
	}
	printf("\n");
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
	VertexMapIter miter = vindex.find(id);
	if (miter == vindex.end())
	{
		return 1;
	}

	Vertex *v = &*miter->second;
	if (v == NULL)
	{
		printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, id);
		exit(1);
	}

	//remove all edges from parents?
	IdList parents;
	v->getParentList(parents);
	FOR_EACH_IN_CONTAINER(pit, parents)
	{
		Vertex *vp = findVertex(*pit);
		vp->removeEdge(id);
	}

	//remove from roots
	roots.remove(id);

	//remove records from children, and also try set children roots
	IdList children;
	v->getChildList(children);
	FOR_EACH_IN_CONTAINER(chit, children)
	{
		int child = *chit;
		Vertex *vch = findVertex(child);
		if (vch == NULL)
		{
			printf("Error in %s(): id:%07d not found.\n", __FUNCTION__, child);
			exit(1);
		}
		vch->removeParent(id);
		// check parent
		if (vch->getParentNum() == 0)
		{
			// become root
			addToRootList(child);
		}
	}

	VertexIter pos = miter->second;
	vertices.erase(pos);

	vindex.erase(miter);

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

	// check parent
	if (v->getParentNum() == 0)
	{
		// become root
		addToRootList(idend);
	}

	return 0;
}


int DAG::removeSubdagRootAt(int rootid, DAG &subdag)
{
	//check all children not have multiple parents
	//if (!isSubdagTree(rootid))
	//{
	//	assert(0);
	//	return 1;
	//}

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

	transferSubdag(*this, subdag, rootid);
	removeFromRootList(rootid);// this node must have become root when we remove edges
	subdag.addToRootList(rootid);

	return 0;
}

int DAG::removeSubdagRootAt(const IdList &subroots, DAG &subdag)
{
	FOR_EACH_IN_CONTAINER(iter, subroots)
	{
		removeSubdagRootAt(*iter, subdag);
	}

	return 0;
}


// dag remain unchanged if some node is missing
int DAG::removeSubdag(const DAG& subdag)
{
	//check all nodes present in current dag
	IdList nodes;
	subdag.getVertexList(nodes);
	FOR_EACH_IN_CONTAINER(viter, nodes)
	{
		if (!checkVertex(*viter))
		{
			printf("%07d in the subdag to be removed is missing.\n", *viter);
			return -1;
		}
	}

	IdList roots;
	getRootList(roots);
	FOR_EACH_IN_CONTAINER(rootit, roots)
	{
		if (subdag.checkVertex(*rootit))
		{
			removeFromRootList(*rootit);
		}
	}

	FOR_EACH_IN_CONTAINER(viter, subdag.vertices)
	{
		int id = viter->getId();
		const Vertex *v = findVertex(id);
		IdList children;
		v->getChildList(children);
//		FOR_EACH_IN_CONTAINER(chit, children)
//		{
//			int child = *chit;
//			if (!subdag.checkVertex(child))
//			{
//				removeEdge(id, child);
//				Vertex *vchild = findVertex(child);
//				if (vchild->getParentNum() == 0)
//				{
//					addToRootList(child);
//				}
//			}
//		}

		removeVertex(id);
	}

	return 0;
}

int DAG::transferSubdag(DAG &src, DAG &dst, int id)
{
	//move all vertices in src subdag into a dst subdag
	VertexMapIter miter = src.vindex.find(id);
	if (miter == src.vindex.end())
	{
		// already moved (maybe check its presence in the dst)
		assert(dst.checkVertex(id));
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


