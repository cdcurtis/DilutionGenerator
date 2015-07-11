#ifndef NRT_ISI_H
#define NRT_ISI_H

#include<iostream>
#include<vector>
#include<math.h>
#include<utility>
#include<algorithm>
#include<iomanip>
#include<stack>
#include<queue>
#include <sstream>
#include <limits>
#include "../../DagGen/Headers/DagGen.h"
#include "../../SharedData/SharedData.h"

using namespace std;

struct NRTISIcomparator
{
	bool operator() ( int i, int j)
	{
		if ( i < j )
			return false;
		else
			return true;
	}
};

typedef priority_queue<int, std::vector<int>, NRTISIcomparator> minHeap;

struct NRTISINode
{
	int id;	//stores the id of the node
	int dagID;
	int cf;	//stores the concentration factor of the node.
	int cost;	//stores the cost of the waste droplet.
	//usability status;	//holds the usability status i.e., reusable, non_reusable, or reused
	answer need;	//need is used in the removeUnnecessaryNodes() function to get rid of nodes with value = 'no'.
	answer target;	//if the current node has a target CF, this variable is set to yes else no.
	NRTISINode *left;
	NRTISINode *right;
	NRTISINode *target_path_ends;	//holds the node where the search for ancestor in the assignCost() function.
	int which_child;	//1->left child, 2->right child
	NRTISINode *which_target_path;
};

class NRT_ISI
{
private:
	bool found;
	int UID;
	int n;	//Number of bits to round off the CF values.
	minHeap T;	//sorted target set in ascending order
	minHeap nodes;	//stores the CF of the nodes in the dilution tree so that only unique nodes are generated in the tree
	NRTISINode *root;	//root of the dilution tree
	stack < NRTISINode * > DTNodes;	//holds all the nodes of the Dilution tree.
	stack < NRTISINode * > leaf;	//holds the leaf nodes. used in the assignCost() function.
	//'waste' stack holds the nodes with atleast one waste droplet except those which have one waste droplet but are target CF.
	vector< vector < NRTISINode* > > wastePair;
	stack < NRTISINode * > waste;	
	NRTISINode *waste1;
	NRTISINode *waste2;
	NRTISINode *result_leaf;
	stack < NRTISINode * > forest;	//holds the root of all the tree's.
	queue<NRTISINode*> currentLevel, nextLevel;
	stack< Vertex *> vertices;
	Vertex *vr, *vb, *vm, *vs, *vw, *vo, *vps;
	int r_count, m_count, s_count, b_count, w_count, o_count;


	NRT_ISI();

	bool checkIfUniqueNode(int cf);
	void generateDilutionTree();
	NRTISINode* Find_parent(NRTISINode *T, NRTISINode *p, NRTISINode *m);
	void removeUnnecessaryNodes();
	void assignCost();
	void selectWastePair();
	void pruning();
	Vertex* createVertex(int &count, string str,DagGen *dag, VertexType type);
	void getDag(NRTISINode *R, DagGen *dag);
	void printLevelOrder(NRTISINode *R);
public:
	static DagGen* RunNRT_ISI(std::vector<std::string>);
};


#endif
