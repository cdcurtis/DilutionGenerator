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
/*
	/*Test 3 7,7,5,5,3,3,2 blood,water,plasma,mercury,sodium,pancakes,oil*
	char * a[] = {"blah", "Test", "3", "1,1,1", "Water,Blood, Water"};
	dag  = MinMix::RunMinMix(argc, a);

	if (dag != NULL)
	dag->generateDotyGraph("test.doty");
	delete dag;
*/

	/*numerator denominator   121 256*/
	/*char * a[] = {"blah", "121", "256"};
	char buffer[50];

	for(int i = 1; i< 256; ++i){
	sprintf(buffer,"%i",i);
	a[1]=buffer;
cout<<"Running:" << a[1]<<"/" <<a[2]<<endl;
	dag = Remia::RunRemia(argc, a);
	dag->DagName()="Remia";
	sprintf(buffer,"Remia%i.doty",i);
	dag->generateDotyGraph(buffer);
	//dag->generateDropletDag("Remia191_1024.txt");
	delete dag;
	}*/


	/*numerator denominator 121 256*/ //partially broken
	char * a[] = {"blah", "121", "256"};

	dag = Gorma::RunGorma(argc, a);
	dag->DagName()="Gorma";
	dag->generateDotyGraph();
	delete dag;

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
	//dag->DagName()="CoDos5React";
	//dag->generateDotyGraph("CoDOS.dot");
	//dag->generateDropletDag("CoDos5React2_45_23_67_93.txt");
	//delete dag;

	/*45 23 67 93*/
	//dag = NRT_ISI::RunNRT_ISI(argc,argv);
	//dag->generateDotyGraph("NRT_ISI.dot");
	//delete dag;

	//TODO:: Find discover the input for NCKU.
//	dag = ISI_NCKU::RunNCKU(argc,argv);
	//dag->generateDotyGraph("NCKU.dot");
	//delete dag;

	//numops desiredconcentratin tolerance
	/*10 0.1015625 .0078125*/
	//dag->DagName()="griffith";
	//dag = GriffDilute::RunGriffith(argc, argv);
	//dag->generateDotyGraph("griffith.dot");
	//dag->generateDropletDag("griffith.txt");
	//delete dag;

	//dag = RoyDilute::RoyDilute_Process(argc,argv);
	//dag->generateDotyGraph("roy.dot");
	//delete dag;

	//dag = MTC::RunMTC(argc,argv);
	//dag->generateDotyGraph("MTC.dot");
	//delete dag;

	cout<<"Terminating"<<endl;
	return 0;
}
