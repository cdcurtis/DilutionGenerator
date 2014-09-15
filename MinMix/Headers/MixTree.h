#ifndef __MIXTREE_H__
#define __MIXTREE_H__

struct LeafNode{
	int level;
	int fluid;
	LeafNode():level(-1),fluid(-1) {}
	LeafNode(int l, int f) :level(l),fluid(f){}

	bool isEmpty()
	{
		return level ==-1;
	}
};
static int numAllocator = 0;
struct MixTree{
	//int NodeIDs;
	
	MixTree* LChild;
	MixTree* RChild;
	LeafNode LNode;
	LeafNode RNode;
	//int UID;

	MixTree(): LChild(NULL), RChild(NULL), LNode(), RNode(){}
	MixTree(LeafNode L, LeafNode R):LChild(NULL), RChild(NULL), LNode(L), RNode(R){}
	MixTree(MixTree* L, MixTree* R):LChild(L), RChild(R), LNode(), RNode() {}
	MixTree(MixTree* L, LeafNode R):LChild(L), RChild(NULL), LNode(), RNode(R) {}
	MixTree(LeafNode L, MixTree* R):LChild(NULL), RChild(R), LNode(L), RNode() {}
    ~ MixTree()
    {
        this->deleteMe();
    }   

	bool isEmpty(){
		return  ((!LChild && LNode.isEmpty())&&(!RChild && RNode.isEmpty()));
	}


	std::ostream& print (std::ostream &out , std :: string sp){
		std :: string stem = sp +"  ";
		if(LChild)
			LChild->print(out, stem);
		else
			out<<stem<</* "L: " << LNode.level << "fluid: " << */LNode.fluid << std :: endl;

		out <<sp << "T:"/*<<UID */<< std :: endl;

		if (RChild)
			RChild->print(out, stem);
		else
			out<<stem<< /*"L: " << RNode.level << "fluid: " <<*/ RNode.fluid << std :: endl;

		return out;
	}
	void deleteMe(){
	    if(LChild){
	        LChild->deleteMe();
	       // delete LChild;
	    }
	    if(RChild){
	        RChild->deleteMe();
	        //delete RChild;
	    }
	    delete this;
	    numAllocator--;
	    return;
	}
};

#endif /*__MIXTREE_H__*/