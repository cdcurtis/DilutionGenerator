/*
 * BinarySearch.h
 *
 *  Created on: Nov 13, 2015
 *      Author: Chris
 */

#ifndef BINARYSEARCH_H_
#define BINARYSEARCH_H_

#include <iostream>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <cmath>
#include <map>
#include <set>

#include "../DagGen/Headers/DagGen.h"

std::string ToString(double dbl)
{
	std::ostringstream strs;
	strs << dbl;
	return strs.str();
}

DagGen* RunBinarySearch(std::vector<std::string> parameters)
{
	// tolerance, desiredconcentration, dropletConcentration, bufferConcentration

	if(parameters.size()==0) {
		std::cerr<<"Error: Paramters are empty\n";
		std::exit(-1);
	}

	double tolerance = -1;
	double desiredConcentration = -1;
	double myConcentration = -1;
	double mybuffer = -1;



	tolerance = atof(parameters[0].c_str());
	desiredConcentration = atof(parameters[1].c_str());


	if(parameters.size()<3) {
		myConcentration = 1;
		mybuffer = 0;
	}
	else if (parameters.size() < 4) {
		myConcentration = atof(parameters[2].c_str());
		mybuffer = 0;
	}
	else{
		myConcentration = atof(parameters[2].c_str());
		mybuffer = atof(parameters[3].c_str());
	}


	if(tolerance ==-1 || desiredConcentration == -1 || myConcentration == -1 || mybuffer == -1){
		std::cerr<< "Error: parameters not properly input:\n"
				"\tTolerance: " << tolerance << std::endl <<
				"\tDesired Concentration: " << desiredConcentration << std::endl <<
				"\tDroplet Concentration: " << myConcentration << std::endl <<
				"\tBuffer Concentration: " << mybuffer << std::endl;
		std::exit(-1);
	}

	DagGen * ret = new DagGen();
	DagGen * availableConcentrations = new DagGen();

	double nextHighValue = myConcentration;
	Vertex* nextHighVertex = availableConcentrations->addVertex(DISPENSE, ToString(myConcentration));

	double nextLowValue = mybuffer;
	Vertex* nextLowVertex = availableConcentrations->addVertex(DISPENSE, ToString(mybuffer));

	double range = myConcentration - mybuffer;
	double newConcentration = range;
	while (abs(desiredConcentration - newConcentration) > tolerance)
	{
		for (unsigned int i = 0 ; i < availableConcentrations->Vertices().size(); i++) {
			Vertex* highValueVertex = availableConcentrations->Vertices()[i];
			double highValue = atof(highValueVertex->label.c_str());

			for(unsigned  int j = 0; j <availableConcentrations->Vertices().size(); ++j) {
				Vertex * lowValueVertex = availableConcentrations->Vertices()[j];
				double lowValue = atof(lowValueVertex->label.c_str());
				if( lowValue >= highValue)
					continue;

				if( lowValue < desiredConcentration && highValue > desiredConcentration) {
					if (highValue - lowValue < range && highValue - lowValue >=0) {
						range = highValue - lowValue;
						nextHighValue = highValue;
						nextLowValue = lowValue;

						nextHighVertex = highValueVertex;
						nextLowVertex = lowValueVertex;
					}
				}
			}
		}
		if (nextHighValue ==-1 || nextLowValue == -1) {
			std::cerr<< "Error, No suitable values found in Available Concentrations.\n";
			std::exit(-1);
		}
		newConcentration = (nextHighValue+nextLowValue)/2.0;
		Vertex* highVertex = NULL;
		Vertex* lowVertex = NULL;
		for (unsigned int i =0 ; i< ret->Vertices().size() ; ++ i ) {
			if (ret->Vertices()[i]->type == MIX)
				continue;
			if (atof(ret->Vertices()[i]->label.c_str()) == nextHighValue)
				highVertex = ret->Vertices()[i];
			if (atof(ret->Vertices()[i]->label.c_str()) == nextLowValue)
				lowVertex = ret->Vertices()[i];
		}

		if (highVertex == NULL || highVertex->type==DISPENSE)
			highVertex = ret->addVertex(DISPENSE, ToString(nextHighValue));
		if (lowVertex == NULL || lowVertex->type==DISPENSE)
			lowVertex = ret->addVertex(DISPENSE, ToString(nextLowValue));

		Vertex* MixVertex = ret->addVertex(MIX, ToString(newConcentration));
		Vertex* SplitVertex = ret->addVertex(SPLIT, ToString(newConcentration));
		availableConcentrations->addVertex(SPLIT, ToString(newConcentration));

		ret->addEdge(highVertex,MixVertex);
		ret->addEdge(lowVertex,MixVertex);
		ret->addEdge(MixVertex,SplitVertex);

		nextHighValue  = -1;
		nextLowValue  = -1;
		nextLowVertex = NULL;
		nextHighVertex = NULL;
	}

	//ret->generateDotyGraph("foo");
	return ret;
}


void FixTree (DagGen* dag, int nodeID, int newTreesNeeded, std::map<int, std::vector<Edge*> > & parentMap, std::map<int, std::vector<Edge*> >& childMap, std::vector<std::string> parameters)
{
	std::string nodeNeeded = dag->getVertex(nodeID)->label;
	//std::cout <<"New Trees Needed: " <<newTreesNeeded<<std::endl;
	//cout<< "Node: " << nodeNeeded << std::endl;


	parameters[1] = nodeNeeded;

	while(newTreesNeeded > 0) {
		DagGen * newSubTree = RunBinarySearch(parameters);

		//newSubTree->generateDotyGraph("Subtree");

		int oldID = -1;
		for(unsigned int i = newSubTree->Vertices().size()-1 ; i >= 0 ; --i ) {
			if (newSubTree->Vertices().at(i)->label == nodeNeeded){
				oldID = newSubTree->Vertices().at(i)->uniqueID;
				break;
			}
		}

		std::map<int,int> newIDs = dag->MergeTree(newSubTree);

		//dag->generateDotyGraph("Merged");
		int count = 0;
		for(Edge * edge: dag->Edges()) {
			if (edge->parent == nodeID)
				if(count++ < 2)
					edge->parent = newIDs.find(oldID)->second;
		}
		newTreesNeeded--;
	}
}


DagGen* RunBinarySearchTrueMixingTree(std::vector<std::string> parameters)
{
	DagGen* FixMe = RunBinarySearch(parameters);

	//FixMe->generateDotyGraph("Orignial");
	std::map<int, std::vector<Edge*> > parentMap;
	std::map<int, std::vector<Edge*> > childMap;

	std:: set <int> problemNodes;

	for (unsigned int i = 0 ; i< FixMe->Edges().size(); ++i) {
		if (parentMap.count(FixMe->Edges()[i]->parent) == 0){
			std::vector< Edge*> edges;
			edges.push_back(FixMe->Edges()[i]);
			parentMap.insert(std::pair<int, vector<Edge*> >(FixMe->Edges()[i]->parent, edges) );
		}
		else {
			parentMap.find(FixMe->Edges()[i]->parent)->second.push_back(FixMe->Edges()[i]);
			if (parentMap.find(FixMe->Edges()[i]->parent)->second.size() > 2)
				problemNodes.insert(FixMe->Edges()[i]->parent);
		}

		if (childMap.count(FixMe->Edges()[i]->child) == 0){
			std::vector< Edge*> edges;
			edges.push_back(FixMe->Edges()[i]);
			childMap.insert(std::pair<int, vector<Edge*> >(FixMe->Edges()[i]->child, edges) );
		}
		else
			childMap.find(FixMe->Edges()[i]->child)->second.push_back(FixMe->Edges()[i]);
	}// create dependancy map and identify nodes that need to be fixed.

	for(int value: problemNodes) {
		int numNewTreesNeeded = ceil((parentMap.find(value))->second.size()/2.0) - 1;
		FixTree(FixMe, value, numNewTreesNeeded, parentMap, childMap, parameters);
	}
	return FixMe;
}
#endif /* BINARYSEARCH_H_ */
