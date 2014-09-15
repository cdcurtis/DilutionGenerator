#include "../Headers/DagGen.h"
//#include "../Headers/Fraction.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

using namespace std;

Vertex * DagGen :: getVertex(int id)
{
	for(int i = 0; i < vertices.size(); ++i) {
		if(vertices[i]->uniqueID == id)
			return vertices[i];
	} 
	return NULL;
}
map<int, vector<int> > DagGen :: createDependencyMap()
{
	map <int, vector<int> > childrenParents;
	map <int, vector<int> > :: iterator it;
	
	for(int i = 0 ; i <edges.size(); ++i){
		it = childrenParents.find(edges[i]->child);
		if(it == childrenParents.end()){
			  vector<int> t;
			t.push_back(edges[i]->parent);
			childrenParents.insert(  pair<int,   vector<int> >(edges[i]->child, t));
		}
		else
			it->second.push_back(edges[i]->parent);
	}
	
	return childrenParents;
}

bool DagGen :: isEmpty()
{
	return (vertices.size() == 0 || edges.size() == 0);
}

bool DagGen :: isValidSingleReactantDilution(vector<double> endConcentrations, int base)
{
	map <int, string> namedNodesByID;
	map <int, vector<int> > childrenParents;
	map <int, vector<int> > :: iterator it;
	map <int, ValidationNodeSingleReactant>  NodeIDConcentrations;
	
	vector <int> outputIDs;
	
	childrenParents = createDependencyMap();
	
	for(int i = 0; i < vertices.size(); ++i){
		if(vertices[i]->type == DISPENSE){
		
			namedNodesByID.insert(  pair<int,   string>(vertices[i]->uniqueID,vertices[i]->label));
			if(vertices[i]->label.find("buffer") == -1){
				NodeIDConcentrations.insert(std::pair <int, ValidationNodeSingleReactant>(vertices[i]->uniqueID, ValidationNodeSingleReactant(1.0,base)));
			}
			else{
				NodeIDConcentrations.insert(std::pair <int, ValidationNodeSingleReactant>(vertices[i]->uniqueID, ValidationNodeSingleReactant(0.0,0)));	
			}
		}
		if(vertices[i]->type == OUTPUT)
			outputIDs.push_back(vertices[i]->uniqueID);
	}
	
	bool changes = true;
	//calculate all concentration values.
	while(changes){
		changes =false;
		for (it = childrenParents.begin(); it != childrenParents.end(); ++it) {
			
			//look at parents if all parents have concentration values. calculate new concentration value.
			//if 1 parent pass concentration to child.
			//if 2 parents add both and divide by 2.
			
			int parentsSize = it->second.size();
			
			if(parentsSize == 1 && NodeIDConcentrations.find(it->second[0]) != NodeIDConcentrations.end() && NodeIDConcentrations.find(it->first) == NodeIDConcentrations.end()){
			    changes = true;
				NodeIDConcentrations.insert(std::pair<int,ValidationNodeSingleReactant> (it->first , NodeIDConcentrations.find(it->second[0])->second));
			}
			else if(parentsSize == 2 && NodeIDConcentrations.find(it->second[0]) != NodeIDConcentrations.end()&& NodeIDConcentrations.find(it->first) == NodeIDConcentrations.end() && NodeIDConcentrations.find(it->second[1]) != NodeIDConcentrations.end()) {
				changes = true;
				int p1Base = NodeIDConcentrations.find(it->second[0])->second.checkRoundoff;
				int p2Base = NodeIDConcentrations.find(it->second[1])->second.checkRoundoff;
				int checkRoundoff = p1Base +p2Base;
				double newC;
				 //this will round the concentration value and match what the algorithms are doing.
				 //otherwise every the dag will not be able to be validated.
				if( checkRoundoff%2 != 0 ){
					int newRoundoff = checkRoundoff/2; 
					newC = (double)newRoundoff/(double)base;
				}
				else{
					double p1 = NodeIDConcentrations.find(it->second[0])->second.concentration;
					double p2 = NodeIDConcentrations.find(it->second[1])->second.concentration;
					newC = (p1 + p2)/2.0;
				}
				
				NodeIDConcentrations.insert(std::pair<int,ValidationNodeSingleReactant> (it->first , ValidationNodeSingleReactant(newC, checkRoundoff/2)));				
			}
		}
	}
	
	for(int i = 0;  i < outputIDs.size(); ++i)
	{
		if( NodeIDConcentrations.find( outputIDs[i] )!= NodeIDConcentrations.end()) {
			double con =  NodeIDConcentrations.find( outputIDs[i] )->second.concentration;
			for(int j = 0; j <endConcentrations.size(); ++j)
			{
				if(endConcentrations[j]==con)
					endConcentrations.erase(endConcentrations.begin()+j);
			}
		}
	}
	return endConcentrations.size()==0;
	

}

/*bool DagGen :: isValidMultipleReactantDilution(vector<int> ratio)
{
	map <int, string> namedNodesByID;
	map <int, vector<int> > childrenParents;
	map <int, vector<int> > :: iterator it;
	map <int, vector<Fraction> >  NodeIDConcentrations;
	
	Vertex *outputID;
	//This creates a dependency map to check when the child node can be created.
	
	childrenParents = createDependencyMap();
	
	
	for(int i = 0; i < vertices.size(); ++i){ //issue an ID to every dispense Node.
		if(vertices[i]->type == DISPENSE){
		
			namedNodesByID.insert(  pair<int,   string>(vertices[i]->uniqueID,vertices[i]->label));
			if(vertices[i]->label.find("buffer") == -1){
				NodeIDConcentrations.insert(std::pair <int, ValidationNodeSingleReactant>(vertices[i]->uniqueID, ValidationNodeSingleReactant(1.0,base)));
			}
			else{
				NodeIDConcentrations.insert(std::pair <int, ValidationNodeSingleReactant>(vertices[i]->uniqueID, ValidationNodeSingleReactant(0.0,0)));	
			}
		}
		if(vertices[i]->type == OUTPUT)
			outputID = vertices[i];
	}
}*/
	
Vertex * DagGen :: addVertex(VertexType vertexType,   string label)
{
	Vertex* v =  new Vertex(vertexType,label,IDs++);
	vertices.push_back(v);
	return v;
}
	
void DagGen :: addEdge (int parent, int child)
{
	edges.push_back(new Edge(parent,child));
}

void DagGen :: addEdge (Vertex* parent, Vertex* child)
{
	edges.push_back(new Edge(parent->uniqueID,child->uniqueID));
}



void DagGen :: generateJSON( string fileName)
{
	std:: ostream& out = (fileName != "") ? *(new std::ofstream(fileName.c_str())) : std::cout; 
	
	out << "{ \"dag\": {\n";
	
	out << "\t \"vertices\": [\n";
	for(int i = 0; i < vertices.size(); i++){
		out << "\t\t{ \"Type\" : " << vertices[i]->type << ", \"Label\" : \"" << vertices[i]->label << "\", \"ID\" : " << vertices[i]->uniqueID << "}";
		if(i != vertices.size()-1)
			out<<",";
		out<<"\n";
	}
	out << "\t],\n";
	
	out << "\t \"edges\": [\n";
	for(int i = 0; i < edges.size(); i++){
		out << "\t\t{\"Parent\" : " << edges[i]->parent << ", \"Child\" : " << edges[i]->child << "}";
		if(i != edges.size()-1)
			out<<",";
		out<<"\n";
	}
	out << "\t]\n";
	
	out<<"\t}\n}\n";
	
	if (&out!=&std::cout)  
   		delete(&out); 
}

void DagGen :: generateMCFLOWDag(  string assayName,   string fileName, int volume, int inputTime, int opTime) {
	std:: ostream& out = (fileName != "") ? *(new std::ofstream(fileName.c_str())) : std::cout; 
	
	char buffer[10];
	int inputID = 0;
	int opID = 0;
	  map <int,   string> namedNodesByID;
	  map <int,   vector<int> > childrenParents;
	  map<int,   vector<int> > :: iterator it;
	
	 
	out << "\"assay\":\"" << assayName << "\"\n\n";
	out << "\"bound\":false\n";
	out << "\"scheduledOps\":false\n";
	out << "\"scheduledRoutes\":false\n\n";
	
	out << "\"architecture\":null\n";

	//This creates a dependency map to check when the child node can be created.
	childrenParents = createDependencyMap();
	
	//To ensure dependency order, all dispenses will be declared first.
	for(int i = 0; i < vertices.size(); ++i){
		if(vertices[i]->type == DISPENSE){
			sprintf(buffer,"i%i",inputID++);
			namedNodesByID.insert(  pair<int,   string>(vertices[i]->uniqueID,buffer));
			out << "{ \"objectType\": \"component\", \"name\": \"" << buffer << "\", \"operation\": \"input\", \"executionTime\":" << inputTime << " }\n";
		}
	}
	bool changes = true;
	while(changes){
		changes =false;
		for(int i = 0; i < vertices.size(); ++i) {
			if(vertices[i]->type == MIX ){
				if(namedNodesByID.count(vertices[i]->uniqueID) == 0 ){
					changes = true;
					  vector<int> parents = childrenParents.find(vertices[i]->uniqueID)->second;
					int count = 0;
					for(int i = 0; i < parents.size(); ++i)
						count+= namedNodesByID.count(parents[i]);
					if(count == parents.size()) { //this means both parents have been declared. 
						sprintf(buffer,"O%i",opID++);
						namedNodesByID.insert(  pair<int,   string>(vertices[i]->uniqueID,buffer));
						  string parent1 = namedNodesByID.find(parents[0])->second;
						  string parent2 = namedNodesByID.find(parents[1])->second;
						out << "{ \"objectType\": \"component\", \"name\": \"" << buffer << "\", \"operation\": \"mix\", \"executionTime\":" << opTime << ", \"dependencies\": [{\"name\":\"" << parent1 << "\",\"amount\":"<< volume <<"},{\"name\":\"" << parent2 << "\",\"amount\":"<< volume <<"}] }\n";                          
					}
				}
			}
			if(vertices[i]->type == SPLIT){
				if(namedNodesByID.count(vertices[i]->uniqueID) == 0 ){
					changes = true;
					int parentID = childrenParents.find(vertices[i]->uniqueID)->second[0];
					if(namedNodesByID.count(parentID)!=0){
						  string parentName = namedNodesByID.find(parentID)->second;
						namedNodesByID.insert(  pair<int,   string> (vertices[i]->uniqueID, parentName));
					}
				}	
			}
		}
	}
}

void DagGen :: generateDropletDag(string fileName, int volume, int time )
{
	//if single output name output and waste
	//if  multi out name diff.
	std:: ostream& out = (fileName != "") ? *(new std::ofstream(fileName.c_str())) : std::cout; 

    out << "// DAG Specification for " << dagName << endl;
    if (dagName != "")
        out << "DAGNAME (" << dagName << ")" << endl;
    else
        out << "DAGNAME (DAG)" << endl;

    for (unsigned i = 0; i < vertices.size(); i++)
    {
        //AssayNode *node = dag->getAllNodes().at(i);
        out << "NODE (";

        if (vertices[i]->type == DISPENSE) // Id, Type, FluidName, Volume, Name
            out << vertices[i]->uniqueID << ", DISPENSE, "  << vertices[i]->label/*vertices[i]->portName*/ <<  ", " << volume << ", " << vertices[i]->label << ")\n";
        else if (vertices[i]->type == MIX) // Id, Type, NumDropsBefore, Time (s), Name
            out << vertices[i]->uniqueID << ", MIX, " << 2 <<  ", " << time << ", " << vertices[i]->label << ")\n";
        else if (vertices[i]->type == SPLIT) // Id, Type, NumDropsAfter, Time (s), Name
            out << vertices[i]->uniqueID << ", SPLIT, " << 2 <<  ", " << time << ", " << vertices[i]->label << ")\n";
        else if (vertices[i]->type == OUTPUT || vertices[i]->type == WASTE) // Id, Type, SinkName, Name
            out << vertices[i]->uniqueID << ", OUTPUT, " << "OUTPUT"/*vertices[i]->portName*/ << ", " << vertices[i]->label << ")\n";
        else // Id, Type, Name
            out << vertices[i]->uniqueID << ", GENERAL, " << vertices[i]->label << ")\n";

    }
	for (unsigned c = 0; c < edges.size(); c++)
            out << "EDGE (" << edges[c]->parent << ", " << edges[c]->child << ")\n";
    
    if (&out!=&std::cout)  
   		delete(&out); 
}

void DagGen :: generateDotyGraph(  string fileName)
{
	std:: ostream& out = (fileName != "") ? *(new std::ofstream(fileName.c_str())) : std::cout; 

	out << "digraph G {\n";

	for(int i = 0; i < vertices.size(); ++i)
		out << "\t" << vertices[i]->uniqueID << " [label = \"" << vertices[i]->label << "\"];" << std::endl;

	for(int i = 0; i<edges.size(); ++i)
		out << "\t" << vertices[edges[i]->parent]->uniqueID << " -> " << vertices[edges[i]->child]->uniqueID <<   endl;
	
	out << "}\n";
	
	if (&out!=&std::cout)  
   		delete(&out); 
}

void DagGen::removeEdge(int loc)
{
	edges.erase(edges.begin()+loc, edges.begin()+loc+1);
	return;
}

vector<Vertex*> & DagGen :: Vertices()
{
	return this->vertices;
}

vector<Edge*> & DagGen :: Edges()
{
	return this->edges;
}
std :: string & DagGen :: DagName()
{
	return this->dagName;
}

int DagGen::getID() const
{
	return this->IDs;
}

void DagGen::outputVertices() const
{
	for(unsigned i = 0; i < vertices.size(); i++)
	{
		cout<<"V"<<i<<": "<<vertices.at(i)->label<<", "<<vertices.at(i)->uniqueID<<flush<<endl;
	}
	return;
}

void DagGen::outputEdges() const
{
	for(unsigned i = 0; i < edges.size(); i++)
	{
		cout<<"E"<<i<<": ("<<edges.at(i)->parent<<" -> "<<edges.at(i)->child<<")"<<flush<<endl;
	}
	return;
}

