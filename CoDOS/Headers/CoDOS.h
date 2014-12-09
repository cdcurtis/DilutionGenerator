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


typedef std::vector < std::vector < int > > Matrix;
typedef std::vector < int > Entry;
typedef std::vector< std::pair<int, int> > Rect_Cood;

class CoDOS
{
private:
	int d, NR;	//NR - Number of reactants
	Matrix index;
public:
	CoDOS();

	void binary(int number, std::vector < int >& r);
	Matrix ConstructRecipeMatrix(Entry T);

	std::vector < Rect_Cood > getRectangle(Matrix M);
	Rect_Cood getHighestPrecedenceRectangle(std::vector < Rect_Cood > rectangles, Entry W);
	Matrix CoDOS_Main(Entry T, Entry W);

	Vertex* createVertex(std::string str,DagGen *dag, VertexType type, int count =-1);
	void Construct_Graph(Matrix M, DagGen *dag);

	static DagGen* RunCoDOS(int argc, char* argv[]);
};

#endif
