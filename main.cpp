/*
 * main.cpp
 *
 *  Created on: Sep 12, 2014
 *      Author: chris
 */

#include <iostream>
#include <string>
#include "DagGen/Headers/DagGen.h"
#include "MinMix/Headers/MinMix.h"
#include "Remia/Headers/Remia.h"
#include "Gorma/Headers/Gorma.h"
#include "Wara/Headers/Wara.h"
#include "GDA/Headers/GDA.h"
#include "CoDOS/Headers/CoDOS.h"
#include "NRT_ISI/Headers/NRT_ISI.h"
#include "ISI_NCKU/Headers/ISI_NCKU.h"
#include "BinarySearch/BinarySearch.h"
//#include "BinSearch/Headers/Griffith_dilution.h"
#include "DMRW/Headers/Roy_dilution.h"
#include "IDMA/Headers/IDMA.h"
#include "MTC/Headers/MTC.h"

const bool SILENCE_OUTPUT = false;
const int OUTPUT_TYPE_POSITION = 1;
const int FILE_NAME_POSITION = 2;
const int ALGORITHM_NAME = 3;
const int ALGORITHM_ARGS_START = 4;

enum DilutionAlgorithms {MINMIX, REMIA, WARA, GORMA, GDA, CODOS, NRT_ISI, ISINCKU, IDMA, BINARYSEARCH, DMRW, MTC, ALGORITHM_NOT_FOUND};
enum OutputTypes {DOTY, DIGITAL, FLOW, FLATFILE, JSON, OUTPUT_ALL, OUPUT_NOT_FOUND};


void RunCommandLineSystem(int argc, char** argv);
bool ValidateParameters(DilutionAlgorithms algorithm, const vector<string> & parameters, string& ErrorMessage);
void OutputDag(DagGen* dag, string output, string fileName);

DilutionAlgorithms GetDilutionAlgorithm(string s);
OutputTypes GetOutputType(string s);

inline bool IsInteger(const std::string & s);
inline bool IsDouble(const std:: string & s);
inline bool IsDivisibleofPower(int n, int powerOf);

int main (int argc, char* argv [])
{
	if (argc > 2)
		RunCommandLineSystem(argc, argv);
	else{
		cerr<< "ERROR: See Doc for proper positioning"<<endl;
	}

	if(!SILENCE_OUTPUT)
		cout<<"Exiting Program."<<endl;
	return 0;
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

	if(!SILENCE_OUTPUT)
		cout<<"Validated Parameters"<<endl;

	switch(algorithm){
	case MINMIX: /* Concentration Values,  7 7 5 5 3 3 2*/
		if(!SILENCE_OUTPUT)
			cout<<"Running MINMIX"<<endl;
		dag = MinMix::RunMinMix(parameters);
		break;
	case REMIA:	/*numerator denominator,   131 256*/
		if(!SILENCE_OUTPUT)
			cout<<"Running Remia"<<endl;
		dag = Remia::RunRemia(parameters);
		break;
	case WARA: /*Single input and multi input,  27 59 223 256*/ //Broken needs to be re written.
		//cout<<"Running Wara"<<endl;
		//Wara::Run_Wara(dag,parameters);
		break;

	case GORMA: /*numerator denominator 121 256*/ //partially broken Needs branch and Bound
		if(!SILENCE_OUTPUT)
			cout<<"Running Gorma: This algorithm takes a while"<<endl;
		dag= new DagGen();
		Gorma::RunGorma(parameters,dag);
		break;
	case GDA:
		//char * a[] = {"blah", "0", "1", ".5", "5"};
		//GDA::RunGDA(5, a,dag);
		break;
	case CODOS: /* {"7","7", "5", "5", "3","3","2" };*/
		if(!SILENCE_OUTPUT)
			cout<<"Running CODOS"<<endl;
		dag = CoDOS::RunCoDOS(parameters);
		break;
	case NRT_ISI: /*45 23 67 93*/
		/*char * a[] = {"blah", "3", "6", "9" };*/
		if(!SILENCE_OUTPUT)
			cout<<"Running NRT_ISI"<<endl;
		dag = NRT_ISI::RunNRT_ISI(parameters);
		break;
	case IDMA://numops  tolerance desiredconcentratin /*10  .0078125 0.1015625*/
		dag = new DagGen();
		if(!SILENCE_OUTPUT)
			cout<< "Running IDMA"<<endl;
		IDMA::RunIDMA(parameters, dag);
		break;
	case ISINCKU:/* numerator differnceBetweenSamples n^2(denominator) numSample  {11 10 6 5 }*/
		dag = new DagGen();
		if(!SILENCE_OUTPUT)
			cout << "Running NCKU"<<endl;
		ISI_NCKU::RUN_NCKU(parameters, dag);
		break;
	case BINARYSEARCH: 	// tolerance, desiredconcentration, dropletConcentration(optional), bufferConcentration(optional)
		dag = new DagGen();
		if(!SILENCE_OUTPUT)
			cout<<"Running Binary Search"<<endl;
		dag = RunBinarySearchTrueMixingTree(parameters);
		break;
	case DMRW:  //numops  tolerance desiredconcentratin /*10  .0078125 0.1015625*/
		dag = new DagGen();
		if(!SILENCE_OUTPUT)
			cout<< "Running DMRW"<<endl;
		RoyDilute::RunDMRW(parameters,dag);
		break;
	case MTC:/*filename denominator numerators ... {test 16 10 12 13 14}*/
		if(!SILENCE_OUTPUT)
			cout<<"Runnig MTC"<<endl;
		parameters.insert(parameters.begin(),argv[FILE_NAME_POSITION]);
		dag = new DagGen();
		MTC::RunMTC(parameters, dag);
		break;

	case ALGORITHM_NOT_FOUND:
	default:
		cerr<<"ERROR: algorithm " << algorithmToRun << " not recognized." << endl;
		return;
	}
	if(!SILENCE_OUTPUT)
		cout<<"Success"<<endl;

	string output = argv[OUTPUT_TYPE_POSITION];
	string fileName = argv[FILE_NAME_POSITION];
	if(fileName == "CONSOLE")
		fileName = "";
	OutputDag(dag, output, fileName);

	delete dag;
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

bool ValidateParameters(DilutionAlgorithms algorithm, const vector<string> & parameters, string& ErrorMessage)
{
	//unsigned int i = 0;
	unsigned int sum =0;
	int denom=0;
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
		for(unsigned int i = 0; i< parameters.size(); ++i)
			if(!IsInteger(parameters[i]))
				break;
		for(unsigned int i ; i<parameters.size(); ++i)
			if(IsInteger(parameters[i])){
				ErrorMessage = "Concentration values mixed with concetration names.";
				return false;
			}
		break;
	case REMIA:
	case WARA:
	case GORMA:
		/*
		 * Each Concentration Value must be an integer. Numerator must be smaller
		 * than denominator. Denominator must be a power of 2
		 */
		for(unsigned int i = 0; i< parameters.size(); ++i)
			if(!IsInteger(parameters[i])){
				ErrorMessage = "Parameter " + parameters[i] + " is not an integer.";
				return false;
			}
		for(unsigned int i = 0; i<parameters.size()-1; ++i)
			if(atoi(parameters[i].c_str()) >= atoi(parameters[parameters.size()-1].c_str())){
				ErrorMessage = "Numerator can not be greater or equal to denominator.";
				return false;
			}
		if(!IsDivisibleofPower(atoi(parameters[parameters.size()-1].c_str()), 2)){
			ErrorMessage = "Denominator is not power of 2.";
			return false;
		}
		break;
	case GDA:
		break;
	case CODOS:
		if(IsInteger(parameters[0])){
			for(unsigned int i = 0 ; i < parameters.size(); ++i) {
				if(!IsInteger(parameters[i])){
					ErrorMessage = parameters[i] + " is not an integer";
					return false;
				}
				sum += atoi (parameters[i].c_str());
			}
		}
		else {
			for(unsigned int i = 1; i < parameters.size(); i+=2) {

				if(!IsInteger(parameters[i])){
					ErrorMessage = "\"" + parameters[i] + "\" is not an integer";
					return false;
				}
				sum += atoi (parameters[i].c_str());
				if(!IsDouble(parameters[i+1])){
					ErrorMessage = parameters[i] + " is not an Price(double).";
					return false;
				}
			}
		}
		if(!IsDivisibleofPower(sum, 2)){
			ErrorMessage = "Sum of concentration values are not a power of two.";
			return false;
		}
		break;
	case NRT_ISI:
		for(unsigned int i =0;  i < parameters.size(); ++i){
			if(!IsInteger(parameters[i])){
				ErrorMessage = "\"" + parameters[i] + "\" is not an integer";
				return false;
			}
		}
		break;
	case ISINCKU: //TODO:: Check OutOfRange validation.
		for(unsigned int i =0;  i < parameters.size(); ++i){
			if(!IsInteger(parameters[i])){
				ErrorMessage = "\"" + parameters[i] + "\" is not an integer";
				return false;
			}
		}
		break;
	case BINARYSEARCH: 	//tolerance, desiredConcentration, Reagent Concentration, Buffer Concentration.
		if(parameters.size()<2){
			ErrorMessage = "Incorrect number of parameters.";
			return false;
		}
		for( unsigned int i= 0; i < parameters.size(); ++i) {
			if(!IsDouble(parameters[i])){
				ErrorMessage = parameters[i] + " is not a double.";
				return false;
			}
		}

		break;
	case IDMA:/*numOps, tolerance, DesiredConcentrate*/
	case DMRW:
		if(parameters.size()<3){
			ErrorMessage = "Incorrect number of parameters.";
			return false;
		}
		if(!IsInteger(parameters[0])) {
			ErrorMessage = "NumOps is not an integer.";
			return false;
		}
		for( unsigned int i= 1; i < parameters.size(); ++i) {
			if(!IsDouble(parameters[i])){
				ErrorMessage = parameters[i] + " is not a double.";
				return false;
			}
		}
		break;
	case MTC:

		if(IsInteger(parameters[0])){
			denom = atoi(parameters[0].c_str());
			if(!IsDivisibleofPower(denom, 2)){
				ErrorMessage = "Denominator: " + parameters[0] + " needs to be a power of 2.";
				return false;
			}
		}
		else {
			ErrorMessage = parameters[0] + "is not an integer.";
			return false;
		}


		for(unsigned int i = 1; i<parameters.size(); ++i) {
			if(!IsInteger(parameters[i])) {
				ErrorMessage = "\"" + parameters[i] + "\" is not an integer.";
				return false;
			}
			if(denom <= atoi(parameters[i].c_str())){
				ErrorMessage = "The numerator: " + parameters[i] + " needs to be smaller than the denominator " + parameters[0] + ".";
				return false;
			}
		}

		break;
	default:
		break;
	}
	return true;
}

DilutionAlgorithms GetDilutionAlgorithm(string s)
{
	if(s.find("MINMIX") != string::npos || s.find("MinMix") != string::npos || s.find("Min Mix") != string::npos || s.find("minmix") != string::npos || s.find("min mix") != string::npos)
		return MINMIX;
	if(s.find("REMIA") != string::npos || s.find("Remia") != string::npos)
		return REMIA;
	if(s.find("GORMA") != string::npos || s.find("Gorma") != string::npos)
		return GORMA;
	if(s.find("WARA") != string::npos || s.find("Wara") != string::npos)
		return WARA;
	if(s.find("GDA") != string::npos)
		return GDA;
	if(s.find("CODOS") != string::npos || s.find("codos") != string::npos)
		return CODOS;
	if(s.find("NRT_ISI") !=  string::npos || s.find("NRTISI") != string::npos)
		return NRT_ISI;
	if (s.find("ISINCKU") != string::npos || s.find("ISI_NCKU") != string::npos)
		return ISINCKU;
	if(s.find("IDMA") != string::npos || s.find("idma") != string::npos)
		return  IDMA;
	if(s.find("BINARYSEARCH") != string::npos || s.find("binary") != string::npos || s.find("Griffith") != string::npos)
		return BINARYSEARCH;
	if(s.find("DMRW") != string::npos || s.find("dmrw") != string::npos)
		return DMRW;
	if(s.find("MTC") != string::npos || s.find("mtc") != string::npos)
		return MTC;
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
	if(s.find("DILUTGEN") != string::npos || s.find("dilut") != string::npos || s.find("FLAT") != string::npos )
		return FLATFILE;
	if(s.find("DOT") != string::npos || s.find("dot") != string::npos)
		return DOTY;
	return OUPUT_NOT_FOUND;
}

inline bool IsInteger(const std::string & s)
{
	if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;

	char * p ;
	strtol(s.c_str(), &p, 10) ;

	return (*p == 0) ;
}
inline bool IsDouble(const std:: string & s)
{
	char *endptr = NULL;

	errno = 0;  // clear errno
	strtof(s.c_str(), &endptr);  // try to convert input as base 10
	if (errno == ERANGE || *endptr != '\0')
		return false;

	return true;
}
inline bool IsDivisibleofPower(int n, int powerOf){
	if(n==0) return false;
	while (n % powerOf == 0) {
		n /= powerOf;
	}
	return n == 1;
}
