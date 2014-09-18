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
#include "../../Rational/Headers/Rational.h"
//#include "../../DagGen/Headers/DagGen.h"
#include "../../BinSearch/Headers/Griffith_dilution.h"
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

	//move to common
	//Vertex2* addVertex(DagGen & dag, VertexType, std:: string, DiluteDroplet* dh, DiluteDroplet* dl, int dropsAvail, int dropsNeeded);
	//void outputVertices();
	//void outputEdges(DagGen & dag);
	//void outputAvailableDroplets(vector<DiluteDroplet*> AvailableDroplets);
	//void outputMixSplits(vector<MixOp*> Mixes);
	//void clearDropletCounts(vector<DiluteDroplet*> AvailableDroplets);
	//void adjustCounts(pair <vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals);
//int findVertex(int vertexID);
//void expander(DagGen & dag, VertexCounts* VC);
//void assignVertCounts(DagGen & dag);
//int findCounter(VertexCounts* VC, Vertex2* vertex);
//string findLabel(Vertex2* vertex);
//void edgeCorrection(DagGen& dag, vector< pair <Vertex2*, Vertex2*> > allAddedPairs);
//void addWaste(DagGen & dag);
//Rational FindRange(DiluteDroplet* input, DiluteDroplet* buffer); //dont need for roy moved to griff
//Rational GetConcentrate(DiluteDroplet* node); //possibly useful
//void SetConcentrate(DiluteDroplet* node, double ConcNum, double ConcDenom); //possibly useful
//Rational GetDesiredConcentrate(DiluteDroplet* node); //returns c from variable list
//Rational GetTolerance(DiluteDroplet* node); //return
//DiluteDroplet* CreateDroplet(Rational Concentration);
//void calcNumDropsFin(pair <vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals);
//void DeleteM(vector<MixOp*> M);
//void DeleteD(vector<pair<DiluteDroplet*, DiluteDroplet*> > D);
//Rational GetUpperBound();
//Rational GetLowerBound();
//Rational SetLowerBound(DiluteDroplet*);
//Rational SetUpperBound(DiluteDroplet*);
//vector<pair<DiluteDroplet*, DiluteDroplet*> > GetDropletSet();
//vector<pair<DiluteDroplet*, DiluteDroplet*> > GetMixSet();

//TODO:: Move this to common and call createDagRoy
//VertexCounts* CreateDag(DagGen & Dag, pair<vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals);


//vector<Edge*> findOutgoingEdges(DagGen & dag, int Vid); //moved to dagGen

//void expandDag(DagGen & dag, VertexCounts* VC); //no longer needed remove


	//methods
	DiluteRet* PerformDilution(DiluteDroplet* di,DiluteDroplet* db, double tolerance, double DesiredConcentrate, int num_ops);
	VertexCounts* CreateDag(DagGen & Dag, pair<vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals);
	pair<VertexCounts*, DagGen*> CreateDag_IDMAHelper(DagGen & Dag, pair<vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals, DagGen* M);
	DagGen* CreateIDMADag(VertexCounts* VC, DagGen* M);
	static DagGen RoyDilute_Process(int argc, char** argv);
	static DiluteRet* RoyDilute_IDMA(DiluteDroplet * L, DiluteDroplet* R, double tol, double Ct, int n);
	static DiluteRet* populateIDMA_M(double tolerance, double DesiredConcentrate, DagGen* M, int num_ops);
	//TODO::move expander back here. Makes more sense
	//Getters and Setters



 //moved to rational remove
	/*static Rational FloatToRat(double value, int num_ops);
	static Rational ConctoRat(double Concentration, int num_ops);
	static Rational CalcMixConc(DiluteDroplet* MixCl, DiluteDroplet* MixCh); 
	static long double RattoFloat(Rational Concentration); 
	*/
};

#endif /* ROY_DILUTION_H_ */
