#include "../Headers/NWara.h"

NCKU_Wara :: NCKU_Wara()
{
	b = "buffer"; r = "reactant"; w = "waste"; m = "mix"; s = "split";
	b_count = 1; r_count = 1; w_count = 1; m_count = 1; s_count = 1;
}

bool NCKU_Wara :: checkIfUniqueNode(priority_queue<N_node*, vector<N_node*>, NCKU_comparator> Q, N_node *n)
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

bool NCKU_Wara :: checkIfUniqueNodeByValue(priority_queue<N_node*, vector<N_node*>, NCKU_comparator> Q, N_node *n)
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

bool NCKU_Wara :: checkIfUniqueNodeInStack(stack< N_node * > Q, N_node *n)
{
	while(!Q.empty())
	{
		if(Q.top() == n)
			return true;
		Q.pop();
	}
	return false;
}

void NCKU_Wara :: generate_stack(N_node *T, priority_queue<N_node*, vector<N_node*>, NCKU_comparator>& sk)
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

N_node* NCKU_Wara :: Find_parent(N_node *T, N_node *p, N_node *m)
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
		N_node *tmp = Find_parent(T->left, T, m);
		if(tmp != NULL)
			return tmp;
	}

	if(T->right != NULL)
	{
		N_node *tmp = Find_parent(T->right, T, m);
		if(tmp != NULL)
			return tmp;
	}

	return NULL;
}

void NCKU_Wara :: Maximal_Droplet_Sharing()
{
	stack <N_node *> node_stack;
	priority_queue<N_node*, vector<N_node*>, NCKU_comparator> tmp_node_stack;
	N_node *T;

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

	N_node *n;	//node to hold the top node from node_stack
	N_node *m;	// node to hold the nodes from dummy stack one at a time
	N_node *parent_n;	//node to hold the parent of the node of n in picture
	N_node *parent_m;	//node to hold the parent of the node of m in picture

	while(!node_stack.empty())
	{
	begin:
		if(node_stack.empty())
			break;
		n = node_stack.top();

		node_stack.pop();
		for (stack<N_node *> dummy = node_stack; !dummy.empty(); dummy.pop())
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
					vector < N_node* > tmpSMT;
					n->status = both_output;
					for(int i = 0; i<SMT.size(); i++)
						if(SMT[i] != m)
							tmpSMT.push_back(SMT[i]);
					SMT = tmpSMT;
				}

				priority_queue<N_node*, vector<N_node*>, NCKU_comparator> tmp_node_stack;
				stack <N_node *> temp_stack;
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

bool NCKU_Wara :: NotInSubtree(N_node *xy, N_node *z)
{
	stack < N_node * > tmp1;
	stack < N_node * > tmp2;
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

void NCKU_Wara :: GetRCPs(vector < N_node* > G, Ncku_RCP &rcp)
{
	priority_queue<N_node*, vector<N_node*>, NCKU_comparator> Q;	//Q for Reusable nodes
	priority_queue<N_node*, vector<N_node*>, NCKU_comparator> RQ;	//RQ holds a copy of Q
	priority_queue<N_node*, vector<N_node*>, NCKU_comparator> NRQ;	//NRQ for Non-Reusable nodes
	priority_queue<N_node*, vector<N_node*>, NCKU_comparator> tmpQ1;
	priority_queue<N_node*, vector<N_node*>, NCKU_comparator> tmpQ2;

	N_node *x;
	N_node *y;
	N_node *z;
	N_node *parent_x;
	N_node *parent_y;

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

int NCKU_Wara :: ERU(vector < N_node* > G)
{
	long double total_pcv = 0;
	priority_queue<N_node*, vector<N_node*>, NCKU_comparator> Q;
	N_node *T;
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

void NCKU_Wara :: SetACs(Ncku_RCP &rcp)
{
	for(Ncku_RCP::iterator it = rcp.begin(); it != rcp.end(); ++it)
	{
		N_node* x = it->first;
		N_node* y = it->second;
		if(x->status == reusable)
			x->ac++;
		if(y->status == reusable)
			y->ac++;
	}
}

void NCKU_Wara :: ResetACs(Ncku_RCP &rcp)
{
	for(Ncku_RCP::iterator it = rcp.begin(); it != rcp.end(); ++it)
	{
		N_node* x = it->first;
		N_node* y = it->second;
		if(x->status == reusable)
			x->ac = 0;
		if(y->status == reusable)
			y->ac = 0;
	}
}

void NCKU_Wara :: GetUniqueness(Ncku_RCP &rcp, vector < int >& uniq)
{
	for(Ncku_RCP::iterator it = rcp.begin(); it != rcp.end(); ++it)
	{
		if(it->first->ac <= it->second->ac)
			uniq.push_back(it->first->ac);
		else
			uniq.push_back(it->second->ac);
	}
}

int NCKU_Wara :: GetGain(vector < N_node* > G, N_node *x, N_node *y, int eru)
{
	priority_queue<N_node*, vector<N_node*>, NCKU_comparator> Q;	//Q for Reusable nodes
	N_node *tmp;
	N_node *z;
	N_node *tx, *ty;
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
				N_node *tmp_y = new N_node();
				tmp_y->id = uid++;
				tmp_y->cv = new NCKU_CV(y->cv->num,y->cv->deno,y->cv->val);
				tmp_y->status = y->status;
				tmp_y->ac = y->ac;
				tmp_y->left = new N_node();
				tmp_y->right = new N_node();
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

void NCKU_Wara :: Droplet_Replacement(vector < N_node* >& SMT)
{

	int eru, tmp_gain, i;
	int max_gain;
	int min_uniq;
	long double z_val;

	while(1)
	{
		Ncku_RCP rcp;
		vector < int > uniq;
		vector < int > gain;
		priority_queue<N_node*, vector<N_node*>, NCKU_comparator> Q;	//Q for Reusable nodes
		N_node *x, *y, *z, *tmp;

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

		for(Ncku_RCP::iterator it = rcp.begin(); it != rcp.end(); ++it)
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

		for(Ncku_RCP::iterator it = rcp.begin(); it != rcp.end(); ++it)
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
						N_node *tmp_y = new N_node();
						tmp_y->id = uid++;
						tmp_y->cv = new NCKU_CV(y->cv->num,y->cv->deno,y->cv->val);
						tmp_y->status = y->status;
						tmp_y->ac = y->ac;
						tmp_y->left = new N_node();
						tmp_y->right = new N_node();
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

Vertex* NCKU_Wara :: createVertex(int &count, string str, DagGen *dag, VertexType type)
{
	ostringstream oss;
	Vertex* v;

	if(!type == DISPENSE) {
	oss << count;
	str += oss.str();
	count++;
	}
	v = dag->addVertex(type, str);
	return (v);
}

void NCKU_Wara :: convertDataStructureForPCV(N_node *T, DagGen *dag, stack< Vertex *>& vertices, N_node *root)
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
		N_node *parent = Find_parent(root, NULL, T);
		for(stack <Vertex *> dummy = vertices; !dummy.empty(); dummy.pop())
		{
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

Vertex* NCKU_Wara :: getCorrespondingVertex(N_node *T, stack< Vertex *>& vertices, VertexType type)
{
	for(stack < int > dump = T->dag_uid; !dump.empty(); dump.pop())
	{
		for(stack <Vertex *> dummy = vertices; !dummy.empty(); dummy.pop())
		{
			if(dump.top() == dummy.top()->uniqueID && type == dummy.top()->type)
				return dummy.top();
		}
	}
}

void NCKU_Wara :: convertDataStructureForMixingTree(stack < N_node* > Q, DagGen *dag, stack< Vertex *>& vertices, vector < NCKU_CV* > PCV)
{
	N_node *x;

	Vertex* vr;
	Vertex* vm;
	Vertex* vs;
	Vertex* vb;
	Vertex* vw;
	Vertex* vo;
	Vertex* v1;
	Vertex* v2;

	stack < N_node* > tmpQ = Q;

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
				if(x->cv->num == lb)
				{
					lb_vertices.push_back(vs);
					lb_vertices.push_back(vs);
				}
				else if(x->cv->num == ub)
				{
					ub_vertices.push_back(vs);
					ub_vertices.push_back(vs);
				}
			}
			else
			{
				if(x->cv->num == lb)
					lb_vertices.push_back(vs);
				else if(x->cv->num == ub)
					ub_vertices.push_back(vs);
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

vector < vector < Vertex* > > NCKU_Wara :: WARA_MAIN(DagGen *dag, vector < int > argv)
{
	NCKU_Remia remia;
	vector < N_node* > F;	//Forest
	vector < NCKU_CV* > PCV;	//Vector to hold all the PCV's.

	int num, deno;
	N_node *T;
	stack< N_node * > tree_unique_nodes;

	deno = argv[argv.size()-1];
	lb = argv[0];
	ub = argv[argv.size()-2];
	for(int i = 0; i<argv.size()-1; i++)
	{
		num = argv[i];
		NCKU_CV* t_cv = new NCKU_CV(num,deno,(long double)num/(long double)deno);	//prime concentration value
		PCV.push_back(t_cv);
	}

	for(int i=0; i<PCV.size();i++)
	{
		N_node *root = new N_node();
		root = remia.BuildMixingTree(PCV[i]);
		SMT.push_back(root);
	}


	Maximal_Droplet_Sharing();

	Droplet_Replacement(SMT);

	remia.BuilEDTforest(SMT, F);

	uid = remia.UID;	//Continue the UID from where remia left

	stack< Vertex *> vertices;
	for(int i=0; i<F.size();i++)
	{
		T = F[i];
		convertDataStructureForPCV(T, dag, vertices, T);
	}

	for(int i=0; i<SMT.size();i++)
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

