#ifndef ISINCKU_H
#define ISINCKU_H

#include "../../Wara/Headers/Wara.h"

struct node_ISI
{
	int id;	//stores the id of the node
	int cf;	//stores the concentration factor of the node.
	float vertex_count;	//keeps count of how many vertices are needed
	node_ISI *left;
	node_ISI *right;
};

struct VerticesISI
{
	node_ISI *T;
	vector < Vertex* > vsplit;
	Vertex* vmix;
};

class ISI_NCKU
{
private:
	Wara wara;
	int a, n, d, count, k;//a-> first element, d-> difference, n->Power for the numerator with base 2 & count-> For the number of CF's & count = 2^(k+1) + 1
	int UID;
	node_ISI *root;	//holds the root of the Linear Dilution Tree
	vector < int > L;	//Target Set
	vector < node_ISI* > nodes; //holds the nodes of the Linear Dilution Tree in the post-order traversal of the binary tree
	int UB, LB, max; //UB->Upper boundary CF, LB->Lower boundary CF
	queue<node_ISI*> currentLevel, nextLevel;
	bool found;
	vector < int > av;

	//Holds the vertices for the boundary nodes. Received from Wara. The 1st entry will always hold the vertices corresponding to lower boundary nodes and the 2nd entry will always hold the vertices corresponding to upper boundary nodes
	vector < vector < Vertex* > > boundary_vertices_wara;
	int lb_index, ub_index;	//indices for LB & UB CF's

	string w, m, s, o;
	int w_count, m_count, s_count, o_count;

	vector < VerticesISI > tree_vertices;	//keeps the vertices of all the nodes in the tree

	queue<VerticesISI> currentLastLevel, nextLastLevel;

	ISI_NCKU();

	node_ISI* Find_parent(node_ISI *T, node_ISI *p, node_ISI *m);

	node_ISI* LDT(vector < int > l);
	void postorder(node_ISI *T);
	void printLevelOrder(node_ISI *R);
	VerticesISI createVertices(node_ISI *T, Vertex *vs, Vertex *vm);
	void update_vertex_count(node_ISI *T, node_ISI *currNode, int diff, float add);
	void createDag(DagGen &dag);
public:

	static DagGen RunNCKU(int argc, char* argv[]);
};


#endif
