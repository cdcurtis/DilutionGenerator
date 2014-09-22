#ifndef __BRUJINGRAPH__H_
#define __BRUJINGRAPH__H_

#include <iostream>
#include <vector>

struct BrujinNode {
    int name;
    BrujinNode* lChild;
    BrujinNode* rChild;
    
    BrujinNode (): name(-1), lChild(NULL) ,rChild(NULL) { }
    BrujinNode (int n): name(n), lChild(NULL), rChild(NULL) { }
    
    std::ostream& print (std::ostream &out, std::vector<int> &seen);       
};

struct BrujinGraph {
    BrujinNode* root;
    std::vector<BrujinNode*> allNodes;
    //n will be the limit of the graph.
    BrujinGraph(int n);
    BrujinGraph(): root(NULL) {}
    
    std::ostream& print (std::ostream &out );
};

#endif /*__BRUJINGRAPH__H_*/