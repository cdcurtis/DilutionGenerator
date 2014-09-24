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
#include "MTC/Headers/MTC.h"


int main (int argc, char* argv [])
{
	cout<< "Welcome to my world!" <<endl;

	DagGen *dag;

	/*Test 3 7,7,5,5,3,3,2 blood,water,plasma,mercury,sodium,pancakes,oil*/
	//dag  = MinMix::RunMinMix(argc, argv);

	//if (dag != NULL)
	//dag->generateDotyGraph("temptemp.txt");
	//delete dag;

	/*numerator denominator   121 256*/
	/*dag = Remia::RunRemia(argc, argv);
	dag->generateDotyGraph("Remia.doty");
	delete dag;
*/
	/*numerator denominator 121 256*/
	//dag = Gorma::RunGorma(argc, argv);
	//dag->generateDotyGraph("Remia.dot");
	//delete dag;

	/*Single input and multi input*/
	//dag = Wara::RunWara(argc,argv);
	//dag->generateDotyGraph("Wara.dot");
	//delete dag;

	//TODO:: Find discover the input for GDA.
	/*dag = GDA::RunGDA(argc, argv);
	dag->generateDotyGraph("GDA.dot");
	delete dag;*/

	/*2 45 23 67 93*/
	//dag = CoDOS::RunCoDOS(argc,argv);
	//dag->generateDotyGraph("CoDOS.dot");
	//delete dag;

	/*45 23 67 93*/
	//dag = NRT_ISI::RunNRT_ISI(argc,argv);
	//dag->generateDotyGraph("NRT_ISI.dot");
	//delete dag;

	*dag = ISI_NCKU::RunNCKU(argc,argv);
	/**dag = GriffDilute::RunGriffith(argc, argv);
	*dag = RoyDilute::RoyDilute_Process(argc,argv);

	cout<<"hi";*/
	cout<<"Terminating"<<endl;
	return 0;
}
