#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <math.h>
#include <vector>
#include "../Headers/ConcentrationMixMatrix.h"
#include "../Headers/Concentration.h"
#include "../Headers/TreeRoot.h"
#include "../Headers/MixTree.h"
#include "../Headers/MinMix.h"
#include "../../DagGen/Headers/DagGen.h"


DagGen * MinMix :: RunMinMix(std::vector<std::string> parameters)
{
	MinMix minMix;
//-----------------------<INPUTS>------------------------
// <fileName> <option>(0), <#parts4ratio>, ...
// <fileName> <option>(1), <#parts4ratio>, <fluidName>, ...
// <fileName> <option>(2), <CSV #parts4ratio>
// <fileName> <option>(3), <CSV #parts4ratio>, <CSV fluidNames>
	std :: vector<int> ratio;
	std :: vector<std :: string> fluids;

	unsigned int i=0;
	while (i<parameters.size() && is_number(parameters[i]))
		ratio.push_back(atoi(parameters[i++].c_str()));

	while (i<parameters.size())
		fluids.push_back(parameters[i++]);

	while( fluids.size() < ratio.size()) {
		char reagent[15];
		sprintf(reagent, "Reagent%i", fluids.size());
		fluids.push_back(reagent);
	}

//	std :: string fileName = argv[1];
/*	int option = atoi(argv[2]);

	for(int i = 3; i < argc ; ++i){
		switch(option){
		case 0:
			ratio.push_back(atoi(argv[i]));
			break;
		case 1:
			ratio.push_back(atoi(argv[i]));
			fluids.push_back(argv[++i]);
			break;
		case 2:
			ratio = minMix.convertRatio(argv[i]);
			break;
		case 3:
			ratio = minMix.convertRatio(argv[i]);
			fluids = minMix.convertFluidList(argv[++i]);
			break;
		default:
			std :: cout << "ERROR: Not a valid Option\n";
			return NULL;
		}
	}*/

	Concentration concentration;

	//Set up the Concentration
	concentration.listOfFluids()=fluids;
	for(unsigned int i = 0; i< ratio.size();++i){
		int t = ratio [i];
		concentration.insert(t,i);
	}

	//Create the mix matrix used for

	ConcentrationMixMatrix mixM(concentration);

	//create the mixTree
	TreeRoot mixTree = mixM.runMinMix();

	//mixTree.print(std::cout);
	DagGen *dag = mixTree.createDag(concentration.listOfFluids());

	return dag;
}

std :: vector<int>  MinMix :: convertRatio(std :: string ratio)
{
	std :: vector<int>ret;
	std :: stringstream ss (ratio);
	while (ss)
	{
		std :: string s;
		if (!getline( ss, s, ',' )) break;
		ret.push_back( atoi(s.c_str()));
	}
	return ret;
}
std :: vector<std :: string>  MinMix :: convertFluidList(std :: string fluidNames)
{
	std :: vector<std :: string>ret;
	std :: stringstream ss (fluidNames);
	while (ss)
	{
		std :: string s;
		if (!getline( ss, s, ',' )) break;
		ret.push_back(s);
	}
	return ret;
}
bool  MinMix :: is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}




