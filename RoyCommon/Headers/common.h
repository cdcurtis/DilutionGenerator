/*
 * Common.h
 *
 *  Created on: April 23, 2014
 *      Author: Kenneth O'Neal
 */

#ifndef COMMON_H_
#define COMMON_H_

#include "../../Rational/Headers/Rational.h"
#include "../../DagGen/Headers/DagGen.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <queue>
#include <sstream>
#include <fstream>
#include <cmath>

 //TODO:: make a pure virtual class that is used as inheritance for Roy, Griffith 
 //and IDMA. 

 //TODO:: make all references to vertices

 //This will allow all vertices and edges references to be used. 

using namespace std;

struct MixOp;

struct DiluteDroplet{
	Rational Concentration;
	Rational DesiredConcentrate;
	Rational Tolerance;
	int uID; //the unique id of the droplet (corresponding the mixop#)
	int count; // the count of the droplet. How many times it is used.
	MixOp* Mixes; //Exact mix op  performed to generate this droplet.
	vector<MixOp*> Mix_Splits; //The sequence of mixops necessary to generate this droplet.
};

struct MixOp{
	pair<DiluteDroplet*, DiluteDroplet*> DropletsPreMix;
	pair<DiluteDroplet*, DiluteDroplet*> DropletsPostMix;
	int count;
};

struct Vertex2 : public Vertex {
	DiluteDroplet* dl; //SPLIT:outgoing drop low. MIX: incoming drop low
	DiluteDroplet* dh; //SPLIT:outgoing drop high MIX: incmoing drop high
	int dropsAvail;
	int dropsNeeded;

	Vertex2(VertexType t, std :: string l, int id, DiluteDroplet* dl, DiluteDroplet* dh, int dropsAvail, int dropsNeeded)
	: Vertex(t, l, id)
	{
		this->dl = dl;
		this->dh = dh;
		this->dropsAvail = dropsAvail;
		this->dropsNeeded = dropsNeeded;
	}
};

struct VertexCounts
{
	int sampleCount;
	int bufferCount;
	int mixCount;
	int splitCount;
	int wasteCount;
	DagGen* H;
};

struct DiluteRet
{
	pair<vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals;
	vector<double> endConcentration;
	int base;
	pair<DagGen*, DagGen*> populateIDMA_ret; //used to add L and R to in IDMA::LR_Calc, so that the final graph can be passed correctly to expander
	
	void deleteMixOps()
	{
		while(!DilutionVals.first.empty()) delete DilutionVals.first.back(), DilutionVals.first.pop_back();
		/*for(unsigned i = 0; i < DilutionVals.first.size(); i++)
		{
			delete DilutionVals.first[i];
		}
		return;*/
	}
	
	void deleteDiluteDroplets()
	{
		while(!DilutionVals.second.empty()) delete DilutionVals.second.back(), DilutionVals.second.pop_back();
		/*for(unsigned i = 0; i < DilutionVals.second.size(); i++)
		{
			delete DilutionVals.second[i];
		}
		return;*/
	}
};



class Dilute
{
protected:
	//variables
	vector<Vertex2*> vertices;

public:
	~Dilute()
	{
		for(unsigned i = 0; i < vertices.size(); i++)
		{
			delete vertices[i];
		}
	}

	Vertex2* addVertex(DagGen & dag, VertexType, std:: string, DiluteDroplet* dh, DiluteDroplet* dl, int dropsAvail, int dropsNeeded); //ID
	// void outputVertices(); //ID
	// void outputEdges(DagGen & dag); //ID
	static bool CompareuID(DiluteDroplet* A, DiluteDroplet* B); //RD
	int findVertex(int vertexID); //ID
	void assignVertCounts(DagGen & dag); //RD
	int findCounter(VertexCounts* VC, Vertex* vertex); //ID
	string findLabel(Vertex* vertex); //ID
	VertexCounts* findVertexCounts(DagGen*); //ID

	//Droplets
	void outputAvailableDroplets(vector<DiluteDroplet*> AvailableDroplets); //RD
	void outputMixSplits(vector<MixOp*> Mixes); //RD
	void clearDropletCounts(vector<DiluteDroplet*> AvailableDroplets); //ID
	void adjustCounts(pair <vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals); //RD
	Rational GetConcentrate(DiluteDroplet* node); //RD possibly useful
	void SetConcentrate(DiluteDroplet* node, double ConcNum, double ConcDenom); //RD possibly useful
	Rational GetDesiredConcentrate(DiluteDroplet* node); //RD returns c from variable list
	Rational GetTolerance(DiluteDroplet* node); //RD return
	DiluteDroplet* CreateDroplet(Rational Concentration); //RD
	void calcNumDropsFin(pair <vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals); //RD
	Rational CalcMixConc(DiluteDroplet* MixCl, DiluteDroplet* MixCh); //COMMON
	void DeleteM(vector<MixOp*> M); //ID
	void DeleteD(vector<pair<DiluteDroplet*, DiluteDroplet*> > D); //ID

	// Rational GetUpperBound(); //RD not impl.
	// Rational GetLowerBound(); //RD not impl.
	// Rational SetLowerBound(DiluteDroplet*); //RD not impl.
	// Rational SetUpperBound(DiluteDroplet*); //RD not impl.
	// vector<pair<DiluteDroplet*, DiluteDroplet*> > GetDropletSet(); //RD not impl.
	// vector<pair<DiluteDroplet*, DiluteDroplet*> > GetMixSet(); //RD not impl.

	//Functionality
	VertexCounts* CreateDagRoy(DagGen & Dag, pair<vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals);
	void expander(DagGen & dag, VertexCounts* VC); //RD
	void edgeCorrection(DagGen& dag, vector< pair <Vertex2*, Vertex2*> > allAddedPairs); //RD
	void addWaste(DagGen & dag); //RD



};

#endif /* COMMON_H_ */