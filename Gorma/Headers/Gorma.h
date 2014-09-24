#ifndef GORMA_H
#define GORMA_H

#include<iostream>
#include<vector>
#include<math.h>
#include<utility>
#include<algorithm>
#include<iomanip>
#include<stack>
#include<queue>
#include <sstream>

#include "../../DagGen/Headers/DagGen.h"
#include "../../SharedData/SharedData.h"
using namespace std;


typedef vector< pair<CV*, CV*> > PrecedingPair; // - User defined data type to store the preceding pairs


struct GormaNode
{
	int id;	//stores the id of the node
	stack < int > dag_uid;	//holds the UID of the corresponding node in the Dag.
	CV *cv;	//stores the CV of the node
	PrecedingPair *PP;	//holds the preceding pairs of the node
	usability status;	//holds the usability status i.e., reusable, non_reusable, or reused
	GormaNode *left;
	GormaNode *right;
};

struct GormaTreeValue	//used to evaluate the tree
{
	int R; //# of reactants
	int B; //# of buffers
	GormaTreeValue(int r, int b): R(r), B(b) {}
};

//Function to sort the preceding pairs of a CV in ascending order of the X value of the pair in the vector
bool SortPrecedingPairs(const pair<CV*, CV*>& i, const pair<CV*, CV*>& j);

class Gorma 
{
private:
	bool found;	//flag being used to find the parent node for a given node. Part of maximal droplet shraing function logic
	int UID;	//used to assign ID's to the nodes in the tree produced by Gorma

	Gorma() : found(false), UID(0) {}
	void FindPrecedingPairs(PrecedingPair *PP, CV* pcv);
	void PrintPrecedingPairs(GormaNode *n);

	void ExtractPair(CV *l, CV *r, PrecedingPair *PP);
	GormaNode* Build_CMT(CV *cv);

	void Tree_Evaluate(GormaNode *p, GormaTreeValue *m, stack <GormaNode *>& sk);
	GormaNode* Tree_Duplicate(GormaNode *s);	//s->source, t->target
	GormaNode* CMT_Enumeration(CV *ct);
	GormaNode* Find_parent(GormaNode *T, GormaNode *p, GormaNode *m);
	void PrintTreePostOrder(GormaNode* p, int indent);
	void printLevelOrder(GormaNode *root);
	void generate_stack(GormaNode *T, stack <GormaNode *>& sk);
	void Maximal_Droplet_Sharing(GormaNode *T);
	void delete_subtree(GormaNode *y);
	void mutable_nodes(GormaNode *T, stack <GormaNode *>& sk);
	GormaNode* last_visited_mutable_node(GormaNode *T);
	bool getAncestors(GormaNode *T, GormaNode *v, stack <GormaNode *>& An);
	bool checkIfLeftChild(GormaNode *T, GormaNode *v);

	bool checkIfUniqueInStack(stack <GormaNode *>& An, GormaNode *n);
	void getUniqueNodes(GormaNode *T, stack <GormaNode *>& Un);

	void convertDataStructure(GormaNode *T, CV* pcv, DagGen *dag);
	void createVertex(int &count, string str,DagGen *dag, GormaNode* tmp, stack< Vertex *>& vertices, VertexType type);
	Vertex* getCorrespondingVertex(GormaNode *T, stack< Vertex *>& vertices, VertexType type);

public:
	static DagGen* RunGorma(int argc, char* []);
};

#endif
