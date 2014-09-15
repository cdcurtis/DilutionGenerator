#ifndef __CONCENTRATION_H__
#define __CONCENTRATION_H__

#include <iostream>
#include <vector>

struct ConcentrationNode{
	int ratio;
	int fluid;
	ConcentrationNode();
	ConcentrationNode(int r, int f): ratio(r), fluid(f) {}
	void print (std :: string = "");
	std :: ostream& print (std :: ostream&);
};

class Concentration {
private:
	int totalRatio;
	std :: vector<ConcentrationNode> parts;
	std :: vector<std :: string> fluidList;

public:
	Concentration():totalRatio(0) {};
	Concentration(int r, int f): totalRatio(r)
	{
		parts.push_back(ConcentrationNode(r,f));
	}

	void insert(int r, int f);

	void print (std :: string fileName = "" , bool printSimple = false);	
		
	std :: vector<ConcentrationNode> & conNodes() {return parts;}
	std :: vector< std :: string> & listOfFluids() { return this->fluidList;}
	int ratioTotal () {return totalRatio;}
};

#endif /*__CONCENTRATION_H__*/