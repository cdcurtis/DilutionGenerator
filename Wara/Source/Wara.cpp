#include "../Headers/Wara.h"
#include <limits>

Wara :: Wara()
{
	found = false;
	uid=-1;//cdcurtis

	b = "buffer"; r = "reactant"; w = "waste"; m = "mix"; s = "split"; o = "output";
	b_count = 1; r_count = 1; w_count = 1; m_count = 1; s_count = 1; o_count = 1;
}

bool Wara :: checkIfUniqueNode(priority_queue<node*, vector<node*>, comparator> Q, node *n)
{
	while(!Q.empty())
	{
		if(Q.top() == n)
		{
			return true;
		}
		Q.pop();
	}
	return false;
}

bool Wara :: checkIfUniqueNodeByValue(priority_queue<node*, vector<node*>, comparator> Q, node *n)
{
	while(!Q.empty())
	{
		if(Q.top()->cv->val == n->cv->val)
		{
			return true;
		}
		Q.pop();
	}
	return false;
}

bool Wara :: checkIfUniqueNodeInStack(stack< node * > Q, node *n)
{
	while(!Q.empty())
	{
		if(Q.top() == n)
			return true;
		Q.pop();
	}
	return false;
}

void Wara :: generate_stack(node *T, priority_queue<node*, vector<node*>, comparator>& sk)
{
	if(T != NULL)
	{
		if(T->status == reusable)
		{
			if(!checkIfUniqueNode(sk, T))
				sk.push(T);
		}
		if(T->left) generate_stack(T->left, sk);
		if(T->right) generate_stack(T->right, sk);
		
	}
}

node* Wara :: Find_parent(node *T, node *p, node *m)
{
	if(found)
		return p;

	if(T == m)
	{
		found = true;
		return p;
	}

	if(T->left != NULL)
	{
		node *tmp = Find_parent(T->left, T, m);
		if(tmp != NULL) 
			return tmp;
	}

	if(T->right != NULL) 
	{	
		node *tmp = Find_parent(T->right, T, m);
		if(tmp != NULL) 
			return tmp;
	}

	return NULL;
}

void Wara :: Maximal_Droplet_Sharing()
{
	stack <node *> node_stack;
	priority_queue<node*, vector<node*>, comparator> tmp_node_stack;
	node *T;

	for(int i=0; i<SMT.size(); i++)
	{
		T = SMT[i];
		generate_stack(T, tmp_node_stack);
	}

	while(!tmp_node_stack.empty())
	{
		node_stack.push(tmp_node_stack.top());
		tmp_node_stack.pop();
	}

	node *n;	//node to hold the top node from node_stack
	node *m;	// node to hold the nodes from dummy stack one at a time
	node *parent_n;	//node to hold the parent of the node of n in picture
	node *parent_m;	//node to hold the parent of the node of m in picture

	while(!node_stack.empty())
	{
	begin:
		if(node_stack.empty())
			break;
		n = node_stack.top();

		node_stack.pop();
		for (stack<node *> dummy = node_stack; !dummy.empty(); dummy.pop())
		{
			m = dummy.top();

			if(n->cv->val == m->cv->val)
			{	
				found = false;
				for(int i=0; i<SMT.size(); i++)
				{
					T = SMT[i];
					parent_m = Find_parent(T, NULL, m);
					if(found == true)
						break;
				}

				found = false;
				for(int i=0; i<SMT.size(); i++)
				{
					T = SMT[i];
					parent_n = Find_parent(T, NULL, n);
					if(found == true)
						break;
				}				

				if(parent_m != NULL)
				{
					parent_m->right = n;
					n->status = reused;
				}
				else if(parent_n != NULL)
				{
					parent_n->right = m;
					m->status = reused;
				}
				else
				{
					vector < node* > tmpSMT;
					n->status = both_output;
					for(int i = 0; i<SMT.size(); i++)
						if(SMT[i] != m)
							tmpSMT.push_back(SMT[i]);
					SMT = tmpSMT;
				}

				priority_queue<node*, vector<node*>, comparator> tmp_node_stack;
				stack <node *> temp_stack;
				//Refresh the stack
				for(int i=0; i<SMT.size(); i++)
				{
					T = SMT[i];
					generate_stack(T, tmp_node_stack);
				}

				while(!tmp_node_stack.empty())
				{
					temp_stack.push(tmp_node_stack.top());
					tmp_node_stack.pop();
				}

				node_stack = temp_stack;

				goto begin;
			}
		}
	}
}

bool Wara :: NotInSubtree(node *xy, node *z)
{
	stack < node * > tmp1;
	stack < node * > tmp2;
	tmp1.push(xy);
	while(!tmp1.empty())
	{
		if(tmp1.top()->left == z || tmp1.top()->right == z)
			return true;
		if(tmp1.top()->left != NULL && tmp1.top()->right != NULL)
		{
			tmp2.push(tmp1.top()->left);
			tmp2.push(tmp1.top()->right);
		}
		tmp1.pop();
		if(tmp1.empty())
		{
			while(!tmp2.empty())
			{
				tmp1.push(tmp2.top());
				tmp2.pop();
			}
		}
	}
	return false;
}

void Wara :: GetRCPs(vector < node* > G, RCP &rcp)
{
	priority_queue<node*, vector<node*>, comparator> Q;	//Q for Reusable nodes
	priority_queue<node*, vector<node*>, comparator> RQ;	//RQ holds a copy of Q
	priority_queue<node*, vector<node*>, comparator> NRQ;	//NRQ for Non-Reusable nodes
	priority_queue<node*, vector<node*>, comparator> tmpQ1;
	priority_queue<node*, vector<node*>, comparator> tmpQ2;

	node *x;
	node *y;
	node *z;
	node *parent_x;
	node *parent_y;

	long double z_val;

	for(int i=0; i<G.size(); i++)
	{
		x = G[i];
		while(x != NULL)
		{
			if(x->status == reusable)
				if(!checkIfUniqueNode(Q, x))
					Q.push(x);
			x = x->right;
		}
	}

	RQ = Q;

	for(int i=0; i<G.size(); i++)
	{
		x = G[i];
		while(x != NULL)
		{
			if(x->left != NULL && x->left->status == non_reusable)
			{
				if(!checkIfUniqueNodeByValue(NRQ, x->left))
					NRQ.push(x->left);
			}
			else if(x->status == non_reusable)
			{
				if(!checkIfUniqueNodeByValue(NRQ, x))
					NRQ.push(x);
			}
			x = x->right;
		}
	}

	while(!Q.empty())
	{
		x = Q.top();
		Q.pop();

		if(!Q.empty())
			tmpQ1 = Q;
		else 
			break;

		found = false;
		for(int i=0; i<G.size(); i++)
		{
			parent_x = Find_parent(G[i], NULL, x);
			if(found == true)
				break;
		}

		while(!tmpQ1.empty())
		{	
			y = tmpQ1.top();
			tmpQ1.pop();

			tmpQ2 = RQ;

			found = false;
			for(int i=0; i<G.size(); i++)
			{
				parent_y = Find_parent(G[i], NULL, y);
				if(found == true)
					break;
			}

			z_val = ((long double)x->cv->num + (long double)y->cv->num)/(2*(long double)x->cv->deno);

			while(!tmpQ2.empty())
			{			
				z = tmpQ2.top();
				tmpQ2.pop();

				if(z->cv->val == z_val && parent_x != z && parent_y != z && !NotInSubtree(x,z) && !NotInSubtree(y,z))
				{
					rcp.push_back(make_pair(x,y));
					break;
				}
			}
		}
	}

	Q = RQ;

	while(!RQ.empty())
	{
		x = RQ.top();
		RQ.pop();

		tmpQ1 = NRQ;

		found = false;
		for(int i=0; i<G.size(); i++)
		{
			parent_x = Find_parent(G[i], NULL, x);
			if(found == true)
				break;
		}

		while(!tmpQ1.empty())
		{	
			y = tmpQ1.top();
			tmpQ1.pop();

			tmpQ2 = Q;

			z_val = ((long double)x->cv->num + (long double)y->cv->num)/(2*(long double)x->cv->deno);

			while(!tmpQ2.empty())
			{	
				z = tmpQ2.top();
				tmpQ2.pop();

				if(z->cv->val == z_val && parent_x != z && !NotInSubtree(x,z))
				{	
					rcp.push_back(make_pair(x,y));
					break;
				}

			}
		}
	}

}

int Wara :: ERU(vector < node* > G)
{
	long double total_pcv = 0;
	priority_queue<node*, vector<node*>, comparator> Q;
	node *T;
	for(int i=0; i<G.size(); i++)
	{
		T = G[i];
		while(T != NULL)
		{	
			if(T->left != NULL && T->left->status == non_reusable)
			{
				if(!checkIfUniqueNode(Q, T->left))
					Q.push(T->left);
			}
			else if(T->status == non_reusable)
			{
				if(!checkIfUniqueNode(Q, T))
					Q.push(T);
			}
			T = T->right;
		}
	}

	while(!Q.empty())
	{
		total_pcv += Q.top()->cv->val;
		Q.pop();
	}
	return(ceil(total_pcv));
	
}

void Wara :: SetACs(RCP &rcp)
{
	for(RCP::iterator it = rcp.begin(); it != rcp.end(); ++it)
	{
		node* x = it->first;
		node* y = it->second;
		if(x->status == reusable)
			x->ac++;
		if(y->status == reusable)
			y->ac++;
	}
}

void Wara :: ResetACs(RCP &rcp)
{
	for(RCP::iterator it = rcp.begin(); it != rcp.end(); ++it)
	{
		node* x = it->first;
		node* y = it->second;
		if(x->status == reusable)
			x->ac = 0;
		if(y->status == reusable)
			y->ac = 0;
	}
}

void Wara :: GetUniqueness(RCP &rcp, vector < int >& uniq)
{
	for(RCP::iterator it = rcp.begin(); it != rcp.end(); ++it)
	{
		if(it->first->ac <= it->second->ac)
			uniq.push_back(it->first->ac);
		else
			uniq.push_back(it->second->ac);
	}
}

int Wara :: GetGain(vector < node* > G, node *x, node *y, int eru)
{
	priority_queue<node*, vector<node*>, comparator> Q;	//Q for Reusable nodes
	node *tmp;
	node *z;
	node *tx, *ty;
	long double z_val;
	int tmp_eru;

	for(int i=0; i<G.size(); i++)
	{
		tmp = G[i];
		while(tmp != NULL)
		{
			if(tmp->status == reusable)
				if(!checkIfUniqueNode(Q, tmp))
					Q.push(tmp);
			tmp = tmp->right;
		}
	}

	z_val = ((long double)x->cv->num + (long double)y->cv->num)/(2*(long double)x->cv->deno);

	while(!Q.empty())
	{		
		z = Q.top();
		Q.pop();

		tx = NULL;
		ty = NULL;

		if(z->cv->val == z_val)
		{	
			tx = z->left;
			ty = z->right;

			z->left = x;
			if(y->status == reusable)
				z->right = y;
			else
			{		
				node *tmp_y = new node();
				tmp_y->id = uid++;
				tmp_y->cv = new CV(y->cv->num,y->cv->deno,y->cv->val);
				tmp_y->status = y->status;
				tmp_y->ac = y->ac;
				tmp_y->left = new node();
				tmp_y->right = new node();
				tmp_y->left = y->left;
				tmp_y->right = y->right;

				z->right = tmp_y;
			}
			break;
		}
	}

	tmp_eru = ERU(G);
	if(tx != NULL && ty != NULL)
	{
		z->left = tx;
		z->right = ty;
	}
	else
	{
		z->left = NULL;
		z->right = NULL;
	}
	return(eru - tmp_eru);
}

void Wara :: Droplet_Replacement(vector < node* >& SMT)
{

	int eru, tmp_gain, i;
	int max_gain;
	int min_uniq;
	long double z_val;

	while(1)
	{	
		RCP rcp;
		vector < int > uniq;
		vector < int > gain;
		priority_queue<node*, vector<node*>, comparator> Q;	//Q for Reusable nodes
		node *x, *y, *z, *tmp;

		max_gain = 0;
		min_uniq = numeric_limits<int>::max();
		i=0;
		x = NULL;
		y = NULL;
		z = NULL;
		tmp = NULL;

		GetRCPs(SMT, rcp);

		if(rcp.empty())
			break;	

		SetACs(rcp);

		GetUniqueness(rcp, uniq);

		eru = ERU(SMT);

		for(RCP::iterator it = rcp.begin(); it != rcp.end(); ++it)
		{
			tmp_gain = GetGain(SMT, it->first, it->second, eru);
			gain.push_back(tmp_gain);
		}

		while(!Q.empty())
			Q.pop();

		for(int j=0; j<SMT.size(); j++)
		{
			tmp = SMT[j];
			while(tmp != NULL)
			{
				if(tmp->status == reusable)
					if(!checkIfUniqueNode(Q, tmp))
						Q.push(tmp);
				tmp = tmp->right;
			}
		}
	
		for(RCP::iterator it = rcp.begin(); it != rcp.end(); ++it)
		{
			if(gain[i] > max_gain)
			{
				x = it->first;
				y = it->second;
				max_gain = gain[i];
				min_uniq = uniq[i];
			}
			else if(gain[i] == max_gain && max_gain != 0)
			{
				if(uniq[i] < min_uniq)
				{
					x = it->first;
					y = it->second;
					min_uniq = uniq[i];
				}
			}
			i++;
		}

		if(x != NULL && y != NULL)
		{
			z_val = ((long double)x->cv->num + (long double)y->cv->num)/(2*(long double)x->cv->deno);

			while(!Q.empty())
			{		
				z = Q.top();
				Q.pop();

				if(z->cv->val == z_val)
				{
					z->left = x;
					x->status = reused;
					if(y->status == reusable)
					{
						z->right = y;
						y->status = reused;
					}
					else
					{		
						node *tmp_y = new node();
						tmp_y->id = uid++;
						tmp_y->cv = new CV(y->cv->num,y->cv->deno,y->cv->val);
						tmp_y->status = y->status;
						tmp_y->ac = y->ac;
						tmp_y->left = new node();
						tmp_y->right = new node();
						tmp_y->left = y->left;
						tmp_y->right = y->right;
	
						z->right = tmp_y;
					}
					break;
				}
			}
			ResetACs(rcp);
		}
		else
			break;

	}
}

Vertex* Wara :: createVertex(int &count, string str, DagGen *dag, VertexType type)
{
	ostringstream oss;
	Vertex* v;

	oss << count;
	str += oss.str();
	count++;
	v = dag->addVertex(type, str);
	return (v);
}

void Wara :: convertDataStructureForPCV(node *T, DagGen *dag, stack< Vertex *>& vertices, node *root)
{
	Vertex* vr;
	Vertex* vm;
	Vertex* vs;
	Vertex* vb;
	Vertex* vw;

	if(T == NULL)
		return;

	if(T->cv->val == 1.0)
	{
		vr = createVertex(r_count, r, dag, DISPENSE);
		T->dag_uid.push(vr->uniqueID);
		vertices.push(vr);
	}
	else
	{
		//Create a buffer vertex
		vb = createVertex(b_count, b, dag, DISPENSE);
		//Create a Mix vertex
		vm = createVertex(m_count, m, dag, MIX);
		//Connect the buffer vertex to the Mix vertex.
		dag->addEdge(vb, vm);
		found = false;
		node *parent = Find_parent(root, NULL, T);
		for(stack <Vertex *> dummy = vertices; !dummy.empty(); dummy.pop())
		{
			vr = NULL; //cdcurtis
			if(parent->dag_uid.top() == dummy.top()->uniqueID)
			{
				vr = dummy.top();
				break;
			}
		}
		//Connect the reagent/previous intermediate node to the current node's mix
		dag->addEdge(vr, vm);
		//Create a Split vertex
		vs = createVertex(s_count, s, dag, SPLIT);
		//Connect the Mix vertex to the Split vertex.
		dag->addEdge(vm, vs);
		vertices.push(vs);
		T->dag_uid.push(vs->uniqueID);

		if((T->left == NULL && T->right != NULL) || (T->left != NULL && T->right == NULL) || (T->status != both_output && T->left == NULL && T->right == NULL))
		{
			//Create a Waste vertex
			vw = createVertex(w_count, w, dag, WASTE);
			//Connect the Split vertex to the Waste vertex.
			dag->addEdge(vs, vw);
		}
	}

	convertDataStructureForPCV(T->left, dag, vertices, root);
	convertDataStructureForPCV(T->right, dag, vertices, root);
}

Vertex* Wara :: getCorrespondingVertex(node *T, stack< Vertex *>& vertices, VertexType type)
{
	for(stack < int > dump = T->dag_uid; !dump.empty(); dump.pop())
	{
		for(stack <Vertex *> dummy = vertices; !dummy.empty(); dummy.pop())
		{
			if(dump.top() == dummy.top()->uniqueID && type == dummy.top()->type)
				return dummy.top();
		}
	}
	return NULL;
}

void Wara :: convertDataStructureForMixingTree(stack < node* > Q, DagGen *dag, stack< Vertex *>& vertices, vector < CV* > PCV)
{
	node *x;

	Vertex* vr;
	Vertex* vm;
	Vertex* vs;
	Vertex* vb;
	Vertex* vw;
	Vertex* vo;
	Vertex* v1;
	Vertex* v2;

	stack < node* > tmpQ = Q;

	while(!Q.empty())
	{
		x = Q.top();
		Q.pop();

		//Create a Mix vertex
		vm = createVertex(m_count, m, dag, MIX);
		vertices.push(vm);

		//Create a Split vertex
		vs = createVertex(s_count, s, dag, SPLIT);
		vertices.push(vs);

		x->dag_uid.push(vm->uniqueID);
		x->dag_uid.push(vs->uniqueID);

		//Connect the Mix vertex to the Split vertex.
		dag->addEdge(vm, vs);

		if(x->target == yes)
		{
			if(x->status == both_output)
			{
				Vertex *vo1 = createVertex(o_count, o, dag, OUTPUT);
				//Connect the Split vertex to the OUTPUT1 vertex.
				dag->addEdge(vs, vo1);

				Vertex *vo2 = createVertex(o_count, o, dag, OUTPUT);
				//Connect the Split vertex to the OUTPUT2 vertex.
				dag->addEdge(vs, vo2);
			}
			else
			{
				Vertex *vo = createVertex(o_count, o, dag, OUTPUT);
				//Connect the Split vertex to the OUTPUT vertex.
				dag->addEdge(vs, vo);
			}
		}

		if(x->status == reusable)
		{
			//Create a Waste vertex
			vw = createVertex(w_count, w, dag, WASTE);
			//Connect the Split vertex to the Waste vertex.
			dag->addEdge(vs, vw);
		}
	}

	while(!tmpQ.empty())
	{
		x = tmpQ.top();
		tmpQ.pop();

		vm = getCorrespondingVertex(x, vertices, MIX);

		//Get the left Vertex of the vertex being handled
		for(stack <Vertex *> dummy = vertices; !dummy.empty(); dummy.pop())
		{
			if(x->left->dag_uid.top() == dummy.top()->uniqueID)
			{
				v1 = dummy.top();
				break;
			}
		}

		//Connect the reactant vertex to the Mix vertex.
		dag->addEdge(v1, vm);

		if(x->right->status == non_reusable)
		{
			//Get the right Vertex of the vertex being handled
			for(stack <Vertex *> dummy = vertices; !dummy.empty(); dummy.pop())
			{
				if(x->right->dag_uid.top() == dummy.top()->uniqueID)
				{
					v2 = dummy.top();
					break;
				}
			}
		}
		else
		{
			v2 = getCorrespondingVertex(x->right, vertices, SPLIT);
		}

		//Connect the reactant vertex to the Mix vertex.
		dag->addEdge(v2, vm);
	}

}

DagGen * Wara :: RunWara(int argc, char* argv[])
{//numerator1, numerator2, numerator3, numerator4, numerator5, ....., denominator -> All the target CV's should be represented in the numerator/denominator format with common denominator. E.g., {1/1024, 506/1024, 1023/1024} can be passed as {1, 506, 1023, 1024}

	DagGen * dag = new DagGen();
	Wara wara;
	Remia remia;
	vector < node* > F;	//Forest
	vector < CV* > PCV;	//Vector to hold all the PCV's.

	int num, deno;
	node *T;
	stack< node * > tree_unique_nodes;

	deno = atoi(argv[argc-1]);
	for(int i = 1; i<argc-1; i++)
	{
		num = atoi(argv[i]);
		CV* t_cv = new CV(num,deno,(long double)num/(long double)deno);	//prime concentration value
		PCV.push_back(t_cv);
	}

	for(int i=0; i<PCV.size();i++)
	{
		node *root = new node();
		root = remia.BuildMixingTree(PCV[i]);
		wara.SMT.push_back(root);
	}


	wara.Maximal_Droplet_Sharing();

	wara.Droplet_Replacement(wara.SMT);

	remia.BuilEDTforest(wara.SMT, F);

	cout<<"Mixing Tree and forest built successfully\n";

	wara.uid = remia.UID;	//Continue the UID from where remia left

	stack< Vertex *> vertices;
	for(int i=0; i<F.size();i++)
	{
		T = F[i];
		wara.convertDataStructureForPCV(T, dag, vertices, T);
	}

	for(int i=0; i<wara.SMT.size();i++)
	{
		if(wara.SMT[i]->left == NULL && wara.SMT[i]->right == NULL)
		{
			Vertex *vs;
			for(stack <Vertex *> dummy = vertices; !dummy.empty(); dummy.pop())
			{
				if(wara.SMT[i]->dag_uid.top() == dummy.top()->uniqueID)
				{
					vs = dummy.top();
					break;
				}
			}
			if(wara.SMT[i]->status == both_output)
			{
				Vertex *vo1 = wara.createVertex(wara.o_count, wara.o, dag, OUTPUT);
				//Connect the Split vertex to the OUTPUT1 vertex.
				dag->addEdge(vs, vo1);

				Vertex *vo2 = wara.createVertex(wara.o_count, wara.o, dag, OUTPUT);
				//Connect the Split vertex to the OUTPUT2 vertex.
				dag->addEdge(vs, vo2);
			}
			else
			{
				Vertex *vo = wara.createVertex(wara.o_count, wara.o, dag, OUTPUT);
				//Connect the Split vertex to the OUTPUT vertex.
				dag->addEdge(vs, vo);
			}
		}
		else
		{
			T = wara.SMT[i];
			while(T != NULL)
			{
				if(!wara.checkIfUniqueNodeInStack(tree_unique_nodes, T))
					tree_unique_nodes.push(T);
				T = T->right;
			}
			tree_unique_nodes.pop();
		}
	}

	wara.convertDataStructureForMixingTree(tree_unique_nodes, dag, vertices, PCV);

   // 	dag.generateJSON("../output/Example.json");
	//dag.generateDropletDag("../output/DropletDag.cpp");
	//dag.generateDotyGraph("../output/Example.dot");
	return dag;
}

vector < vector < Vertex* > > Wara :: WARA_MAIN(DagGen *dag, vector < int > argv)
{
	Remia remia;
	vector < node* > F;	//Forest
	vector < CV* > PCV;	//Vector to hold all the PCV's.

	int num, deno;
	node *T;
	stack< node * > tree_unique_nodes;

	deno = argv[argv.size()-1];
	lb = argv[0];
	ub = argv[argv.size()-2];
	for(int i = 0; i<argv.size()-1; i++)
	{
		num = argv[i];
		CV* t_cv = new CV(num,deno,(long double)num/(long double)deno);	//prime concentration value
		PCV.push_back(t_cv);
	}

	for(unsigned int i=0; i<PCV.size();i++)
	{
		node *root = new node();
		root = remia.BuildMixingTree(PCV[i]);
		SMT.push_back(root);
	}


	Maximal_Droplet_Sharing();

	Droplet_Replacement(SMT);

	remia.BuilEDTforest(SMT, F);

	uid = remia.UID;	//Continue the UID from where remia left

	stack< Vertex *> vertices;
	for(unsigned int i=0; i<F.size();i++)
	{
		T = F[i];
		convertDataStructureForPCV(T, dag, vertices, T);
	}

	for(unsigned int i=0; i<SMT.size();i++)
	{
		if(SMT[i]->left == NULL && SMT[i]->right == NULL)
		{
			Vertex *vs;
			for(stack <Vertex *> dummy = vertices; !dummy.empty(); dummy.pop())
			{
				if(SMT[i]->dag_uid.top() == dummy.top()->uniqueID)
				{
					vs = dummy.top();
					break;
				}
			}
			if(SMT[i]->status == both_output)
			{
				if(SMT[i]->cv->num == lb)
				{
					lb_vertices.push_back(vs);
					lb_vertices.push_back(vs);
				}
				else if(SMT[i]->cv->num == ub)
				{
					ub_vertices.push_back(vs);
					ub_vertices.push_back(vs);
				}
			}
			else
			{
				if(SMT[i]->cv->num == lb)
					lb_vertices.push_back(vs);
				else if(SMT[i]->cv->num == ub)
					ub_vertices.push_back(vs);
			}
		}
		else
		{
			T = SMT[i];
			while(T != NULL)
			{
				if(!checkIfUniqueNodeInStack(tree_unique_nodes, T))
					tree_unique_nodes.push(T);
				T = T->right;
			}
			tree_unique_nodes.pop();
		}
	}

	convertDataStructureForMixingTree(tree_unique_nodes, dag, vertices, PCV);

	vector < vector < Vertex* > > boundary_vertices;
	boundary_vertices.push_back(lb_vertices);
	boundary_vertices.push_back(ub_vertices);

	return boundary_vertices;
}

