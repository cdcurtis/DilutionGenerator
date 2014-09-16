#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include "../Headers/ConcentrationMixMatrix.h"
#include "../Headers/MixTree.h"

using namespace std;


/*
 * Name: ConvertToBinary
 * Takes a number n and converts it to binary of NumDigits length.
 * If the number does not fit, then it returns an empty vector.
 */
vector<int> ConvertToBinary(int n,int numDigits)
{
	vector<int> ret(numDigits);
	for(int i = 0 ; i< numDigits; ++i) {
		if(n!=0){
			if (n & 1)
				ret[(numDigits-1)-i]= 1;
			else
				ret[(numDigits-1)-i]= 0;
			n >>= 1;
		}
		else
			ret[(numDigits-1)-i]= 0;
	}
	if (n !=0 )/*error happened return an empty vector*/
		return vector<int>();
	return ret;
}

/*Creates a ConcentrationMixMatrix:
 * This matrix is a N x d matrix
 * 		N = number of fluids in ratio
 * 		d = Maximum number of binary bits needed to represent any number within the Concentration
 * 	This matrix can then be passed to different mixing algorithms for optimizations.
 */
ConcentrationMixMatrix :: ConcentrationMixMatrix (Concentration c)
{
	int numBits = int(ceil(log2(c.ratioTotal())));	
	for(int i = 0 ; i< c.conNodes().size(); ++i){
		fluidReference.push_back(c.conNodes()[i].fluid);
		mixMatrix.push_back(ConvertToBinary(c.conNodes()[i].ratio,numBits));
	}
}

void combineNodes(vector<MixTree*> & saveList, vector<LeafNode> &combineThis)
{
	MixTree *m;
	while(combineThis.size()>1){
		m = new MixTree(combineThis[combineThis.size()-1],combineThis[combineThis.size()-2]);
		numAllocator++;
		combineThis.pop_back();
		combineThis.pop_back();
		saveList.push_back(m);
	}
}
void combineNodes(vector<MixTree*> & saveList, vector<MixTree*> &combineThis)
{
	MixTree *m;
	while(combineThis.size()>1){
		m = new MixTree(combineThis[combineThis.size()-1],combineThis[combineThis.size()-2]);
numAllocator++;
		combineThis.pop_back();
		combineThis.pop_back();
		saveList.push_back(m);
	}
}
void combineNodes(vector<MixTree*> & saveList, vector<LeafNode> &combineThis, vector<MixTree*> &combineThis2)
{
	MixTree *m;
	while(combineThis.size()>1){
		m = new MixTree(combineThis[combineThis.size()-1],combineThis[combineThis.size()-2]);
numAllocator++;
		combineThis.pop_back();
		combineThis.pop_back();
		saveList.push_back(m);
	}
	while(combineThis2.size()>1){
		m = new MixTree(combineThis2[combineThis2.size()-1], combineThis2[combineThis2.size()-2]);
numAllocator++;
		combineThis2.pop_back();
		combineThis2.pop_back();
		saveList.push_back(m);
	}
	if(combineThis.size()==1 && combineThis2.size() ==1){
		m = new MixTree(combineThis[0],combineThis2[0]);
numAllocator++;
		combineThis.pop_back();
		combineThis2.pop_back();
		saveList.push_back(m);
	}
}

/*
 * Name combineMinMixLevel
 *
 * This takes a list of Nodes at a certain level and returns a new list of MixNodes, which is the
 * combination of that list for the next level up on the mixing tree.
 */
vector<MixTree*> combineMinMixLevel (vector<LeafNode> newLeafNodes, vector<LeafNode> & OldLeafNodes, vector<MixTree*> levelMixes, vector<MixTree*> & oldMixTrees){
	vector<MixTree*> newMixes;
	//Combine all leaves first => add to new mixes
	combineNodes(newMixes,newLeafNodes); // both list can be modified.

	//any left overs add to OldLeaves
	for(int i = 0; i < newLeafNodes.size(); ++i){
		OldLeafNodes.push_back(newLeafNodes[i]);
	}
	//if oldleaves can combine into node =>add to new Mixes

	combineNodes(newMixes,OldLeafNodes);// both list can be modified.

	//Combine all levelMixes

	combineNodes(newMixes,levelMixes);

	//any left overs add to Old trees
	for(int i = 0; i < levelMixes.size(); ++i){
		oldMixTrees.push_back(levelMixes[i]);
	}

	//if old tree can combine with old trees|leaves => add to new Mixes.

	combineNodes(newMixes,OldLeafNodes,oldMixTrees);

	return newMixes;
}


TreeRoot combineMinMixLastLevel (vector<LeafNode> newLeafNodes, vector<LeafNode> & OldLeafNodes, vector<MixTree*> levelMixes, vector<MixTree*> & oldMixTrees)
{			
	vector<MixTree*> lastLevel = combineMinMixLevel(newLeafNodes,OldLeafNodes,levelMixes,oldMixTrees);

	MixTree * ret;
	if(lastLevel.size()!=0){
	    while(lastLevel.size()>1){
	    	MixTree* m  = new MixTree(lastLevel[lastLevel.size()-1],lastLevel[lastLevel.size()-2]);
	    	numAllocator++;
	    	lastLevel.pop_back();
	    	lastLevel.pop_back();
	    	lastLevel.insert(lastLevel.begin(),m);
	    }
	    ret =lastLevel[0];
    	if(OldLeafNodes.size()==1){ //OldLeafNodes can not be greater than 1 becasue of the previous call to combine MinMixLevel.
    		ret = new MixTree(ret,OldLeafNodes[0] );
    		numAllocator++;
    		}
    	if(oldMixTrees.size()==1){
    		ret = new MixTree(ret,oldMixTrees[0]);
    		numAllocator++;
    		}
    }
    else {
        if(OldLeafNodes.size()==1){ //OldLeafNodes can not be greater than 1 becasue of the previous call to combine MinMixLevel.
    		if(oldMixTrees.size()==1){
    		    ret = new MixTree(OldLeafNodes[0],oldMixTrees[0]);
    		    numAllocator++;
    		    }
            else{
                ret =  new MixTree(OldLeafNodes[0],NULL);
                numAllocator++;
                }
            }
        else if(oldMixTrees.size()==1)
   		    ret = oldMixTrees[0];         
        else
        {
            cout<<"Error Occured: NoMixTreeFound.\n"; 
            ret = NULL;
        }
    }
	return TreeRoot(ret);
}



/*
 * Name runMinMix
 *
 * This takes in a mix matrix and looks at the Nth column first to see which fluids can be combined.
 * these Mix Nodes then
 */
TreeRoot ConcentrationMixMatrix :: runMinMix()
{
	vector<LeafNode> newLeaves, oldLeaves;
	vector<MixTree*> newMixTrees, oldMixTrees;

	int size = mixMatrix[0].size()-1;

	for(int j = size ; j >=0;--j ){
		for(int i = 0 ; i< mixMatrix.size() ; ++i) {
			if(mixMatrix[i][j]==1)
				newLeaves.push_back(LeafNode(j,fluidReference[i]));
		}
			newMixTrees = combineMinMixLevel(newLeaves,oldLeaves,newMixTrees, oldMixTrees);//oldLeaves and oldMixTrees can be modified.
			newLeaves.clear();
		}
	return TreeRoot(combineMinMixLastLevel(newLeaves,oldLeaves,newMixTrees,oldMixTrees));
}

void ConcentrationMixMatrix :: print (std::string fileName) 
{
	std:: ostream& out = (fileName != "") ? *(new std::ofstream(fileName.c_str())) : std::cout; 
	
	for(int i=0 ; i<mixMatrix.size() ; ++i){
		for ( int j = 0 ; j< mixMatrix[i].size() ; ++j){
			if( mixMatrix[i].size()==1){
				out <<"|"<<mixMatrix[i][j]<< "|"<<std :: endl;
			}
			else{
				if(j==0)
					out <<"|"<<mixMatrix[i][j]<< ", ";
				else if( j== mixMatrix[i].size()-1)
					out <<mixMatrix[i][j]<< "|"<< std :: endl;
				else
					out <<mixMatrix[i][j]<< ", ";
			}
		}
	}
	
	if (&out!=&std::cout)  
   		delete(&out); 
}
