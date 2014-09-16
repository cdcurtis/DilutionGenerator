/*
 * main.cpp
 *
 *  Created on: Sep 12, 2014
 *      Author: chris
 */

#include <iostream>
#include "DagGen/Headers/DagGen.h"
#include "MinMix/Headers/MinMix.h"
#include "Remia/Headers/Remia.h"
#include "Gorma/Headers/Gorma.h"
#include "Wara/Headers/Wara.h"
#include "GDA/Headers/GDA.h"
#include "CoDOS/Headers/CoDOS.h"

int main (int argc, char* argv [])
{
	DagGen *dag = NULL;

	*dag = MinMix::RunMinMix(argc, argv);
	*dag = Remia::RunRemia(argc, argv);
	*dag = Gorma::RunGorma(argc, argv);
	*dag = Wara::RunWara(argc,argv);
	*dag = GDA::RunGDA(argc, argv);
	*dag = CoDOS::RunCoDOS(argc,argv);

	cout<<"hi";
	return 0;
}
