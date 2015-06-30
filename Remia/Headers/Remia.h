#ifndef REMIA_H
#define REMIA_H

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

struct node
{
	int id;
	stack < int > dag_uid;	//holds the UID of the corresponding node in the Dag.
	CV *cv;
	usability status;
	answer target;
	int ac;	//apprearance count of the node
	node *left;
	node *right;
};

struct comparator
{
	bool operator() ( node *i, node *j)
	{
		if ( i->cv->val < j->cv->val )
        		return false;
        	else
        		return true;
	}
 
};

class Remia 
{
private:

	string b, r, w, m, s, o;
	int b_count, r_count, w_count, m_count, s_count, o_count;
public:
	int UID;
	Remia();
	CV * extractMSNZB(CV *ccv);
	node* BuildMixingTree(CV *Ct);

	void printLevelOrder(node *root);
	bool checkIfUniqueInStack(priority_queue<node*, vector<node*>, comparator> Q, node *n);

	void BuilEDTforest(node *T, vector < node* >& F);
	void BuilEDTforest(vector < node* >& SMT, vector < node* >& F);
	Vertex* createVertex(int &count, string str,DagGen *dag, VertexType type);
	void convertDataStructureForPCV(node *T, DagGen *dag, stack< Vertex *>& vertices);
	void convertDataStructureForMixingTree(node *T, DagGen *dag, stack< Vertex *>& vertices, CV* pcv);

	static DagGen* RunRemia(std::vector<std::string> parameters);

};

#endif
