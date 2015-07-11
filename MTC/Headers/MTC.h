/*
 * MTC.h
 *
 *  Created on: Sep 18, 2014
 *      Author: chris
 */

#ifndef MTC_H_
#define MTC_H_

#include<iostream>
#include <fstream>
#include <stdlib.h>
#include "../../DagGen/Headers/DagGen.h"
#include "BrujinGraph.h"
#include "WeightedGraph.h"
#include "HamiltonianPath.h"

struct LKHFileMaker{
	std::string fileName;
	std::string problemFile;
	std::string parameterFile;
	std::ofstream pFileStream;

	LKHFileMaker(std::string f): fileName(f), problemFile(""), parameterFile(""){ }

	void makeATSPParameterFile(int runs, std::string tourFile = "")
	{
		std::ofstream myfile;
		parameterFile = fileName + ".par";
		problemFile = fileName + ".atsp";
		if(tourFile == "")
			tourFile = fileName + "Tour.txt";

		myfile.open(parameterFile.c_str());
		myfile<<"PROBLEM_FILE = "<<(fileName + ".atsp")<<std::endl;
		myfile<<"OUTPUT_TOUR_FILE = "<<tourFile<<std::endl;
		myfile<<"TRACE_LEVEL = 0"<<std::endl;
		myfile<<"RUNS = "<<runs<<std::endl;
		myfile.close();
	}
	void makeATSPProblemFile(int dimension){
		pFileStream.open(problemFile.c_str());
		pFileStream<<"NAME : foo\nTYPE : ATSP\nDIMENSION : "<< dimension <<"\nEDGE_WEIGHT_TYPE : EXPLICIT\nEDGE_WEIGHT_FORMAT : FULL_MATRIX\nEDGE_WEIGHT_SECTION\n";
	}

};

class MTC
{
	BrujinGraph _brujinGragh;
	WeightedGraph _weightedGraph;

	MTC() {}

	void MTCPhase1(std::vector<std::string>);
	void MTCPhase2(std::vector<std::string>, DagGen *, bool );

	int toInt(std::string);
	int getTourLength(std::string);
	std::vector<int> parseTourFile(std::string, int &);
	void parseMTCFile(std::string, BrujinGraph&, WeightedGraph&);
	std::vector<int> reorderTour(std::vector<int>);
	void CreateDag(std::vector<int>, WeightedGraph, DagGen*, bool);
	std::vector<int> getNamedNodes (std::vector<int>, WeightedGraph);
	void fillDagGen(std::vector<HamiltonianPath>, int, DagGen*, bool);

public:
	static void RunMTC(std::vector<std::string>, DagGen*, bool printWasteResults = false );
};

#endif /* MTC_H_ */
