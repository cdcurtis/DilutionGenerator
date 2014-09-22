#include "../Headers/WeightedGraph.h"

#include <fstream>
#include <map>
using namespace std;
void WeightedGraph :: addVertex(BrujinNode* v)
{
    vertices.push_back(WeightedGraphNode(v));
}
    
bool WeightedGraph :: isComplete()
{
    int numVert = vertices.size();
    for (int i = 0 ; i < numVert ; ++i){
        if(vertices[i].edges.size() != numVert-1)
            return false;
    } 
    return true;
}
HamiltonianPath WeightedGraph :: shortestPath (BrujinNode* Start, BrujinNode* end)
{
	HamiltonianPath ret;
	shortestPathWeight(Start,end,ret);
	return ret;
}


int WeightedGraph :: shortestPathWeight (BrujinNode* Start, BrujinNode* end, HamiltonianPath& hp)
{	
    queue<WeightedNode> worker;
    queue<HamiltonianPath> path;
    map<int,int> seenNode;
    seenNode.insert(pair<int,int> (Start->name,0));
   
    HamiltonianPath hamPath(Start->name);
   	path.push(hamPath);
    
    if(Start->name == end->name){
    	hp = hamPath;
        return 0;
    }
    if(Start->lChild->name == end->name){
    	hamPath.AddToPath(Start->lChild->name,0);
    	hp = hamPath;
    	return 1;
    }
    else if(Start->rChild->name == end->name){
       	hamPath.AddToPath(Start->rChild->name,1); 
       	hp = hamPath;   
        return 1;
   }
    hamPath = path.front();
    HamiltonianPath hamPath2(hamPath);
    hamPath.AddToPath(Start->lChild->name,0);
    hamPath2.AddToPath(Start->rChild->name,1);
    path.pop();
    path.push(hamPath);
    path.push(hamPath2);
    
    worker.push(WeightedNode(Start->lChild,1));
    worker.push(WeightedNode(Start->rChild,1));
    
    while(!worker.empty()){
    	HamiltonianPath temp2 = path.front(); 
        WeightedNode temp = worker.front();
        if (seenNode.find(temp.node->name) == seenNode.end()) {
            seenNode.insert(pair<int,int> (temp.node->name, temp.weight));
            if(temp.node->lChild->name == end->name){
            	temp2.AddToPath(temp.node->lChild->name,0);
            	hp = temp2;
            	return temp.weight+1;
            }
            else if( temp.node->rChild->name == end->name){	
	            temp2.AddToPath(temp.node->rChild->name,1);
                hp = temp2;
                return temp.weight+1;
            }
            else{
            	HamiltonianPath temp3(temp2);
            	temp2.AddToPath(temp.node->lChild->name,0);
    	        temp3.AddToPath(temp.node->rChild->name,1);
    	        
    	        path.push(temp2);
    	        path.push(temp3);
                worker.push(WeightedNode(temp.node->lChild,temp.weight+1));
                worker.push(WeightedNode(temp.node->rChild,temp.weight+1));
            } 
        }
        worker.pop();
        path.pop();         
    }
    return -1;    
}

void WeightedGraph :: calculateEdges()
{
    for(int i = 0 ; i < vertices.size(); ++i){
        //cout<< "Node: " << vertices[i].v->name<<endl;
        vertices[i].edges.clear();
        for(int j = 0 ; j < vertices.size(); ++j){
            if(vertices[i].v->name != vertices[j].v->name){
            	HamiltonianPath shortestPath;
                int weight = shortestPathWeight(vertices[i].v, vertices[j].v, shortestPath);
                //cout<< "\t" << vertices[j].v->name<<": "<<weight<<endl;
                vertices[i].edges.push_back(WeightedNode(vertices[j].v, weight));
            }
        }    
    }
}   
void WeightedGraph :: printMTC(std:: string fileName)
{
	std:: ostream& out = (fileName != "") ? *(new std::ofstream(fileName.c_str())) : std::cout; 
	out <<"SIZE" <<endl;
	out << g->allNodes.size()<<endl;
	
	out<< "Verticies" <<endl;
	for(int i = 0 ; i < vertices.size(); ++i)
		out << vertices[i].v->name << endl ;
	
	if (&out!=&std::cout)  
   		delete(&out); 

}


void WeightedGraph :: printJSON(std:: string fileName)
{
	std:: ostream& out = (fileName != "") ? *(new std::ofstream(fileName.c_str())) : std::cout; 

	out << "{ \"WeightedBrujin\" : {\n";
	
	out << "\t\"BrujinGraph\" : {\n";
	out << "\t\t\"root : " << g->root->name << ",\n";
	out << "\t\t\"nodes\" : [\n";
	for(int i=0; i < g->allNodes.size(); ++i) {
		out << "\t\t\t{ \"name\" : " << g->allNodes[i]->name << " , \"lChild\" : " <<  g->allNodes[i]->lChild->name << " , \"rChild\" : " <<  g->allNodes[i]->rChild->name << "}";
		if(i!=g->allNodes.size()-1)
			out<<",";
		out<<endl;
	}
	out <<"\t\t]\n\t}\n";

	out << "\t\"WeightMatrix\" : {\n";
	out << "\t\t\"ColumnNames\" : [";
	for(int i = 0 ; i < vertices.size(); ++i){
		out << vertices[i].v->name;
		if(i != vertices.size()-1)
			out<<", ";
	}
	out << "],\n";
	out << "\t\t\"Row\" : [\n";
	 for(int i = 0 ; i < vertices.size(); ++i){
	 	out << "\t\t{\n";
	 	out << "\t\t\t\"Name\" : " << vertices[i].v->name <<",\n\t\t\t\"Values\" : [";
        for(int j = 0 ; j < vertices[i].edges.size(); ++j){
        	if(i==j)
        		out<<"0, ";
			out << vertices[i].edges[j].weight;
				
			if(j != vertices[i].edges.size()-1)
				out<<", ";
		}
		if(i==  vertices.size()-1)
        	out<<", 0";
		out<< "]\n\t\t}";
		if(i != vertices.size()-1)
			out<<", ";
		out<<endl;		
    }
	out << "\t\t]\n";
		
	out<< "\t}\n}}\n";
	if (&out!=&std::cout)  
   		delete(&out); 
}


void WeightedGraph :: printDotyGraph (std:: string fileName, std::string s)
{
	std:: ostream& out = (fileName != "") ? *(new std::ofstream(fileName.c_str())) : std::cout; 
	if(s=="")
		s="WeightedGraph";
	out << "digraph "<< s <<" {\n";
    for(int i = 0 ; i < vertices.size(); ++i){
        //out<< "Node: " << vertices[i].v->name<<endl;
        for(int j = 0 ; j < vertices[i].edges.size(); ++j){
        	out<< vertices[i].v->name << " -> " << vertices[i].edges[j].node->name << " [label = "<<vertices[i].edges[j].weight <<"]"<<endl;
          //  out<< "\t" << vertices[i].edges[j].node->name<<": "<<vertices[i].edges[j].weight<<endl;
        }    
    }
    out << "}\n";
    if (&out!=&std::cout)  
   		delete(&out); 
}
std::ostream& WeightedGraph :: printMatrix (std::ostream & out)
{   
    for(int i = 0 ; i < vertices.size(); ++i){
        //out<< "Node: " << vertices[i].v->name<<endl;
        for(int j = 0 ; j < vertices[i].edges.size(); ++j){
        	if(i==j)
        		out<<"0 ";
            out<<vertices[i].edges[j].weight<<" ";
        }  
        if(i==  vertices.size()-1)
        	out<<"0 ";
        out<<endl;
    }
 	return out;
 }
