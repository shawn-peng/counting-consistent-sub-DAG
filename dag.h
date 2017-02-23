//Shawn Peng
//This is the DAG data structure header file, defining the DAG class,
//the Vertex class and the Edge class.
//This DAG data structure uses "map" to index the vertices by id, and each
//vertex object saves the edges starting from it and its parent's (or parents')
//id. It also saves an privdata which is defined and used by the user.


#ifndef __DAG_H_
#define __DAG_H_

#include <string>
#include <vector>
#include <list>
#include <map>
#include <memory>

using namespace std;

namespace NS_DAG
{

struct Edge
{
	int vstart;
	int vend;
	Edge(int vs,int ve):vstart(vs), vend(ve){}
};

typedef list<int> IdList;

typedef list<Edge> EdgeList;

enum DataTypeEnum
{
	DT_EMPTY = 0,
	DT_LONGLONG,
	DT_LONG,
	DT_INT,
	DT_FLOAT,
	DT_DOUBLE,
	DT_POINTER,
};

struct PrivDataUnion;

typedef int (*PrivDataFn)(PrivDataUnion *);

typedef int (*ConstPrivDataFn)(const PrivDataUnion *);

struct PrivDataUnion
{
	DataTypeEnum type;
	//PrivDataFn destroy;
	union DataUnion
	{
		long long dlonglong;
		long dlong;
		int dint;
		float dfloat;
		double ddouble;
		//shared_ptr<void> dptr;

		DataUnion();
		//~DataUnion();
	} _data;

	long long &dlonglong;
	long &dlong;
	int &dint;
	float &dfloat;
	double &ddouble;
	shared_ptr<void> dptr;
	//shared_ptr<void> &dptr;
	//void *&dptr;

	PrivDataUnion();
	PrivDataUnion(const PrivDataUnion &);
	PrivDataUnion &operator =(const PrivDataUnion &o);
	~PrivDataUnion();
};

class Vertex
{
private:
	int id;
	list<Edge> edges;
	typedef EdgeList::iterator EdgeIter;
	IdList parents;
	PrivDataUnion privdata;
public:
	Vertex(int id);
	Vertex(const Vertex &other);

	int getId() const;
	void print(int depth) const;
	void printId() const;
	void addEdge(int vstart, int vend);
	void addParent(int id);
	int removeEdge(int vend);
	int removeParent(int parent);
	int getParentNum();
	int getParentList(IdList &list) const;
	void getChildList(IdList &list) const;

	void setPrivData(const PrivDataUnion &data);
	PrivDataUnion &getPrivData();
	const PrivDataUnion &getPrivData() const;
};

typedef list<Vertex> VertexList;
typedef VertexList::iterator VertexIter;

class DAG
{
private:
	VertexList vertices; //list of vertices
	map<int, VertexIter> vindex; //index to find vertex with certain id in the list
	typedef map<int, VertexIter> VertexMap;
	typedef VertexMap::iterator VertexMapIter;

	// int rootid;
	IdList roots;

	PrivDataUnion privdata;

protected:
	int rebuildIndex();

	static int transferSubdag(DAG &src, DAG &dst, int id);

	//not using
	static int transferNode(DAG &src, DAG &dst, VertexIter ind);


public:
	DAG() {};
	DAG(const DAG &other);
	int addVertex(int id);

	int addEdge(int vstart, int vend);

	void setPrivData(const PrivDataUnion &data);
	PrivDataUnion &getPrivData();
	const PrivDataUnion &getPrivData() const;

	int setPrivData(int id, const PrivDataUnion &data);
	PrivDataUnion &getPrivData(int id);
	const PrivDataUnion &getPrivData(int id) const;
	void copyVertexPrivData(const DAG &other);
	void clearVertexPrivData();

	int addDAGAsChildOf(int parent, const DAG &other);
	int addDAGAsChildOf(const IdList &parents, const DAG &other);
	int transplantAsChildOf(const IdList &parents, DAG &other);

	//NULL if not found
	Vertex *findVertex(int id) const;
	bool checkVertex(int id) const;

	int getVertexNumber() const;
	int getVertexList(IdList &list) const;
	int getVertexString(string &str) const;

	int getChildList(int id, IdList &list) const;

	//int getAllParent(int id, IdList parents);
	int getParentNum(int id);
	int getParentList(int id, IdList &list) const;

	void setRoot(int id);
	int getRoot() const;
	void addToRootList(int id);
	void removeFromRootList(int id);
	void getRootList(IdList &list) const;
	bool isRoot(int id) const;
	int generateRoots();

	int getVertexNum() const;

	void print() const;
	void print(const IdList &ids) const;
	void print(int id) const;
	void print(ConstPrivDataFn fn) const;
	void print(int id, ConstPrivDataFn fn) const;
	void printSubdag(const Vertex &v, int depth) const;
	void printSubdag(const Vertex &v, int depth, ConstPrivDataFn fn) const;
	void printVertexes(ConstPrivDataFn fn = 0) const;
	void printEdges() const;

	//int merge(const DAG &other, DAG &dest); //

	int removeVertex(int id);
	int removeEdge(int idstart, int idend);
	// remove the subdag with root whose id is rootid
	// the subdag must be a tree
	int removeSubdagRootAt(int rootid, DAG &subdag);
	// remove the subdag with given roots
	int removeSubdagRootAt(const IdList &roots, DAG &subdag);

	// remove the exact subdag
	int removeSubdag(const DAG &subdag);

	//check if the subdag with rootid is a tree
	bool isSubdagTree(int rootid);

	DAG &operator =(const DAG &other);

};


};

#endif /* __DAG_H_ */
