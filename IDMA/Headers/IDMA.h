/*
 * IDMA_dilution.h
 *
 *  Created on: Apr 10, 2014
 *      Author: Kenneth O'Neal
 */

#ifndef IDMA_H_
#define IDMA_H_

using namespace std;
#include <iostream>
//#include "../../Rational/Headers/Rational.h"
//#include "../../DagGen/Headers/DagGen.h"
#include "../../DMRW/Headers/Roy_dilution.h"
////////#include "../../BinSearch/Headers/Griffith_dilution.h"
#include "../../RoyCommon/Headers/common.h"
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <queue>
#include <sstream>
#include <fstream>
#include <cmath>


class IDMA : public Dilute
{
	vector<Vertex2*> vertices;

public:
	~IDMA()
	{
		for(unsigned i = 0; i < vertices.size(); i++)
		{
			delete vertices[i];
		}
	}

	static vector<int> CalcSkew(DagGen* M); //calculates skew of all nodes in graph
	void outputSkewVals(vector<int> skewVals);

	int PruneDMRW(DagGen* M, vector<int> skewVals, DiluteRet* valid); //resultant M is G
	int PruneDiGraph(DiluteRet* valid);

	void TrimDag(DagGen* M, int Vid, DiluteRet *valid);
	void TrimDiGraph(DiluteRet* valid, int Vid); //Vid signifies skew droplet
	pair< pair<DiluteDroplet*, DiluteDroplet*>, int> LR_Calc(DiluteRet *valid, int n, int skewID, DagGen* M, DagGen* createDagBackup, pair< pair<int, DiluteDroplet*>, pair<int, DiluteDroplet*> > parentConcentrations);
	pair< pair<DiluteDroplet*, DiluteDroplet*>, int> LR_CalcDiGraph(DiluteRet *valid, int n, int skewID, pair< pair<int, DiluteDroplet*>, pair<int, DiluteDroplet*> > parentConcentrations, DiluteDroplet* Right, DiluteDroplet* Rcopy, DiluteDroplet* Left, DiluteDroplet* Lcopy, MixOp* IDMAMix);

	pair< pair<int, DiluteDroplet*>, pair<int, DiluteDroplet*> > findSkewParentConcDag(int skewID, DiluteRet* valid, DagGen* M);
	pair< pair<int, DiluteDroplet*>, pair<int, DiluteDroplet*> > findSkewParentConcDiGraph(int skewID, DiluteRet* valid);

	double findSkewConc(int skewID, DiluteRet* valid, DagGen* M);
	vector<DiluteDroplet*> findAvailDroplets(int skewID, DiluteRet* valid, DagGen* M);
	int findSkewDroplet(double skewConc, DiluteRet* valid);

	DagGen* IntegrateDags(DagGen* M, DagGen* H); //returns union of G and DRMW return H. called M'
	void IntegrateDigraphs(DiluteRet* validM, DiluteRet* validH, DiluteDroplet* RightOrig, DiluteDroplet* LeftOrig); //upon return, M will contain the final Digraph, H will be deleted

	void DispenseCorrection(DiluteRet* validM);
	//static pair< int, int> IDMA_Process(int num_ops, double DesiredConcetrate, double tolerance); //will be the decision logic for determingin which dilution algorithm's results to use
	static pair< int, int > IDMA_Process(int num_ops, double DesiredConcetrate, double tolerance, DagGen *); //will be the decision logic for determingin which dilution algorithm's results to use
	static void IDMA_Wrapper(DiluteDroplet* db, DiluteDroplet* di, int num_ops, double DesiredConcentrate, double tolerance);

	static void RunIDMA(int argc, char **, DagGen *);



};


#endif /* IDMA_H_ */
