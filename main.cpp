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
#include "IDMA/Headers/IDMA.h"
#include "MTC/Headers/MTC.h"


int main (int argc, char* argv [])
{
	cout<< "Welcome to my world!" <<endl;

	DagGen * dag;// = new DagGen("Remia62_256.dag");

	//dag->generateDotyGraph("test.dot");
	//dag->generateDotyGraph();
	//cout << dag->Vertices().size() <<endl;
	/*Test 3 7,7,5,5,3,3,2 blood,water,plasma,mercury,sodium,pancakes,oil*/
	/*char * a[] = {"blah", "Test", "3", "1,1,1", "Water,Blood,Water"};
	dag  = MinMix::RunMinMix(5, a);

	if (dag != NULL)
//	dag->generateDotyGraph();
	dag->WriteToFile();
	delete dag;
*/

	/*numerator denominator   121 256*/
	char * a[] = {"blah", "121", "256"};
	char buffer[50];

	for(int i = 1; i< 256; ++i){
	sprintf(buffer,"%i",i);
	a[1]=buffer;
	cout<<"Running:" << a[1]<<"/" <<a[2]<<endl;
	dag = Remia::RunRemia(argc, a);

	sprintf(buffer,"Remia%i_256",i);
	dag->DagName()=buffer;
	string fileName = buffer; fileName += ".dag";
	dag->WriteToFile(fileName);
	//dag->generateDotyGraph(buffer);
	//dag->generateDropletDag("Remia191_1024.txt");
	delete dag;
	}


	/*numerator denominator 121 256*/ //partially broken
	//char * a[] = {"blah", "121", "256"};
	//Gorma g;
	//g.RunGorma(dag,argc, a);
	//dag.DagName()="Gorma";
	//dag.generateDotyGraph("test.dot");
	//delete dag;

	/*Single input and multi input*/
	/*Wara w;
	char * a[] = {"blah", "43", "67", "123", "256"};
	dag = new DagGen();
	w.Run_Wara(dag,argc,a);
*/

	//w.RunWara(dag, argc,a);
	//dag->generateDotyGraph("Wara.dot");
	//delete dag;

	//char * a[] = {"blah", "0", "1", ".5", "5"};
	//GDA::RunGDA(5, a,dag);
	//dag->generateDotyGraph("GDA.dot");
	//delete dag;

	/*2 45 23 67 93*/
	/*char * a[] = {"blah", "2", "7","7", "5", "5", "3","3","2" };
	dag = CoDOS::RunCoDOS(9, a);
	dag->DagName()="CoDos5React";
	dag->generateDotyGraph("CoDOS.dot");
	dag->generateDropletDag("CoDos5React2_45_23_67_93.txt");
	delete dag;*/

	/*45 23 67 93*/
	/*char * a[] = {"blah", "1", "5", "9" };
	dag = NRT_ISI::RunNRT_ISI(4,a);
	dag->generateDotyGraph("NRT_ISI.dot");
	delete dag;*/

/*	ISI_NCKU ncku;
	char * a[] = {"blah", "11", "10", "6", "5" };
	ncku.RUN_NCKU(dag,5,a);
	dag->generateDotyGraph("NCKU.dot");
	delete dag;*/

	/*char * a[] = {"blah", "10", "0.0078125", "0.1015625" };
	IDMA::RunIDMA(4,a, dag);
	cout<<dag->isEmpty();

	dag->generateDotyGraph("IDMA.dot");

	delete dag;
*/

	//numops desiredconcentratin tolerance
	/*10 0.1015625 .0078125*/
//	char * a[] = {"blah", "10","0.00048828125", "0.1240234375"};
//	dag->DagName()="griffith";
//	GriffDilute::RunGriffith(4, a, dag);
//	dag->generateDotyGraph("griffith.dot");
//	//dag->generateDropletDag("griffith.txt");
//	delete dag;

//	char * a[] = {"blah", "10",".0078125", "0.1015625"};
//	RoyDilute::RunDMRW(4,a,dag);
//	dag->generateDotyGraph("roy.dot");
//	delete dag;

	/*char * a[] = {"blah", "test", "16", "10", "12", "13", "14" };
	MTC::RunMTC(7,a, dag);
	cout << dag->isEmpty()<<endl;
	dag->generateDotyGraph("MTC.dot");
	//delete dag;*/

	cout<<"Terminating"<<endl;
	return 0;
}
