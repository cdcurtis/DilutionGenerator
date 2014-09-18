#ifndef __DAGGEN__H_
#define __DAGGEN__H_

#include <iostream>
#include <vector>
#include <map>

enum VertexType {DISPENSE, MIX, SPLIT, WASTE, OUTPUT};

struct ValidationNodeSingleReactant{
	double concentration;
	int checkRoundoff;
	ValidationNodeSingleReactant(double c, int cR): concentration(c), checkRoundoff(cR) {}
};

struct Vertex{
	VertexType type;
	std :: string label;
	std :: string portName;
	int uniqueID;
	Vertex(VertexType t, std :: string l, int id): type(t), label(l), portName(""), uniqueID(id) {}
	Vertex(VertexType t, std :: string l, std :: string pN, int id): type(t), label(l), portName(pN), uniqueID(id) {}
};

struct Edge{
	int parent;
	int child;

	Edge (int p, int c):parent(p), child(c) { }
};

class DagGen {
	std :: string dagName;
	std :: vector<Vertex*> vertices;
	std :: vector<Edge*>  edges;
	int IDs;
	 
	std::map<int, std :: vector<int> > createDependencyMap();
public:
	DagGen():dagName(""), IDs(0) {}
	~DagGen()
	{
		for(int i =0; i<vertices.size(); ++i)
			delete vertices[i];
		for(int i =0; i< edges.size(); ++i)
			delete edges[i];
	}
	
	Vertex * addVertex(VertexType, std :: string );
	void removeVertex(int Vid);
	bool isEmpty();
	bool isValidSingleReactantDilution(std :: vector<double>, int);
	//bool isValidMultipleReactantDilution(std :: vector<int>);

	void addEdge (int , int );
	void addEdge (Vertex* , Vertex* );
	void removeEdge(int);
	
	void generateJSON(std :: string = "");
	void generateDotyGraph(std :: string = "");

//  Microfluidic Architectures 
	void generateDropletDag(std :: string fileName = "", int volume = 10, int time = 5);
	void generateMCFLOWDag(std :: string assayName = "Assay", std :: string fileName = "", int volume = 10, int inputTime = 0, int opTime = 5);
	
	//	getters/setters
	Vertex * getVertex(int id); 
	std :: vector <Vertex*> & Vertices();
	std :: vector <Edge*> & Edges();
	std :: string & DagName();
	std::vector<Edge*> findOutgoingEdges(int Vid) const;
	std::vector<Edge*> findIncomingEdges(int Vid) const;
	int getID() const;

	int calcNumWaste() const;
	int calcNumMixSplits() const;

//	I/O
	void outputVertices() const;
	void outputEdges() const;
};

#endif /*__DAGGEN__H_*/
