#ifndef __WARA__H_
#define __WARA__H_

#include "WRemia.h"

typedef vector< pair<W_node*, W_node*> > RCP;	//RCP - Replacement Candidate Pair

class Wara
{
private:
	bool found;	//flag being used to find the parent node for a given node. Part of maximal droplet shraing function logic
	int uid;
	string b, r, w, m, s, o;
	int b_count, r_count, o_count;
	vector < W_node* > SMT;	//Forest to hold the mixing tree for each PCV.
public:
	int s_count, m_count, w_count;
	Wara();
	W_node* Find_parent(W_node *T, W_node *p, W_node *m);
	bool checkIfUniqueNode(priority_queue<W_node*, vector<W_node*>, W_comparator> Q, W_node *n);
	void generate_stack(W_node *T, priority_queue<W_node*, vector<W_node*>, W_comparator>& sk);
	void Maximal_Droplet_Sharing();

	bool checkIfUniqueNodeByValue(priority_queue<W_node*, vector<W_node*>, W_comparator> Q, W_node *n);
	void GetRCPs(vector < W_node* > G, RCP &rcp);
	int GetGain(vector < W_node* > G, W_node *x, W_node *y, int eru);
	void SetACs(RCP &rcp);
	void ResetACs(RCP &rcp);
	void GetUniqueness(RCP &rcp, vector < int >& uniq);
	bool NotInSubtree(W_node *xy, W_node *z);
	int ERU(vector < W_node* > G);
	void Droplet_Replacement(vector < W_node* >& SMT);

	Vertex* createVertex(int &count, string str,DagGen *dag, VertexType type);
	void convertDataStructureForPCV(W_node *T, DagGen *dag, stack< Vertex *>& vertices, W_node *root);
	Vertex* getCorrespondingVertex(W_node *T, stack< Vertex *>& vertices, VertexType type);
	bool checkIfUniqueNodeInStack(stack< W_node * > Q, W_node *n);
	void convertDataStructureForMixingTree(stack < W_node* > Q, DagGen *dag, stack< Vertex *>& vertices, vector < W_CV* > PCV);

	void Run_Wara(DagGen *dag, int argc, char* argv[]);
};

#endif /*__WARA__H_*/
