/*
 * MTC.cpp
 *
 *  Created on: Sep 18, 2014
 *      Author: chris
 */

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <sstream>
#include "../Headers/WeightedGraph.h"
#include "../Headers/MTC.h"

void MTC::RunMTC(std::vector<std::string> parameters, DagGen * dag, bool printWasteResults)
{

	MTC mtc;
	std::string filename = parameters[0];
	std::string basedirectory = "C:\\Users\\Chris\\Documents\\DilutionGenerator\\DilutionGenerator\\";

	mtc.MTCPhase1(parameters);

	std:: string runTravelingSales = basedirectory + "MTC\\lkh.exe " + filename +".par";
	system(runTravelingSales.c_str());


	std::vector<std::string> phase2;

	phase2.push_back(basedirectory + filename + "Tour.txt");
	phase2.push_back(basedirectory + filename + ".MTC");

	mtc.MTCPhase2(phase2, dag, printWasteResults);
}

void MTC::MTCPhase1(std::vector<std::string> parameters)
{
	std::string fileName = parameters[0];
	int SizeOfGraph = atoi(parameters[1].c_str());
   	int conCount=1;
   	_brujinGragh = BrujinGraph(SizeOfGraph);

	//n.print(std::cout);
   	_weightedGraph = WeightedGraph(&_brujinGragh);
   	_weightedGraph.addVertex(_brujinGragh.allNodes.at(0));

   	for(unsigned int i = 2; i<parameters.size(); ++i){
   		_weightedGraph.addVertex(_brujinGragh.allNodes.at(atoi(parameters[i].c_str())));
		++conCount;
   	}
	//std::cout<<"FileName:" << fileName<<std::endl;
	//std::cout<< "Starting MTC Phase1 for File: "<<fileName<<std::endl;



	_weightedGraph.calculateEdges();
    if(!_weightedGraph.isComplete())
    	std::cout<<"ERROR: Graph could not be completed.\n";

   // _weightedGraph.printDotyGraph("WeightedGraph.dot");
    //_weightedGraph.printMatrix(std::cout);

    LKHFileMaker atsp (fileName);
    atsp.makeATSPParameterFile(10);
    atsp.makeATSPProblemFile(conCount);

    _weightedGraph.printMatrix(atsp.pFileStream);
    atsp.pFileStream<<"EOF";
    atsp.pFileStream.close();

    _weightedGraph.printMTC(fileName+".MTC");
    //std::cout<< "Finished ATSP Prep on: "<<fileName<<std::endl;
    return ;
}

void MTC::MTCPhase2(std::vector<std::string> argv, DagGen* dag, bool printWasteResults)
{
	if(argv.size() < 2) {
		std::cout<< "ERROR: please make sure that the input is <TourFile> <MTCFile>"<<std::endl;
			return;
		}
		int mixSplits = -1;
		//BrujinGraph bruGraph;
		//WeightedGraph weiGraph;

		std::string tourFile = argv[0];
		//std::string MTCFile  = argv[1];

	//	std::string filename = MTCFile.substr(0, MTCFile.find("."));
		//std::cout<< "Starting MTC Phase2 for File: "<<filename<<std::endl;

		//std::cout<<"RESULTS:" <<std::endl;
		std::vector<int> tourOrder = parseTourFile(tourFile, mixSplits);
	//	parseMTCFile (MTCFile, _brujinGragh, _weightedGraph);

		//for(unsigned int i = 0; i < tourOrder.size(); ++i)
		//	std::cout<< tourOrder[i]<<std::endl;

		tourOrder = reorderTour(tourOrder);
	//	for(unsigned int i = 0; i < tourOrder.size(); ++i)
	//		std::cout<< tourOrder[i]<<std::endl;

		int cycledWeight = _weightedGraph.vertices[tourOrder[tourOrder.size()-1]-1].edges[0].weight;
		mixSplits -= cycledWeight;

		CreateDag(tourOrder, _weightedGraph, dag, printWasteResults);
		//dag.generateDotyGraph(filename + ".dot");
		//dag.generateJSON(filename + ".json");
		dag->DagName() = "foo";
//		dag.generateDropletDag(filename+".txt");

		/*std::vector<double> testValidate;
		for(unsigned int i = 0 ; i <_weightedGraph.vertices.size(); ++i)
		{
			if(_weightedGraph.vertices[i].v->name != 0){
				testValidate.push_back((double)_weightedGraph.vertices[i].v->name/(double)_weightedGraph.g->allNodes.size());
			}
		}*/
		//if(dag->isValidSingleReactantDilution(testValidate, weiGraph.g->allNodes.size()))
			//std::cout<< "PASSED\n";
		//else
			//std::cout<<"FAILED\n";

//		dag->generateDropletDag();
		//dag.generateMCFLOWDag();

		return;
}


std::vector<int> MTC::getNamedNodes (std::vector<int> DagOrder, WeightedGraph wg)
{
	std::vector<int> ret;
	for(int i = 0 ; i <DagOrder.size();++i)
		ret.push_back(wg.vertices[DagOrder[i]-1].v->name);
	return ret;
}
void MTC::fillDagGen(std::vector<HamiltonianPath> paths, int size, DagGen * ret, bool printResults)
{ 	int bufferCount = 1;
	int reactantCount = 1;
	int wasteCount=0;
	int mixSplitCount =0;
	char buffer [25];

	Vertex* saved = NULL;
	for(int i = 0 ; i < paths.size(); ++i){
		for (HamiltonianNode* temp = paths[i].head; temp->next != NULL; temp = temp->next)
		{

			Vertex* d1 = NULL;
			if(saved)
				d1 = saved;
			else if(temp->name){
				sprintf(buffer,"reactantDrop%i", reactantCount++);
				d1 = ret->addVertex(DISPENSE, buffer );
			}
			else{
				sprintf(buffer,"bufferDrop%i", bufferCount++);
				d1 = ret->addVertex(DISPENSE, buffer);
			}

			Vertex* d2 = NULL;
			if(temp->next->edgeType){
				sprintf(buffer,"reactantDrop%i", reactantCount++);
				d2 = ret->addVertex(DISPENSE, buffer);
			}
			else{
				sprintf(buffer,"bufferDrop%i", bufferCount++);
				d2 = ret->addVertex(DISPENSE, buffer);
			}

			Vertex* m = NULL;
			sprintf(buffer,"Mixed%i",temp->next->name);
			m = ret->addVertex(MIX, buffer);

			Vertex* s = NULL;
			sprintf(buffer,"Split%i", temp->next->name);
			s = ret->addVertex(SPLIT, buffer);

			Vertex* w = NULL;
			if(temp->next->next == NULL) {
				sprintf(buffer,"Output%i", temp->next->name);
				w = ret->addVertex(OUTPUT, buffer);
				saved = s;
			}
			else {
				sprintf(buffer,"Waste%i", temp->next->name);
				w = ret->addVertex(WASTE, buffer);
				wasteCount++;
				saved = s;
			}
			mixSplitCount++;

			ret->addEdge(d1,m);
			ret->addEdge(d2,m);
			ret->addEdge(m,s);
			ret->addEdge(s,w);
		}
	}
	sprintf(buffer,"Waste%s", saved->label.c_str());
	Vertex* w = ret->addVertex(WASTE, buffer);
	wasteCount++;
	ret->addEdge(saved,w);

	if(printResults){
		std::cout << "MixSplits: " <<   mixSplitCount << std::endl;
		std::cout << "Waste: " << wasteCount << std::endl;
		std::cout << "Samples: " << reactantCount-1 << std::endl;
		std::cout << "buffer: " << bufferCount-1 << std::endl;
	}
}

void MTC::CreateDag(std::vector<int> DagOrder, WeightedGraph wg, DagGen* dag, bool printWasteResults)
{
	std::vector<int> namedNodes = getNamedNodes(DagOrder, wg);

	std::vector<HamiltonianPath> paths;
	for(int i = 0; i< namedNodes.size()-1; ++i){
		paths.push_back(wg.shortestPath(wg.g->allNodes[namedNodes[i]], wg.g->allNodes[namedNodes[i+1]]));
	}
	fillDagGen(paths,wg.g->allNodes.size(), dag, printWasteResults);
}

/* Name: parseMTCFile
 *
 * Parameters: string
 *
 * This function takes in the filename of the MTC Graphs. This will repopulate the Brujin
 * graph and the associated weighted graph that was used to find the path.
 */

void MTC::parseMTCFile(std::string fileName, BrujinGraph& bg, WeightedGraph& wg)
{
	std::cout<<"Parsing MTC"<<std::endl;
	//BrujinGraph *bruGraph = NULL;
	//WeightedGraph *weiGraph = NULL;
	std::ifstream infile;
	infile.open(fileName.c_str());
	if(!infile){
		std::cout<< "ERROR: Could not open MTC file";
		return;
	}
	std::string fileLine;
	bool addVertices = false;
	while(!infile.eof()){

		getline(infile, fileLine);
		if(!addVertices){
			if(fileLine =="SIZE") {
				getline(infile, fileLine);
		//		bruGraph = new BrujinGraph(toInt(fileLine));
		//		bg = *bruGraph;
			}
			else if (fileLine =="Verticies"){
			;//	addVertices = true;
			//	weiGraph = new WeightedGraph(bruGraph);
			}
		}
		else if(fileLine !=""){
			//weiGraph->addVertex(bruGraph->allNodes[toInt(fileLine)]);
			//std:: cout<<toInt(fileLine)<< " ";
		}
	}
//	weiGraph->calculateEdges();
//	if(weiGraph->isComplete())
//		wg = *weiGraph;
//	else
//		wg = NULL;
	infile.close();
}

/* Name: parseTourFile
 *
 * Parameters: string file, int mixSplits
 *
 * This function takes in the name of the tour file produced by LKH. This file holds the
 * total mixSplits and the traveling salesman path. If the Mix/Split amount is not found
 * then the mixSplit count is set to -1. If the file could not be opened then an empty
 * vector is returned and the mixSplit count is set to -1.
 */

std::vector<int> MTC::parseTourFile(std::string file, int & mixSplits)
{
	std::ifstream tourFile;
	tourFile.open(file.c_str());

	bool saveTour = false;
	mixSplits = -1;

	std::string tourLine;
	std::vector<int> tourOrder;
	if (!tourFile){
		std::cout<< "ERROR: Could not open tour file";
		return tourOrder;
	}

	while(!tourFile.eof()){
		getline(tourFile, tourLine);
		if(!saveTour){
			if(tourLine.find("Length") != -1)
				mixSplits = getTourLength(tourLine);
			else if( tourLine.find("TOUR_SECTION") != -1)
				saveTour = true;
		}
		else
		{
			int node = toInt(tourLine);
			if(node == -1)
				return tourOrder;
			tourOrder.push_back(node);
		}
	}
	tourFile.close();
	return tourOrder;
}

/* Name: getTourLength
 *
 * Parameters: string s
 *
 * The parameterized string is assumed to be a line from the tour file. This functions
 * parses out the number of Mix/Splits it takes to run a full tour. On error the function
 * returns -1.
 */
int MTC::getTourLength(std::string s)
{
	int index = s.find("=");
	if(index != -1){
		std::stringstream ss;
		s=s.substr(index+1);

		return toInt(s);
	}
	return -1;
}
/* Name: toInt
 *
 * Parameters: string s
 *
 * This function assumes that the parameter is a single ascii number and returns the
 * converted number.
 */
int MTC::toInt(std::string s)
{
	std::stringstream ss;
	ss << s;
	int ret;
	ss >> ret;
	return ret;
}

/* Name: reorderTour
 *
 * Parameters vector<int>
 *
 * This takes an existing list of nodes and shifts the order to force node 1 to be first.
 */
std::vector<int> MTC::reorderTour(std::vector<int> tour)
{
	if(tour.size() > 0 && tour[0] == 1)
		return tour;
	int start = -1;
	std::vector<int> ret;
	for(int i = 0; i < tour.size(); ++i){
		if(start ==-1){
			if(tour[i]==1){
				start=i;
				ret.push_back(tour[i]);
			}
		}
		else
			ret.push_back(tour[i]);
	}
	for(int i = 0; i < start; ++i)
		ret.push_back(tour[i]);
	return ret;
}
