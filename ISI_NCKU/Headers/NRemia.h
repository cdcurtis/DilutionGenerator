#ifndef NREMIA_H
#define NREMIA_H

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

using namespace std;
#include "../../DagGen/Headers/DagGen.h"
#include "../../SharedData/SharedData.h";
struct NCKU_CV //concentration value structure
{
	int num;
	int deno;
	long double val;
	NCKU_CV(int n, int d, long double v): num(n), deno(d), val(v) {}
};



struct N_node
{
	int id;
	stack < int > dag_uid;	//holds the UID of the corresponding node in the Dag.
	NCKU_CV *cv;
	usability status;
	answer target;
	int ac;	//apprearance count of the node
	N_node *left;
	N_node *right;
};

struct NCKU_comparator
{
	bool operator() ( N_node *i, N_node *j)
	{
		if ( i->cv->val < j->cv->val )
        		return false;
        	else
        		return true;
	}

};

class NCKU_Remia
{
public:
	int UID;
	NCKU_Remia();
	NCKU_CV * extractMSNZB(NCKU_CV *ccv);
	N_node* BuildMixingTree(NCKU_CV *Ct);

	void printLevelOrder(N_node *root);

	bool checkIfUniqueInStack(priority_queue<N_node*, vector<N_node*>, NCKU_comparator> Q, N_node *n);
	void BuilEDTforest(vector < N_node* >& SMT, vector < N_node* >& F);

};

#endif
