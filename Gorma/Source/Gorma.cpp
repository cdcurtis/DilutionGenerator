#include "../Headers/Gorma.h"

Gorma :: Gorma()
{
	UID = 0;
}

//This function is called from Build_CMT to find the preceding pairs of a CV
void Gorma :: FindPrecedingPairs(PrecedingPair *PP, CV* pcv)
{
	int mul, add;
	CV* tmp = new CV(1,1,1);
	add = 0;

	int pl, temp;
	temp = pcv->deno;
	pl = 0;

	while(temp != 1)
	{
		temp = temp/2;
		pl++;
	}

	if(pl == 1)
	{
		CV* X = new CV(1,1,1);
		CV* Y = new CV(0,1,0);
		PP->push_back(make_pair(X,Y));
		return;
	}

	for(int i=0;i<pl-1;i++)
	{
		CV* X = new CV(0,0,0);
		CV* Y = new CV(0,0,0);

		if(i!=0)
		{
			X->num = tmp->deno*add + tmp->num;
			X->deno = tmp->deno*2;
			X->val = (long double)X->num/(long double)X->deno;

			tmp->num = X->num;
			tmp->deno = X->deno;
			tmp->val = X->val;

			if(add == 0)
				add = 1;
			else
				add = 0;
		}
		else
		{
			X->num = 1;
			X->deno = 1;
			X->val = 1;
		}

		mul = pow(2,pl-1)/X->deno;

		Y->num = pcv->num - (mul * X->num);

		if(X->val == 1 && Y->num < 0)
		{
			X->num = 0;
			X->deno = 1;
			X->val = 0;

			Y->num = pcv->num - (mul * X->num);
		}

		if(Y->num == 0)
			Y->deno = 1;
		else
			Y->deno = pow(2,pl-1);
		Y->val = (long double)Y->num/(long double)Y->deno;

		if(X->val >= 0 && Y->val >= 0)
		{
			if(X->val > Y->val)
				PP->push_back(make_pair(X,Y));
			else
				PP->push_back(make_pair(Y,X));
		}
	}
}

//Function to print the preceding pairs of a CV
void Gorma :: PrintPrecedingPairs(GormaNode *n)
{
	cout<<"For CV = ("<<n->cv->num<<"/"<<n->cv->deno<<")\n";
	int i = 1;
	for(PrecedingPair::iterator it = n->PP->begin(); it != n->PP->end(); ++it)
	{
		CV* x = it->first;
		CV* y = it->second;
		cout<<"Pair "<<i<<" = ("<<x->num<<"/"<<x->deno<<","<<y->num<<"/"<<y->deno<<"), X->val = "<<x->val<<", Y->val = "<<y->val<<"\n";
		i++;
	}
}

//Function to sort the preceding pairs of a CV in ascending order of the X value of the pair in the vector
bool SortPrecedingPairs(const pair<CV*, CV*>& i, const pair<CV*, CV*>& j)
{
	if(i.first->val < j.first->val)
		return true;
	else
		return false;
}

//Function to extract the last pair of the vector and pop it out of the vector.
void Gorma :: ExtractPair(CV *l, CV *r, PrecedingPair *PP)
{
	PrecedingPair::iterator it = PP->end()-1;
	l->num = it->first->num;
	l->deno = it->first->deno;
	l->val = it->first->val;

	r->num = it->second->num;
	r->deno = it->second->deno;
	r->val = it->second->val;

	PP->pop_back();
}

//Function to print the tree in level order.
void Gorma :: printLevelOrder(GormaNode *T)
{
	if (!T)
		return;
	queue<GormaNode*> currentLevel, nextLevel;
	currentLevel.push(T);

	while (!currentLevel.empty())
	{
		GormaNode *currNode = currentLevel.front();
		currentLevel.pop();
		if (currNode)
		{
      			//cout<< currNode->cv->num<<"/"<<currNode->cv->deno<<"="<<currNode->cv->val<<"-"<<currNode->status<<"\t";
      			cout<<currNode->id<<"->["<<currNode->cv->num<<"/"<<currNode->cv->deno<<", "<<currNode->status<<"]"<<"\t";
			nextLevel.push(currNode->left);
			nextLevel.push(currNode->right);
		}

		if (currentLevel.empty())
		{
			cout <<"\n\n";
			swap(currentLevel, nextLevel);
		}
	}
}

//Function to check if the node n is present in the stack or not
bool Gorma :: checkIfUniqueInStack(stack <GormaNode *>& An, GormaNode *n)
{
	for(stack <GormaNode *> dummy = An; !dummy.empty(); dummy.pop())
	{
		if(dummy.top() == n)
		{
			return true;
		}
	}
	return false;
}

//Used to retrieve the unique nodes from the stack containing all the nodes.
void Gorma :: getUniqueNodes(GormaNode *T, stack <GormaNode *>& Un)
{
	if(!T)
		return;

	queue<GormaNode*> currentLevel, nextLevel;
	currentLevel.push(T);
	Un.push(T);

	while (!currentLevel.empty())
	{
		GormaNode *currNode = currentLevel.front();
		currentLevel.pop();
		if (currNode)
		{
			if(currNode->left != NULL)
			{
				nextLevel.push(currNode->left);
				if(!checkIfUniqueInStack(Un, currNode->left))
					Un.push(currNode->left);
			}
			if(currNode->right != NULL)
			{
				nextLevel.push(currNode->right);
				if(!checkIfUniqueInStack(Un, currNode->right))
					Un.push(currNode->right);
			}
		}

		if (currentLevel.empty())
		{
			swap(currentLevel, nextLevel);
		}
	}
}

//Function to build Complete mixing tree for a given CV
GormaNode* Gorma :: Build_CMT(CV *z)
{
	GormaNode *v = new GormaNode();

	v->id = UID++;

	v->cv = new CV(z->num,z->deno,z->val);
	v->PP = new PrecedingPair();	//Preceding pair

	v->left = new GormaNode();
	v->right = new GormaNode();

	if(v->cv->val != 1 && v->cv->val != 0 && v->PP->size() == 0)
	{
		FindPrecedingPairs(v->PP, v->cv);	//finding all the preceding pairs of the current CV
		//sorting all the preceding pairs based on value of the first element of the pair
		sort(v->PP->begin(), v->PP->end(), SortPrecedingPairs);
		v->status = reusable;
		//PrintPrecedingPairs(v);
	}
	else if(v->cv->val == 1 || v->cv->val == 0)
	{
		v->status = non_reusable;
	}

	//Logic to create the left and right child goes here
	if(v->PP->size() != 0)
	{
		CV *l = new CV(0,0,0);
		CV *r = new CV(0,0,0);
		ExtractPair(l,r,v->PP);

		v->left = Build_CMT(l);
		v->right = Build_CMT(r);
	}
	else
	{
		v->left = NULL;
		v->right = NULL;
	}

	return v;
}

//Function to evaluate the tree in terms of reactant and buffer usage
void Gorma :: Tree_Evaluate(GormaNode *p, GormaTreeValue *m, stack <GormaNode *>& sk)
{
	found = false;
	if(p != NULL)
	{
		for (stack<GormaNode *> dummy = sk; !dummy.empty(); dummy.pop())
		{
			if(dummy.top() == p)
			{
				found = true;
				break;
			}
		}

		if(p->cv->val == 1 && !found)
		{
				m->R++;
				sk.push(p);
		}
		else if(p->cv->val == 0 && !found)
		{
				m->B++;
				sk.push(p);
		}

	    if(p->left) Tree_Evaluate(p->left, m, sk);
	    if(p->right) Tree_Evaluate(p->right, m, sk);
	}
}

//Function to copy a tree to back up tree
GormaNode* Gorma :: Tree_Duplicate(GormaNode *s)
{
	if(s != NULL)
	{
		GormaNode *t = new GormaNode();
		//initialising the data structure of the tree's copy

		t->id = s->id;
		t->cv = new CV(s->cv->num,s->cv->deno,s->cv->val);

		//copying the preceding pair of the current node
		t->PP = new PrecedingPair();	//Preceding pair
		for(PrecedingPair::iterator it = s->PP->begin(); it != s->PP->end(); ++it)
		{
			CV *l = new CV(it->first->num, it->first->deno, it->first->val);
			CV *r = new CV(it->second->num, it->second->deno, it->second->val);
			t->PP->push_back(make_pair(l,r));
		}

		t->status = s->status;

	    if(s->left)
	    	t->left = Tree_Duplicate(s->left);
	    else
	    	t->left = NULL;

	    if(s->right)
	    	t->right = Tree_Duplicate(s->right);
	    else
	    	t->right = NULL;

		return t;
	}
	return s; //s passed as null.
}

//Function to put all the reusable nodes of the tree in the stack
void Gorma :: generate_stack(GormaNode *T, stack <GormaNode *>& sk)
{
	if(T != NULL)
	{
		if(T->cv->val != 1 && T->cv->val != 0 && T->status == reusable)
		{
			sk.push(T);
			if(T->left) generate_stack(T->left, sk);
			if(T->right) generate_stack(T->right, sk);
		}
	}
}

//Function to find the parent of a node in the tree
GormaNode* Gorma :: Find_parent(GormaNode *T, GormaNode *p, GormaNode *m)
{
	if(found)
		return p;

	if(T == m)
	{
		found = true;
		return p;
	}

	if(T->left == NULL)
		return NULL;

	GormaNode *tmp = Find_parent(T->left, T, m);
	if(tmp != NULL)
		return tmp;

	if(T->right == NULL)
		return NULL;

	tmp = Find_parent(T->right, T, m);
	if(tmp != NULL)
		return tmp;

	return NULL;
}

void Gorma :: delete_subtree(GormaNode *y)
{
	if(y != NULL)
	{
		if(y->left)
			delete_subtree(y->left);
		if(y->right)
			delete_subtree(y->right);
		free(y);
	}
}

void Gorma :: Maximal_Droplet_Sharing(GormaNode *T)
{
	stack <GormaNode *> node_stack;

	generate_stack(T, node_stack);

	GormaNode *n = new GormaNode();	//node to hold the top node from node_stack
	GormaNode *m = new GormaNode();	// node to hold the nodes from dummy stack one at a time
	GormaNode *parent = new GormaNode();	//node to hold the parent of the node in picture

    	while(!node_stack.empty())
	{
	begin:
		if(node_stack.empty())
			break;

		n = node_stack.top();

		node_stack.pop();
		for (stack<GormaNode *> dummy = node_stack; !dummy.empty(); dummy.pop())
		{
			m = dummy.top();

			if(n->cv->val == m->cv->val)
			{
				found = false;
				parent = Find_parent(T, NULL, m);
				if(parent != NULL)
				{
					if(parent->left == m)
						parent->left = n;
					else
						parent->right = n;

					n->status = reused;
					stack <GormaNode *> temp_stack;

					//Refresh the stack
					generate_stack(T, temp_stack);
					node_stack = temp_stack;

					goto begin;
				}
			}
		}
	}
}

void Gorma :: mutable_nodes(GormaNode *T, stack <GormaNode *>& sk)
{
	if(T != NULL)
	{
		if(T->PP->size() != 0)
			sk.push(T);

		if(T->left) mutable_nodes(T->left, sk);
		if(T->right) mutable_nodes(T->right, sk);
	}
}

GormaNode* Gorma :: last_visited_mutable_node(GormaNode *T)
{
	stack <GormaNode *> mutable_node;
	mutable_nodes(T, mutable_node);

	if(!mutable_node.empty())
		return mutable_node.top();
	else
		return NULL;
}

bool Gorma :: getAncestors(GormaNode *T, GormaNode *v, stack <GormaNode *>& An)
{
	/* base cases */
	if (T == NULL)
    	return false;

	if (T->cv->val == v->cv->val)
    	return true;

	/* If target is present in either left or right subtree of this node, then print this node */
  	if (getAncestors(T->left, v, An) || getAncestors(T->right, v, An))
  	{
  		An.push(T);
    		return true;
  	}

  	/* Else return false */
  	return false;
}

bool Gorma :: checkIfLeftChild(GormaNode *T, GormaNode *v)
{
	if(T == NULL)
		return false;
	if(T->cv->val == v->cv->val)
		return true;
	if(checkIfLeftChild(T->left, v) || checkIfLeftChild(T->right, v))
		return true;
	return false;
}

//Function to enumerate all feasible CMT's for a given CV and find the best CMT.
GormaNode* Gorma :: CMT_Enumeration(CV *ct)
{
	GormaNode *root = new GormaNode();
	stack <GormaNode *> eval;

	GormaTreeValue *min_r = new GormaTreeValue(0,0);

	GormaNode *best_t = new GormaNode();

	root = Build_CMT(ct); //building initial CMT

	best_t = Tree_Duplicate(root);

	Maximal_Droplet_Sharing(best_t);

	Tree_Evaluate(best_t, min_r, eval);

    	GormaNode *v = new GormaNode();

	//Declaring the dummy node
	GormaNode *dummy = new GormaNode();
	dummy->id = UID++;
	dummy->cv = new CV(0,1,0);
	dummy->status = non_reusable;
	dummy->left = new GormaNode();
	dummy->right = new GormaNode();
	dummy->left = NULL;
	dummy->right = NULL;

	v = last_visited_mutable_node(root);

	while(v!= NULL)
	{
		GormaTreeValue *cur_r = new GormaTreeValue(0,0);
		GormaTreeValue *cur_tr = new GormaTreeValue(0,0);
		GormaNode *cur_t = new GormaNode();

		CV *l = new CV(0,0,0);
		CV *r = new CV(0,0,0);
		ExtractPair(l,r,v->PP);

		delete_subtree(v->left);
		delete_subtree(v->right);

		v->left = Build_CMT(l);
		v->right = Build_CMT(r);

		stack <GormaNode *> Ancestors;
		stack <GormaNode *> tmpAncestors;

		cur_t = Tree_Duplicate(root);

		getAncestors(cur_t, v, tmpAncestors);

		for (stack<GormaNode *> dump = tmpAncestors; !dump.empty(); dump.pop())
		{
			if(checkIfLeftChild(dump.top()->left, v))
				dump.top()->right = dummy;
		}
		Maximal_Droplet_Sharing(cur_t);
		Tree_Evaluate(cur_t, cur_tr, eval);

		if((cur_tr->R < min_r->R && cur_tr->B < min_r->B) || (cur_tr->R < min_r->R && cur_tr->B == min_r->B) || (cur_tr->R == min_r->R && cur_tr->B < min_r->B))
		{
			getAncestors(root, v, Ancestors);
			for (stack<GormaNode *> dump = Ancestors; !dump.empty(); dump.pop())
			{
				if(checkIfLeftChild(dump.top()->left, v))
				{
					if(dump.top()->right->PP->size() !=0)
					{
						CV *l = new CV(0,0,0);
						CV *r = new CV(0,0,0);
						ExtractPair(l,r,dump.top()->right->PP);

						delete_subtree(dump.top()->right->left);
						delete_subtree(dump.top()->right->right);

						dump.top()->right->left = Build_CMT(l);
						dump.top()->right->right = Build_CMT(r);

						break;
					}
				}
			}
		}

		cur_t = Tree_Duplicate(root);

		Maximal_Droplet_Sharing(cur_t);

		Tree_Evaluate(cur_t, cur_r, eval);

		if((cur_r->R < min_r->R && cur_r->B < min_r->B) || (cur_r->R < min_r->R && cur_r->B == min_r->B) || (cur_r->R == min_r->R && cur_r->B < min_r->B))
		{
			min_r->R = cur_r->R;
			min_r->B = cur_r->B;
			best_t = cur_t;
		}
		v = last_visited_mutable_node(root);
	}

	return best_t;
	//return root;
}

void Gorma :: createVertex(int &count, string str,DagGen &dag, GormaNode* tmp, stack< Vertex *>& vertices, VertexType type)
{
	ostringstream oss;
	Vertex* v;

	oss << count;
	str += oss.str();
	count++;
	v = dag.addVertex(type, str);
	tmp->dag_uid.push(v->uniqueID);
	vertices.push(v);
}

Vertex* Gorma :: getCorrespondingVertex(GormaNode *T, stack< Vertex *>& vertices, VertexType type)
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

void Gorma :: convertDataStructure(GormaNode *T, CV* pcv, DagGen &dag)
{
	stack <GormaNode *> unique_nodes;
	GormaNode *tmp = new GormaNode();

	string b = "buffer", r = "reactant", w = "waste", m = "mix", s = "split", o = "output";
	int b_count = 1, r_count = 1, w_count = 1, m_count = 1, s_count = 1, o_count = 1;

	stack< Vertex *> vertices;
	Vertex* v;
	Vertex* vTmp;

	getUniqueNodes(T, unique_nodes);

	for(stack <GormaNode *> dummy = unique_nodes; !dummy.empty(); dummy.pop())
	{
		tmp = dummy.top();

		if(tmp->cv->val == 1)
			createVertex(r_count, r, dag, tmp, vertices, DISPENSE);
		else if(tmp->cv->val == 0)
			createVertex(b_count, b, dag, tmp, vertices, DISPENSE);
		else if(tmp->status == 2)
		{
			createVertex(m_count, m, dag, tmp, vertices, MIX);
			vTmp = vertices.top();

			createVertex(s_count, s, dag, tmp, vertices, SPLIT);
			v = vertices.top();

			dag.addEdge(vTmp, v);
		}
		else if(tmp->status == 0 && tmp->cv->val != pcv->val)
		{
			createVertex(m_count, m, dag, tmp, vertices, MIX);
			vTmp = vertices.top();

			createVertex(s_count, s, dag, tmp, vertices, SPLIT);
			v = vertices.top();

			dag.addEdge(vTmp, v);

			createVertex(w_count, w, dag, tmp, vertices, WASTE);
			vTmp = vertices.top();

			dag.addEdge(v, vTmp);
		}
		else if(tmp->cv->val == pcv->val)
		{
			createVertex(m_count, m, dag, tmp, vertices, MIX);
			vTmp = vertices.top();

			createVertex(s_count, s, dag, tmp, vertices, SPLIT);
			v = vertices.top();

			dag.addEdge(vTmp, v);

			createVertex(w_count, w, dag, tmp, vertices, WASTE);
			vTmp = vertices.top();

			dag.addEdge(v, vTmp);

			createVertex(o_count, o, dag, tmp, vertices, OUTPUT);
			vTmp = vertices.top();

			dag.addEdge(v, vTmp);
		}
	}

	for(stack <GormaNode *> dummy = unique_nodes; !dummy.empty(); dummy.pop())
	{
		tmp = dummy.top();
		if(tmp->left != NULL && tmp->right != NULL)
		{
			Vertex *parent;
			Vertex *child;

			child = getCorrespondingVertex(tmp, vertices, MIX);

			if(tmp->left->status == non_reusable)
				parent = getCorrespondingVertex(tmp->left, vertices, DISPENSE);
			else
				parent = getCorrespondingVertex(tmp->left, vertices, SPLIT);

			dag.addEdge(parent,child);

			if(tmp->right->status == non_reusable)
				parent = getCorrespondingVertex(tmp->right, vertices, DISPENSE);
			else
				parent = getCorrespondingVertex(tmp->right, vertices, SPLIT);

			dag.addEdge(parent,child);
		}
	}
}

void Gorma :: RunGorma(DagGen &dag, int argc, char* argv[])
{
	GormaNode *root = new GormaNode();
	int num, deno, gcd;

	num = atoi(argv[1]);
	deno = atoi(argv[2]);

	for(int i=1;i<=num&&i<=deno;i++)
	{
		if(num%i==0 && deno%i == 0 )
			gcd=i;
	}

	if(gcd != 1)
	{
		num /= gcd;
		deno /= gcd;
	}

	CV* t_cv = new CV(num,deno,(long double)num/(long double)deno);	//prime concentration value

	root = CMT_Enumeration(t_cv);
	//cout<<"Tree Enumeration Complete\n";

//	printLevelOrder(root);

	convertDataStructure(root, t_cv, dag);

	//cout<<"Your Dag files are in ../output folder\n\n";

   // 	dag.generateJSON("../output/Example.json");
   //dag.generateDropletDag("../output/DropletDag.cpp");
   //dag.generateDotyGraph("../output/Example.dot");

}


