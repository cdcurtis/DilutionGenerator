#include "../../Remia/Headers/Remia.h"
#include <vector>

typedef vector< pair<node*, node*> > RCP;	//RCP - Replacement Candidate Pair

class Wara
{
private:
	bool found;	//flag being used to find the parent node for a given node. Part of maximal droplet shraing function logic
	int uid;
	string b, r, w, m, s, o;
	int b_count, r_count, w_count, m_count, s_count, o_count;
	vector < node* > SMT;	//Forest to hold the mixing tree for each PCV.

	Wara();
	node* Find_parent(node *T, node *p, node *m);
	bool checkIfUniqueNode(priority_queue<node*, vector<node*>, comparator> Q, node *n);
	void generate_stack(node *T, priority_queue<node*, vector<node*>, comparator>& sk);
	void Maximal_Droplet_Sharing();

	bool checkIfUniqueNodeByValue(priority_queue<node*, vector<node*>, comparator> Q, node *n);
	void GetRCPs(vector < node* > G, RCP &rcp);
	int GetGain(vector < node* > G, node *x, node *y, int eru);
	void SetACs(RCP &rcp);
	void ResetACs(RCP &rcp);
	void GetUniqueness(RCP &rcp, vector < int >& uniq);
	bool NotInSubtree(node *xy, node *z);
	int ERU(vector < node* > G);
	void Droplet_Replacement(vector < node* >& SMT);

	Vertex* createVertex(int &count, string str,DagGen &dag, VertexType type);
	void convertDataStructureForPCV(node *T, DagGen &dag, stack< Vertex *>& vertices, node *root);
	Vertex* getCorrespondingVertex(node *T, stack< Vertex *>& vertices, VertexType type);
	bool checkIfUniqueNodeInStack(stack< node * > Q, node *n);
	void convertDataStructureForMixingTree(stack < node* > Q, DagGen &dag, stack< Vertex *>& vertices, vector < CV* > PCV);
public:
	static DagGen RunWara(int argc, char* argv[]);
};
