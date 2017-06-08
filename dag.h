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

typedef list<int> IdList;

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
	union DataUnion
	{
		long long dlonglong;
		long dlong;
		int dint;
		float dfloat;
		double ddouble;

		DataUnion();
	} _data;

	long long &dlonglong;
	long &dlong;
	int &dint;
	float &dfloat;
	double &ddouble;
	shared_ptr<void> dptr;

	PrivDataUnion();
	PrivDataUnion(const PrivDataUnion &);
	PrivDataUnion &operator =(const PrivDataUnion &o);
	~PrivDataUnion();
};

class Vertex
{
private:
	int id;
	// used when decendants are pruned, saves all decendants' id's
	// to consider convenience, the vertex itself also counted
	// there may exist recursive subkey
	std::string subkey;
	IdList children;
	IdList parents;
	PrivDataUnion privdata;
public:
	Vertex(int id);
	Vertex(const Vertex &other);

	int getId() const;
	void print(int depth) const;
	void printId() const;
	void addChild(int id);
	void addParent(int id);
	int removeChild(int parent);
	int removeParent(int parent);
	int getParentNum() const;
	int getParentList(IdList &list) const;
	int getChildNum() const;
	void getChildList(IdList &list) const;

	void reverse();

	void setPrivData(const PrivDataUnion &data);
	PrivDataUnion &getPrivData();
	const PrivDataUnion &getPrivData() const;

	void setSubkey(const std::string &key);
	const std::string &getSubkey() const;
	std::string &getSubkey();
	void clearSubkey();
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

	bool reversed;

protected:
	int rebuildIndex();

	static int transferSubdag(DAG &src, DAG &dst, int id);

	//not using
	static int transferNode(DAG &src, DAG &dst, VertexIter ind);


public:
	DAG() : reversed(false) {};
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

	void setSubkey(int id, const std::string &subkey);
	const std::string &getSubkey(int id) const;
	std::string &getSubkey(int id);
	void clearSubkey(int id);

	int addDAGAsChildOf(int parent, const DAG &other);
	int addDAGAsChildOf(const IdList &parents, const DAG &other);
	int transplantAsChildOf(const IdList &parents, DAG &other);

	Vertex *findVertex(int id) const; //NULL if not found
	bool checkVertex(int id) const;

	int getVertexList(IdList &list) const;
	int getVertexString(string &str) const;

	int getMultiParentVertices(IdList &list) const;

	void setSingleRoot(int id);
	int getFirstRoot() const;
	void addToRootList(int id);
	void removeFromRootList(int id);
	int generateRoots();

	int getVertexNum() const;
	int getEdgeNum() const;
	int getRootNum() const;

	int getChildNum(int id) const;
	int getParentNum(int id);

	bool isRoot(int id) const;
	bool isLeaf(int id) const;
	bool isInternal(int id) const;

	void getRootList(IdList &list) const;
	int getParentList(int id, IdList &list) const;
	int getChildList(int id, IdList &list) const;

	void print() const;
	void print(const IdList &ids) const;
	void print(int id) const;
	void print(ConstPrivDataFn fn) const;
	void print(int id, ConstPrivDataFn fn) const;
	void printSubdag(const Vertex &v, int depth) const;
	void printSubdag(const Vertex &v, int depth, ConstPrivDataFn fn) const;
	void printVertexes(ConstPrivDataFn fn = 0) const;
	void printEdges() const;

	// reverse all directions
	void reverse();

	//int merge(const DAG &other, DAG &dest); //

	int removeVertex(int id);

	int removeEdge(int idstart, int idend);

	// remove the subdag with root whose id is rootid
	// the subdag must be a tree
	int removeSubdagRootAt(int rootid, DAG &subdag);

	// remove the subdag with given roots
	int removeSubdagRootAt(const IdList &roots, DAG &subdag);

	// remove the exact subdag
	// dag remain unchanged if some node is missing
	int removeSubdag(const DAG &subdag);

	//check if the subdag with rootid is a tree
	bool isSubdagTree(int rootid);

	DAG &operator =(const DAG &other);

};


};

#endif /* __DAG_H_ */
