#ifndef __WEIGHTEDGRAPH__H_
#define __WEIGHTEDGRAPH__H_

#include "HamiltonianPath.h"
#include "BrujinGraph.h"
#include <iostream>
#include <vector>
#include <queue>


struct WeightedNode {
    BrujinNode * node;
    int weight;
    WeightedNode() : node(NULL), weight(-1) {}
    WeightedNode(BrujinNode* n,int w): node(n), weight(w) {}
    
};

struct WeightedGraphNode{
    BrujinNode* v;
    std::vector<WeightedNode> edges;
    WeightedGraphNode ():v(NULL) {}
    WeightedGraphNode (BrujinNode* n): v(n) { }
 };
 
 /* Class: WeightedGraph
  * This weighted graph class assumes that it is being built from a Brujin Graph, so all
  * vertices need to point to each other.
  */
struct WeightedGraph{
    BrujinGraph *g;
    std::vector<WeightedGraphNode> vertices;
    
    WeightedGraph(BrujinGraph* bg): g(bg) {}
    WeightedGraph(): g(NULL){}
    
    void addVertex(BrujinNode* v);
    
    bool isComplete();
    HamiltonianPath shortestPath (BrujinNode* Start, BrujinNode* end);
    int shortestPathWeight (BrujinNode* Start, BrujinNode* end, HamiltonianPath&);
    void calculateEdges();
    void printMTC(std:: string filename = "");
    
    void printJSON(std:: string filename = "");
    void printDotyGraph (std:: string filename = "", std::string s=""  );
    std::ostream& printMatrix (std::ostream&);
};

#endif /*__WEIGHTEDGRAPH__H_*/