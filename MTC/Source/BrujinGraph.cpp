#include "../Headers/BrujinGraph.h"
#include <math.h>

using namespace std;

std::ostream& BrujinNode :: print (std::ostream &out, vector<int> &seen)
{
    for(int i=0 ; i < seen.size(); ++i){
        if(seen[i] == name)
            return out;
        }
    seen.push_back(name);
    out<< name << "\n\t" << lChild->name << " " << rChild->name << endl;
    lChild->print(out, seen);
    rChild->print(out, seen);
    return out;
}

//n will be the limit of the graph.
BrujinGraph :: BrujinGraph(int n) 
{
    vector<BrujinNode*> holder;
    for(int i= 0 ; i<n;++i)
        holder.push_back(new BrujinNode(i));
    
    // fill children.
    int shiftAmt= int(ceil(log2(n)));
    int add1 = 1;
    for(int i= 0; i< shiftAmt-1; ++i)
        add1 = add1 << 1;

    for(int i=0; i<n ; ++i){
        int shiftedBy0 = i >> 1;
        int shiftedBy1 = ((i >> 1) + add1)%n;
            
        holder[i]->lChild = holder[shiftedBy0];
        holder[i]->rChild = holder[shiftedBy1];
    }
    root = holder[0];
    allNodes = holder;
}
    
std::ostream& BrujinGraph :: print (std::ostream &out )
{
    vector<int> temp;
    return root->print(out,temp);
}


