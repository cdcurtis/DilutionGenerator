#ifndef NWARA_H
#define NWARA_H

#include "NRemia.h"
#include <iostream>

typedef std::vector< pair<N_node*, N_node*> > Ncku_RCP;	//RCP - Replacement Candidate Pair

class NCKU_Wara
{
private:
	bool found;	//flag being used to find the parent node for a given node. Part of maximal droplet shraing function logic
	std::string b, r, w, m, s;
	std::vector < N_node* > SMT;	//Forest to hold the mixing tree for each PCV.
	int lb, ub;	//lower and upper boundary CF's from ISI_NCKU
	std::vector < Vertex* > lb_vertices;
	std::vector < Vertex* > ub_vertices;
public:
	int b_count, r_count, w_count, m_count, s_count;
	int uid;
	NCKU_Wara();
	N_node* Find_parent(N_node *T, N_node *p, N_node *m);
	bool checkIfUniqueNode(priority_queue<N_node*, std::vector<N_node*>, NCKU_comparator> Q, N_node *n);
	void generate_stack(N_node *T, priority_queue<N_node*, std::vector<N_node*>, NCKU_comparator>& sk);
	void Maximal_Droplet_Sharing();

	bool checkIfUniqueNodeByValue(priority_queue<N_node*, vector<N_node*>, NCKU_comparator> Q, N_node *n);
	void GetRCPs(vector < N_node* > G, Ncku_RCP &rcp);
	int GetGain(vector < N_node* > G, N_node *x, N_node *y, int eru);
	void SetACs(Ncku_RCP &rcp);
	void ResetACs(Ncku_RCP &rcp);
	void GetUniqueness(Ncku_RCP &rcp, vector < int >& uniq);
	bool NotInSubtree(N_node *xy, N_node *z);
	int ERU(vector < N_node* > G);
	void Droplet_Replacement(vector < N_node* >& SMT);

	Vertex* createVertex(int &count, string str,DagGen *dag, VertexType type);
	void convertDataStructureForPCV(N_node *T, DagGen *dag, std::stack< Vertex *>& vertices, N_node *root);
	Vertex* getCorrespondingVertex(N_node *T, stack< Vertex *>& vertices, VertexType type);
	bool checkIfUniqueNodeInStack(stack< N_node * > Q, N_node *n);
	void convertDataStructureForMixingTree(stack < N_node* > Q, DagGen *dag, stack< Vertex *>& vertices, vector < NCKU_CV* > PCV);

	vector < vector < Vertex* > > WARA_MAIN(DagGen *dag, vector < int > argv);
};
#endif
