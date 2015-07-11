#ifndef ISINCKU_H
#define ISINCKU_H

#include <iostream>
#include "NWara.h"


struct node_ISI
{
	int id;	//stores the id of the node
	int cf;	//stores the concentration factor of the node.
	float vertex_count;	//keeps count of how many vertices are needed
	node_ISI *left;
	node_ISI *right;
};

struct vertices
{
	node_ISI *T;
	std::vector < Vertex* > vsplit;
	Vertex* vmix;
};

class ISI_NCKU
{
private:
	NCKU_Wara wara;
	int a, n, d, count, k;//a-> first element, d-> difference, n->Power for the numerator with base 2 & count-> For the number of CF's & count = 2^(k+1) + 1
	int UID;
	node_ISI *root;	//holds the root of the Linear Dilution Tree
	std::vector < int > L;	//Target Set
	std::vector < node_ISI* > nodes; //holds the nodes of the Linear Dilution Tree in the post-order traversal of the binary tree
	int UB, LB, max; //UB->Upper boundary CF, LB->Lower boundary CF
	std::queue<node_ISI*> currentLevel, nextLevel;
	bool found;
	std::vector < int > av;


public:
	//Holds the vertices for the boundary nodes. Received from Wara. The 1st entry will always hold the vertices corresponding to lower boundary nodes and the 2nd entry will always hold the vertices corresponding to upper boundary nodes
	std::vector < std::vector < Vertex* > > boundary_vertices_wara;
	int lb_index, ub_index;	//indices for LB & UB CF's

	std::string w, m, s, o;
	int w_count, m_count, s_count, o_count;

	std::vector < vertices > tree_vertices;	//keeps the vertices of all the nodes in the tree

	std::queue<vertices> currentLastLevel, nextLastLevel;

	ISI_NCKU();

	node_ISI* Find_parent(node_ISI *T, node_ISI *p, node_ISI *m);

	node_ISI* LDT(std::vector < int > l);
	void postorder(node_ISI *T);
	void printLevelOrder(node_ISI *R);
	vertices createVertices(node_ISI *T, Vertex *vs, Vertex *vm);
	void update_vertex_count(node_ISI *T, node_ISI *currNode, int diff, float add);
	void createDag(DagGen *dag);
	void RUN_NCKU_Internal(std::vector<std::string>, DagGen *dag);
	static void RUN_NCKU(std::vector<std::string>, DagGen *dag);

};


#endif
