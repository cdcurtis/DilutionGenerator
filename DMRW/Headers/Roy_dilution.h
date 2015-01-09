/*
 * Roy_dilution.h
 *
 *  Created on: Apr 23, 2013
 *      Author: Kenneth O'Neal
 */

#ifndef ROY_DILUTION_H_
#define ROY_DILUTION_H_

using namespace std;
#include <iostream>
//#include "../../Rational/Headers/Rational.h"
//#include "../../DagGen/Headers/DagGen.h"
//////////#include "../../BinSearch/Headers/Griffith_dilution.h"
#include "../../RoyCommon/Headers/common.h"
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <queue>
#include <sstream>
#include <fstream>
#include <cmath>

bool CompareuID(DiluteDroplet* A, DiluteDroplet* B);

class RoyDilute : public Dilute //??? check if this should be entity(may be nothing)
{
	vector<Vertex2*> vertices;

public:
	~RoyDilute()
	{
		for(unsigned i = 0; i < vertices.size(); i++)
		{
			delete vertices[i];
		}
	}

	//methods
	DiluteRet* PerformDilution(DiluteDroplet* di,DiluteDroplet* db, double tolerance, double DesiredConcentrate, int num_ops);
	VertexCounts* CreateDag(DagGen & Dag, pair<vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals);
	pair<VertexCounts*, DagGen*> CreateDag_IDMAHelper(DagGen * Dag, pair<vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals, DagGen* M);
	DagGen* CreateIDMADag(VertexCounts* VC, DagGen* M);
	static pair< int, int > RoyDilute_Process(DiluteDroplet* db, DiluteDroplet* di, int num_ops, double DesiredConcetrate, double tolerance, DagGen*);
	static DiluteRet* RoyDilute_IDMA(DiluteDroplet * L, DiluteDroplet* R, double tol, double Ct, int n);
	static DiluteRet* populateIDMA_M(double tolerance, double DesiredConcentrate, DagGen* M, int num_ops);
	//TODO::move expander back here. Makes more sense
	//Getters and Setters
	static void RunDMRW(int argc, char** argv, DagGen*);
};

#endif /* ROY_DILUTION_H_ */
