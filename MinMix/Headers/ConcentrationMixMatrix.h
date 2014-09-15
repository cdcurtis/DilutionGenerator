#ifndef __CONCENTRATIONMIXMATRIX_H__
#define __CONCENTRATIONMIXMATRIX_H__

#include <iostream>
#include <vector>
#include "Concentration.h"
#include "TreeRoot.h"


class ConcentrationMixMatrix{
	std :: vector<std :: vector<int> > mixMatrix;
	std :: vector<int> fluidReference;
public:
	ConcentrationMixMatrix() {}
	ConcentrationMixMatrix(std :: vector< std :: vector<int> > m, std :: vector<int> f): mixMatrix(m), fluidReference(f) {}
	ConcentrationMixMatrix (Concentration c);
	
	TreeRoot runMinMix();
	
	void print (std:: string fileName = ""); 
};

#endif /*__CONCENTRATIONMIXMATRIX_H__*/