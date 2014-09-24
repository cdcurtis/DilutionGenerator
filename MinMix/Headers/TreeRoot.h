#ifndef __TREEROOT_H__
#define __TREEROOT_H__

#include "MixTree.h"
#include "../../DagGen/Headers/DagGen.h"

struct leaf {
	MixTree* mt;
	LeafNode ln;
	leaf(MixTree* m): mt(m), ln() {}
	leaf(LeafNode l): mt(NULL), ln(l) {}
};

class TreeRoot{
	MixTree* root;
	Vertex* createDagHelp(DagGen * dag ,leaf mixTree ,std :: vector<std :: string> fluidNames);
public:
	TreeRoot(MixTree* r): root(r) { }

	std::ostream& print (std::ostream &out) {
		return root->print(out,"");
	}	
	DagGen * createDag(std :: vector<std :: string> fluidNames);
};

#endif /*__TREEROOT_H__*/
