/*
 * Griffith_dilution.h
 * This code is intended to implement static binary search style
 * dag creation for droplet dilution. One new dag per dilution request.
 *
 *  Created on: Apr 19, 2013
 *      Author: Kenneth O'Neal
 */

#ifndef GRIFFITH_DILUTION_H_
#define GRIFFITH_DILUTION_H_

#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <queue>
#include <sstream>
#include <fstream>
#include <cmath>
#include "../../Rational/Headers/Rational.h"
#include "../../DagGen/Headers/DagGen.h"
#include "../../RoyCommon/Headers/common.h"

bool CompareByuID(DiluteDroplet* A, DiluteDroplet* B);

class GriffDilute //??? check if this should be entity(may be nothing)
{
	vector<Vertex2*> vertices;

public:
	~GriffDilute()
	{
		for(unsigned i =0; i< vertices.size(); ++i)
			delete vertices[i];
	}

	Vertex2* addVertex(DagGen & dag, VertexType, std:: string, DiluteDroplet* dh, DiluteDroplet* dl, int dropsAvail, int dropsNeeded);
	Rational GetConcentrate(DiluteDroplet* node);
	void SetConcentrate(DiluteDroplet* node, double ConcNum, double ConcDenom);
	Rational GetDesiredConcentrate(DiluteDroplet* node); //returns c from variable list
	Rational GetTolerance(DiluteDroplet* node);

	static Rational FloatToRat(double value, int num_ops);
	static Rational ConctoRat(double Concentration, int num_ops);
	static Rational CalcMixConc(DiluteDroplet* MixCl, DiluteDroplet* MixCh); 
	static long double RattoFloat(Rational Concentration); 
	DiluteDroplet* CreateDroplet(Rational Concentration);

	int findCounter(VertexCounts* VC, Vertex2* vertex);
	string findLabel(Vertex2* vertex);

	//output/input
	void outputVertices();
	void outputEdges(DagGen & dag);
	void outputAvailableDroplets(vector<DiluteDroplet*> AvailableDroplets);
	void outputMixSplits(vector<MixOp*> Mixes);
	void clearDropletCounts(vector<DiluteDroplet*> AvailableDroplets);

	//methods
	Rational FindRange(DiluteDroplet* input, DiluteDroplet* buffer);
	DiluteRet PerformDilution(DiluteDroplet* di, DiluteDroplet* db, double tolerance, double DesiredConcentrate, int num_ops);
	VertexCounts* CreateDag(DagGen & dag, pair< vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals);
	void calcNumDropsFin(pair <vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals);
	void expandDag(DagGen & dag, VertexCounts* VC);
	static DagGen RunGriffith(int argc, char ** argv);

	//memory management
	void DeleteM(vector<MixOp*> M);
	void DeleteD(vector<pair<DiluteDroplet*, DiluteDroplet*> > D);
};
#endif /* GRIFFITH_DILUTION_H_ */
