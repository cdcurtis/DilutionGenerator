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
#include "NRT_ISI/Headers/NRT_ISI.h"
#include "ISI_NCKU/Headers/ISI_NCKU.h"
#include "BinSearch/Headers/Griffith_dilution.h"
#include "DMRW/Headers/Roy_dilution.h"

int main (int argc, char* argv [])
{
	DagGen *dag = NULL;

	*dag = MinMix::RunMinMix(argc, argv);
	dag->generateDotyGraph("foo.txt");
	dag->generateDotyGraph();
	/**dag = Remia::RunRemia(argc, argv);
	*dag = Gorma::RunGorma(argc, argv);
	*dag = Wara::RunWara(argc,argv);
	*dag = GDA::RunGDA(argc, argv);
	*dag = CoDOS::RunCoDOS(argc,argv);
	*dag = NRT_ISI::RunNRT_ISI(argc,argv);
	*dag = ISI_NCKU::RunNCKU(argc,argv);
	*dag = GriffDilute::RunGriffith(argc, argv);
	*dag = RoyDilute::RoyDilute_Process(argc,argv);

	cout<<"hi";*/
	return 0;
}
