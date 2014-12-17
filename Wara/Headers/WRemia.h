/*
 * WRemia.h
 *
 *  Created on: Dec 15, 2014
 *      Author: Chris
 */

#ifndef WREMIA_H_
#define WREMIA_H_

#include<iostream>
#include<vector>
#include<math.h>
#include<utility>
#include<algorithm>
#include<iomanip>
#include<stack>
#include<queue>
#include "../../DagGen/Headers/DagGen.h"
#include <sstream>
#include <limits>
using namespace std;

struct W_CV //concentration value structure
{
	int num;
	int deno;
	long double val;
	W_CV(int n, int d, long double v): num(n), deno(d), val(v) {}
};

enum W_usability {W_reusable, W_non_reusable, W_reused, W_both_output};
enum W_answer {W_yes, W_no};

struct W_node
{
	int id;
	stack < int > dag_uid;	//holds the UID of the corresponding node in the Dag.
	W_CV *cv;
	W_usability status;
	W_answer target;
	int ac;	//apprearance count of the node
	W_node *left;
	W_node *right;
};

struct W_comparator
{
	bool operator() ( W_node *i, W_node *j)
	{
		if ( i->cv->val < j->cv->val )
        		return false;
        	else
        		return true;
	}

};

class W_Remia
{
public:
	int UID;
	W_Remia();
	W_CV * extractMSNZB(W_CV *ccv);
	W_node* BuildMixingTree(W_CV *Ct);

	void printLevelOrder(W_node *root);

	bool checkIfUniqueInStack(priority_queue<W_node*, vector<W_node*>, W_comparator> Q, W_node *n);
	void BuilEDTforest(vector < W_node* >& SMT, vector < W_node* >& F);

};


#endif /* WREMIA_H_ */
