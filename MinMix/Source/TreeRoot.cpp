#include <iostream>
#include <math.h>
#include "../Headers/TreeRoot.h"


Vertex* TreeRoot :: createDagHelp(DagGen & dag ,leaf mixTree ,std :: vector<std :: string> fluidNames)
{
	if(mixTree.mt){ 								// checks to see if this leaf is another MixTree or Just a leaf
		Vertex* leftChild = NULL;
		Vertex* rightChild = NULL;
	
		if(mixTree.mt->LChild) 						// Set the Left Child of the MixTree
			leftChild = createDagHelp(dag, leaf(mixTree.mt->LChild), fluidNames);
		else if(mixTree.mt->LNode.level !=-1)
			leftChild = createDagHelp(dag, leaf(mixTree.mt->LNode), fluidNames);
		else{
			std :: cout << "ERROR:\n Left TreeNode Not found.\n";
			mixTree.mt->print(std :: cout,"");
		}

		if(mixTree.mt->RChild)						// Set the Right Child of the MixTree
			rightChild = createDagHelp(dag, leaf(mixTree.mt->RChild), fluidNames);
		else if(mixTree.mt->RNode.level !=-1)
			rightChild = createDagHelp(dag, leaf(mixTree.mt->RNode), fluidNames);
		else{
			std :: cout << "ERROR:\n Right TreeNode Not found.\n";
			mixTree.mt->print(std:: cout,"");
		}
		if(leftChild  && rightChild) { 				//Makes sure Left and Right are not NULL
			Vertex* mix = dag.addVertex(MIX,"Mix");	//Dag Operations
			dag.addEdge(leftChild, mix);
			dag.addEdge(rightChild, mix);

			Vertex* split = dag.addVertex(SPLIT,"Split");
			dag.addEdge(mix, split);

			Vertex* waste = dag.addVertex(WASTE,"Waste");
			dag.addEdge(split, waste);

			return split;							//Returns the tail to build on top of it.
		}
		else
			std :: cout << "ERROR: something went wrong\n";
		return NULL;		
	}
	else if(mixTree.ln.level != -1){						// the MixTree ck failed this makes sure that the leaf is actually a leaf.
		char buffer [50];
		if(fluidNames.size()){								//sets fluid name if one exists
			sprintf(buffer,"%s",fluidNames[mixTree.ln.fluid].c_str());
			return dag.addVertex(DISPENSE,buffer);
		}
		else {												//otherwise make a name
			sprintf(buffer,"fluid%i", mixTree.ln.fluid);
			return dag.addVertex(DISPENSE,buffer);
		}
	}
	else
		std :: cout << "ERROR: something went wrong\n";
	return NULL;
}


DagGen TreeRoot :: createDag(std :: vector<std :: string> fluidNames)
{
	if(!root)
		std :: cout << "ERROR:\nMixTree Not Found. Make sure the tree root is connected to a tree.\n";

	DagGen dag;

	Vertex* parent = createDagHelp(dag, leaf(root), fluidNames);
	Vertex* out = dag.addVertex(OUTPUT,"OUTPUT Concentration");
	dag.addEdge(parent, out);	

	return dag;
}
	
	
	
