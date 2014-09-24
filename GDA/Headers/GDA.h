#ifndef GDA_H
#define GDA_H

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
#include <stdlib.h>

using namespace std;

typedef vector < long double > vld;	//vector of long double

class GDA
{
private:
//Variables to store the original input data
	vld b;	//holds <b1, b2, b3, ..., bn>
	long double Ca;	//holds the desired CF of the biofluid
	int d;	//holds the accuracy level of Ca.

//Processed data
	vld B;	//holds <B1, B2, B3, ..., Bn> of <B1/2^d, B2/2^d, B3/2^d, ..., Bn/2^d>
	long double T;

	vector < vld > W;
	vld Q;	//Set Q to store all the numbers of W except the last entry which corresponds to the buffer
	vld w;	//holds the weight
	vector < vld > R;	//Subset-Sum
	vector < vector < int > > Rindex;
	vector < vld > validR;	//valid Subset-Sum
	vector < vector < int > > validRindex;

	//For optimal condition - minimize height (d')
	vector < int > d1_validR;	//To hold the 1st optimal condition d' for all valid sets in R
	vector < vld > R_Optimal_d1;	//subsets with most optimal d'

	//For optimal condition - minimize # of non-leaf nodes (m)
	vector < int > m_validR;	//To hold the 2nd optimal condition m for all valid sets in R
	vector < vld > R_Optimal_m;	//subsets with most optimal m

public:
	GDA()
	{
		T=0;//cdcurtis
		d=0;
		Ca=0;
	}

	//Using the function findSubset to find the subset from Q such that sum == T +/- 0.5
	void findSubset(long double *p, int *Qindex, int index, int low, int high, long double sum);

	static DagGen* RunGDA(int argc, char* argv[]);
};

#endif
