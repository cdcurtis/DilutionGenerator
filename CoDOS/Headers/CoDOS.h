#ifndef CODOS_H
#define CODOS_H

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

typedef vector < vector < int > > Matrix;
typedef vector < int > Entry;
typedef vector< pair<int, int> > Rect_Cood;

class CoDOS
{
private:
	int d, NR;	//NR - Number of reactants
	Matrix index;
public:
	CoDOS();

	void binary(int number, vector < int >& r);
	Matrix ConstructRecipeMatrix(Entry T);

	vector < Rect_Cood > getRectangle(Matrix M);
	Rect_Cood getHighestPrecedenceRectangle(vector < Rect_Cood > rectangles, Entry W);
	Matrix CoDOS_Main(Entry T, Entry W);

	Vertex* createVertex(int count, string str,DagGen &dag, VertexType type);
	void Construct_Graph(Matrix M, DagGen &dag);

	static DagGen RunCoDOS(int argc, char* argv[]);
};

#endif
