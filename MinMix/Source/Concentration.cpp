#include "../Headers/Concentration.h"

#include <fstream>
void ConcentrationNode :: print (std :: string fileName)
{
	std:: ostream& out = (fileName != "") ? *(new std::ofstream(fileName.c_str())) : std::cout;
	
	out << "R: " << ratio << " \t Fluid: " << fluid << std :: endl;
	
	if (&out!=&std::cout)  
   		delete(&out);
}

std :: ostream&  ConcentrationNode :: print (std:: ostream&  out)
{
	out << "R: " << ratio << " \t Fluid: " << fluid << std :: endl;
	return out;	
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

void Concentration :: insert(int r, int f)
{
	totalRatio +=r;
	parts.push_back(ConcentrationNode(r,f));
}

void Concentration :: print (std :: string fileName, bool printSimple)
{
	std:: ostream& out = (fileName != "") ? *(new std::ofstream(fileName.c_str())) : std::cout;
	
	if( (!printSimple) && fluidList.size() >= parts.size()) {
		for(int i=0;i<parts.size(); ++i) {
			out << "R: " << parts[i].ratio << " \t Fluid: " << fluidList[parts[i].fluid] << std :: endl;
		}
	}
	else
		for(int i=0;i<parts.size(); ++i) {
			parts[i].print(out);
		}
	if (&out!=&std::cout)  
   		delete(&out); 
}

