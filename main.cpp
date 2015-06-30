/*
 * main.cpp
 *
 *  Created on: Sep 12, 2014
 *      Author: chris
 */

#include <iostream>
#include "UserHelpStrings.h"
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


enum DilutionAlgorithms {MINMIX, REMIA , ALGORITHM_NOT_FOUND};
enum OutputTypes {DOTY, DIGITAL, FLOW, FLATFILE, JSON, OUTPUT_ALL, OUPUT_NOT_FOUND};

inline bool IsInteger(const std::string & s)
{
	if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;

	char * p ;
	strtol(s.c_str(), &p, 10) ;

	return (*p == 0) ;
}
inline bool IsDivisibleofPower(int n, int powerOf){
	while (n % powerOf == 0) {
		n /= powerOf;
	}
	return n == 1;
}

void RunCommandLineSystem(int argc, char** argv);
void RunMenuSystem(DagGen * = NULL);


const int OUTPUT_TYPE_POSITION = 1;
const int FILE_NAME_POSITION = 2;
const int ALGORITHM_NAME = 3;
const int ALGORITHM_ARGS_START = 4
		;
int main (int argc, char* argv [])
{
	cout<< "Welcome to my world!" <<endl;


	if (argc > 1)
		RunCommandLineSystem(argc, argv);
	else{
		cerr<< "ERROR: See Doc for proper positioning"<<endl;
		//	RunMenuSystem();
	}


	/*numerator denominator 121 256*/ //partially broken
	/*char * a[] = {"blah", "121", "256"};
	DagGen dag;// = new DagGen();
	Gorma g;
	g.RunGorma(dag,3, a);
	dag.DagName()="Gorma";
	dag.generateDotyGraph("test.dot");
	 */

	/*Single input and multi input*/
	/*Wara w;
	char * a[] = {"blah", "27", "59", "223", "256"};
	dag = new DagGen();
	w.Run_Wara(dag,5,a);
	dag->generateDropletDag("Wara27_59_223.txt");
	delete dag;*/

	/*char * a[] = {"blah", "0", "1", ".5", "5"};
	GDA::RunGDA(5, a,dag);
	dag->generateDotyGraph("GDA.dot");
	delete dag;
	 */
	/*2 45 23 67 93*/
	/*char * a[] = {"blah", "2", "7","7", "5", "5", "3","3","2" };
	dag = CoDOS::RunCoDOS(9, a);
	dag->DagName()="CoDos5React";
	dag->generateDotyGraph("CoDOS.dot");
	dag->generateDropletDag("CoDos5React2_45_23_67_93.txt");
	delete dag;*/

	/*45 23 67 93*/
	/*char * a[] = {"blah", "3", "6", "9" };
	dag = NRT_ISI::RunNRT_ISI(4,a);
	dag->generateDotyGraph("NRT_ISI.dot");
	delete dag;
	 */
	/*ISI_NCKU ncku;
	char * a[] = {"blah", "11", "10", "6", "5" };
	ncku.RUN_NCKU(dag,5,a);
	dag->generateDropletDag("NCKU.txt");
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

	/*	char * a[] = {"blah", "test", "16", "10", "12", "13", "14" };
	MTC::RunMTC(7,a, dag);
	cout << dag->isEmpty()<<endl;
	dag->generateDotyGraph("MTC.dot");
	//delete dag;
	 */
	cout<<"Terminating"<<endl;
	return 0;
}
/*
 * Each Concentration Value must be an integer.
 * all values must be declared before the Names are declared
 */
bool ValidateParameters(DilutionAlgorithms algorithm, vector<string> parameters, string& ErrorMessage)
{
	unsigned int i = 0;
	if(parameters.size() == 0){
		cerr<<"There are no parameters for the alogorithm."<<endl;
		return false;
	}
	switch (algorithm) {
	case MINMIX:
		/*
		 * Each Concentration Value must be an integer. All values must be
		 * declared before the names are declared.
		 */
		for(; i< parameters.size(); ++i)
			if(!IsInteger(parameters[i]))
				break;
		for(;i<parameters.size(); ++i)
			if(IsInteger(parameters[i])){
				ErrorMessage = "Concentration values mixed with concetration names.";
				return false;
			}
		break;
	case REMIA:
		/*
		 * Each Concentration Value must be an integer. Numerator must be smaller
		 * than denominator. Denominator must be a power of 2
		 */
		for(; i< parameters.size(); ++i)
			if(!IsInteger(parameters[i])){
				ErrorMessage = "Parameter " + parameters[i] + " is not an integer.";
				return false;
			}
		if(atoi(parameters[0].c_str()) >= atoi(parameters[1].c_str())){
			ErrorMessage = "Numerator can not be greater or equal to denominator.";
			return false;
		}
		if(!IsDivisibleofPower(atoi(parameters[1].c_str()), 2)){
			ErrorMessage = "Denominator is not power of 2.";
			return false;
		}
		break;
	default:
		break;
	}
	return true;
}

string GetUserInput(string userMessage)
{
	cout <<userMessage;
	char userInput[50];
	cin.get(userInput,50);

	return userInput;
}

DilutionAlgorithms GetDilutionAlgorithm(string s)
{
	if(s.find("1") != string::npos || s.find("MINMIX") != string::npos || s.find("MinMix") != string::npos || s.find("Min Mix") != string::npos || s.find("minmix") != string::npos || s.find("min mix") != string::npos)
		return MINMIX;
	if(s.find("REMIA") != string::npos || s.find("Remia") != string::npos)
		return REMIA;
	return ALGORITHM_NOT_FOUND;
}
OutputTypes GetOutputType(string s)
{
	if (s.find("ALL") != string::npos || s.find("all") != string::npos)
		return OUTPUT_ALL;
	if(s.find("DIGITAL") != string::npos || s.find("digital") != string::npos)
		return DIGITAL;
	if (s.find("FLOW") != string::npos || s.find("flow") != string::npos)
		return FLOW;
	if(s.find("DAGGEN") != string::npos || s.find("daggen") != string::npos || s.find("FLAT") != string::npos )
		return FLATFILE;
	if(s.find("DOT") != string::npos || s.find("dot") != string::npos)
		return DOTY;
	return OUPUT_NOT_FOUND;
}

void OutputDag(DagGen* dag, string output, string fileName)
{
	OutputTypes outputType = GetOutputType(output);

	switch (outputType){
	case OUTPUT_ALL:
		dag->WriteToFile(fileName);
		dag->generateMCFLOWDag(fileName);
		dag->generateDropletDag(fileName);
		dag->generateDotyGraph(fileName);
		break;
	case DIGITAL:
		dag->generateDropletDag(fileName);
		break;
	case FLOW:
		dag->generateMCFLOWDag(fileName);
		break;
	case FLATFILE:
		dag->WriteToFile(fileName);
		break;
	case DOTY:
		dag->generateDotyGraph(fileName);
		break;
	default:
		cerr<<"ERROR: no output recognized for " << output <<"\n";
		//RunMenuSystem(dag);
		return;
	}
}

void RunCommandLineSystem(int argc, char** argv){
	string algorithmToRun="";
	if(argc > ALGORITHM_NAME)
		algorithmToRun = argv[ALGORITHM_NAME];
	else {
		cerr<<"Error: System went into command line system with no content.\n Switching to menu system.\n";
		//return RunMenuSystem();
	}

	DilutionAlgorithms algorithm = GetDilutionAlgorithm(algorithmToRun);
	DagGen * dag = NULL;
	vector<string> parameters;


	for(int i = ALGORITHM_ARGS_START; i< argc; ++i)
		parameters.push_back(argv[i]);

	string errorMessage;
	if(!ValidateParameters(algorithm,parameters,errorMessage)){
		cerr<<"Arguments for " << algorithmToRun << " invalid\n"<< errorMessage<<endl;
		return;
	}

	switch(algorithm){
	case MINMIX: /* Concentration Values,  7 7 5 5 3 3 2*/
		cout<<"Running MINMIX"<<endl;
		dag = MinMix::RunMinMix(parameters);
		break;
	case REMIA:	/*numerator denominator,   131 256*/
		cout<<"Running Remia"<<endl;
		dag = Remia::RunRemia(parameters);
		break;
	case ALGORITHM_NOT_FOUND:
	default:
		cerr<<"ERROR: algorithm " << algorithmToRun << " not recognized." << endl;
		return;
	}



	string output = argv[OUTPUT_TYPE_POSITION];
	string fileName = argv[FILE_NAME_POSITION];
	if(fileName == "CONSOLE")
		fileName = "";
	OutputDag(dag, output, fileName);

	delete dag;
}




/*void RunOutputSystem(DagGen* dag)
{
	if(dag == NULL || dag->isEmpty()) {
		cerr<<"ERROR: Generated dag is empty or does not exist.\n";
		return;
	}
	while(true) {
		string userInput;
		cout<< string(80,'=')<<"\n";
		cout<<"What output format do you want?\n";
		cout<<"\t1. Digital Microfluidic instructions.\n";
		cout<<"\t2. Continuous Flow Instructions.\n";
		cout<<"\t3. Dilution Generator File.\n";
		cout<<"\t4. Doty Graph.\n";
		cout<<"\t5. All.\n";
		cout<<"\t6. Exit.\n";

		cin >> userInput;
		if(IsInteger(userInput)){
			int userChoice = atoi(userInput.c_str());
			string fileName = GetUserInput("Enter name for file. Press enter to output to console.\n");

			switch (userChoice) {
			case 1:
				dag->generateDropletDag(fileName);
				break;
			case 2:
				dag->generateMCFLOWDag(fileName);
				break;
			case 3:
				dag->WriteToFile(fileName);
				break;
			case 4:
				dag->generateDotyGraph(fileName);
				break;
			case 5:
				dag->WriteToFile(fileName);
				dag->generateMCFLOWDag(fileName);
				dag->generateDropletDag(fileName);
				dag->generateDotyGraph(fileName);
				break;
			case 6:
				cout <<"Returning to Main menu.\n";
				return;
			default:
				cerr << "ERROR: User entered unspecified choice, try again.\n";
				continue;
			}
		}
		else{
			cerr<< "ERROR: "<< userInput << " is not a number, try again.\n";
			continue;
		}
	}
}*/
/*bool DoesUserNeedHelp(string s)
{
	return s.find("-Help") != -1 || s.find("-help") != -1;
}*/
/*
void DisplayHelp(DilutionAlgorithms algorithm)
{
	switch (algorithm) {
		case MINMIX:
			cout<<UserHelpString::MINMIX_HELP_STRING<<endl;
			GetUserInput("Press Enter to exit.\n");
			return;
		default:
			break;
	}
}

void RunMenuSystem(DagGen* dag){
	if(dag != NULL)
		RunOutputSystem(dag);

	while (true) {
		cout << "What dilution algorithm do you want to perform?\n Type -Help after your choice for more information.\n";
		cout << "\t1. MinMix\n";

		string userInput;
		getline(cin, userInput);

		DilutionAlgorithms dilutionAlgorithm = getDilutionAlgorithm(userInput);
		if(dilutionAlgorithm == NOTFOUND){
			cout << "ERROR: Dilution Algorithm not found try again.";
			continue;
		}
		bool needsHelp = DoesUserNeedHelp(userInput);

		switch (dilutionAlgorithm) {
		case MINMIX:
			if(needsHelp)
				DisplayHelp(MINMIX);
			else
				 MinMix::RunMinMix();
			break;
		default:
			break;
		}
	}

}*/
