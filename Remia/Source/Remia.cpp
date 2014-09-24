#include "../Headers/Remia.h"

Remia :: Remia()
{
	UID = 0;
	b = "buffer"; r = "reactant"; w = "waste"; m = "mix"; s = "split"; o = "output";
	b_count = 1; r_count = 1; w_count = 1; m_count = 1; s_count = 1; o_count = 1;
}

void Remia :: printLevelOrder(node *T) 
{
	if (!T) 
		return;
	queue<node*> currentLevel, nextLevel;
	currentLevel.push(T);

	while (!currentLevel.empty()) 
	{
		node *currNode = currentLevel.front();
		currentLevel.pop();
		if (currNode) 
		{
      			cout<<currNode->id<<"->["<<currNode->cv->num<<"/"<<currNode->cv->deno<<" = "<<currNode->status<<"]"<<"\t";
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


CV * Remia :: extractMSNZB(CV *ccv)
{
	long double c = 0.0;	//c stores temporary calculation
	vector < int > dec;
	int b = 2, p = 1;	//b-> base, p-> power
	long double tmp_val = ccv->val;	//holds the value of the parents CV

	while(tmp_val != 0)
	{
		tmp_val = tmp_val * 2;
		if(tmp_val >= 1)
		{  
			dec.push_back(1);
			break;
		}
		else if(tmp_val < 1)
			dec.push_back(0);
	}

	for(int i=0;i<dec.size();i++)
	{
		c = c + dec[i]*(1/pow(b,p));
		p++;
	}
	c = c*2;

	CV *lcv = new CV(c*ccv->deno,ccv->deno,c);
	return lcv;
}

node* Remia :: BuildMixingTree(CV *Ct)
{
	node *root = new node();
	root->id = UID++;
	root->cv = Ct;
	root->status = reusable;

	node *cur = root;
	CV * ccv = new CV (0,0,0);
	ccv = Ct;

	bool pcv = false;
	while(!pcv)
	{
		cur->left = new node();
		cur->right = new node();

		CV * lcv = new CV (0,0,0);
		CV * rcv = new CV (0,0,0);

		lcv = extractMSNZB(ccv);

		if(lcv->num/2 == ccv->num)	//check if the lcv is pcv. if yes, break out of loop else continue
		{
			cur->left = NULL;
			cur->right = NULL;
			cur->status = non_reusable;
			pcv = true;
		}
		else
		{	
			cur->left->id = UID++;
			cur->left->cv = lcv;
			cur->left->status = non_reusable;

			rcv = new CV(((2 * ccv->num) - lcv->num), ccv->deno, ((long double)((2 * ccv->num) - lcv->num)/ccv->deno));

			cur->right->id = UID++;
			cur->right->cv = rcv;
			cur->right->status = reusable;

			cur = cur->right;
			ccv = rcv;
		}
	}

	return root;

}

//This function is for Wara use only. Not a part of original Remia Code
bool Remia :: checkIfUniqueInStack(priority_queue<node*, vector<node*>, comparator> Q, node *n)
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

void Remia :: BuilEDTforest(node *T, vector < node* >& F)
{
	priority_queue<node*, vector<node*>, comparator> Q;	//min-heap to store all the leaf nodes of T.

	node *x = T;
	node *y;

	while(x != NULL)
	{
		if(x->left != NULL)
			Q.push(x->left);
		else
		{
			Q.push(x);
			break;
		}

		x = x->right;
	}

	while(!Q.empty())
	{
		x = Q.top();	//Extracting the first element in the min-heap
		Q.pop();

		if(x->cv->val != 1.0)
		{
			node *z = new node();
			z->id = UID++;
			z->cv = new CV(x->cv->num*2, x->cv->deno, x->cv->val*2);
			z->status = reusable;
			z->left = x;
			
			if(!Q.empty())
			{
				y = Q.top();
				if(x->cv->val == y->cv->val)
				{
					Q.pop();
					z->right = y;
				}
			}
			else
				z->right = NULL;
			Q.push(z);
		}
		else
			F.push_back(x);
	}
}

Vertex* Remia :: createVertex(int &count, string str,DagGen *dag, VertexType type)
{
	ostringstream oss;
	Vertex* v;

	oss << count;
	str += oss.str();
	count++;
	v = dag->addVertex(type, str);
	return (v);
}

void Remia :: convertDataStructureForPCV(node *T, DagGen *dag, stack< Vertex *>& vertices)
{
	Vertex* vr;
	Vertex* vm;
	Vertex* vs;
	Vertex* vb;
	Vertex* vw;

	if(T == NULL)
		return;

	if(T->cv->val == 1.0 && T->left == NULL && T->right == NULL)
	{
		vr = createVertex(r_count, r, dag, DISPENSE);
		T->dag_uid.push(vr->uniqueID);
		vertices.push(vr);
	}
	else
	{
		if(T->cv->val == 1.0)
		{
			//Create a reactant dispense vertex
			vr = createVertex(r_count, r, dag, DISPENSE);
			T->dag_uid.push(vr->uniqueID);
			vertices.push(vr);

			//Create a Mix vertex
			vm = createVertex(m_count, m, dag, MIX);
			vertices.push(vm);

			//Connect the reactant vertex to the Mix vertex.
			dag->addEdge(vr, vm);

			//Create a buffer dispense vertex
			vb = createVertex(b_count, b, dag, DISPENSE);
			vertices.push(vb);

			//Connect the buffer vertex to the Mix vertex.
			dag->addEdge(vb, vm);

			//Create a Split vertex
			vs = createVertex(s_count, s, dag, SPLIT);
			vertices.push(vs);

			//Connect the Mix vertex to the Split vertex.
			dag->addEdge(vm, vs);

			if(T->left != NULL)
				T->left->dag_uid.push(vs->uniqueID);
			if(T->right != NULL)
				T->right->dag_uid.push(vs->uniqueID);

			if((T->left == NULL && T->right != NULL) || (T->left != NULL && T->right == NULL))
			{
				//Create a Waste vertex
				vw = createVertex(w_count, w, dag, WASTE);
				vertices.push(vw);
				//Connect the Split vertex to the Waste vertex.
				dag->addEdge(vs, vw);
			}
		}
		else
		{
			if((T->left == NULL && T->right != NULL) || (T->left != NULL && T->right == NULL) || (T->left != NULL && T->right != NULL))
			{
				//Create a buffer dispense vertex
				vb = createVertex(b_count, b, dag, DISPENSE);
				vertices.push(vb);

				//Get the Vertex corresponding to the vertex being handled
				for(stack <Vertex *> dummy = vertices; !dummy.empty(); dummy.pop())
				{
					vr = NULL; //cdcurtis
					if(T->dag_uid.top() == dummy.top()->uniqueID)
					{
						vr = dummy.top();
						break;
					}
				}

				//Create a Mix vertex
				vm = createVertex(m_count, m, dag, MIX);
				vertices.push(vm);

				//Connect the buffer vertex to the Mix vertex.
				dag->addEdge(vb, vm);

				//Connect the reactant vertex to the Mix vertex.
				dag->addEdge(vr, vm);

				//Create a Split vertex
				vs = createVertex(s_count, s, dag, SPLIT);
				vertices.push(vs);

				//Connect the Mix vertex to the Split vertex.
				dag->addEdge(vm, vs);

				if(T->left != NULL)
					T->left->dag_uid.push(vs->uniqueID);
				if(T->right != NULL)
					T->right->dag_uid.push(vs->uniqueID);

				if((T->left == NULL && T->right != NULL) || (T->left != NULL && T->right == NULL))
				{
					//Create a Waste vertex
					vw = createVertex(w_count, w, dag, WASTE);
					vertices.push(vw);
					//Connect the Split vertex to the Waste vertex.
					dag->addEdge(vs, vw);
				}
				
			}
		}
	}
	convertDataStructureForPCV(T->left, dag, vertices);
	convertDataStructureForPCV(T->right, dag, vertices);
}

void Remia :: convertDataStructureForMixingTree(node *T, DagGen *dag, stack< Vertex *>& vertices, CV* pcv)
{
	node *x;
	stack < node* > Q;

	Vertex* vr;
	Vertex* vm;
	Vertex* vs;
	Vertex* vw;
	Vertex* vo;
	
	x = T;
	while(x != NULL)
	{
		if(x->status == reusable)
			Q.push(x);
		x = x->right;
	}

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

		x->dag_uid.push(vs->uniqueID);

		//Connect the Mix vertex to the Split vertex.
		dag->addEdge(vm, vs);

		//Create a Waste vertex
		vw = createVertex(w_count, w, dag, WASTE);
		vertices.push(vw);
		//Connect the Split vertex to the Waste vertex.
		dag->addEdge(vs, vw);

		if(x->cv->val == pcv->val)
		{
			//Create a OUTPUT vertex
			vo = createVertex(o_count, o, dag, OUTPUT);
			vertices.push(vo);
			//Connect the Split vertex to the OUTPUT vertex.
			dag->addEdge(vs, vo);
		}

		//Get the left Vertex of the vertex being handled
		for(stack <Vertex *> dummy = vertices; !dummy.empty(); dummy.pop())
		{
			if(x->left->dag_uid.top() == dummy.top()->uniqueID)
			{
				vr = dummy.top();
				break;
			}
		}

		//Connect the reactant vertex to the Mix vertex.
		dag->addEdge(vr, vm);

		//Get the right Vertex of the vertex being handled
		for(stack <Vertex *> dummy = vertices; !dummy.empty(); dummy.pop())
		{
			if(x->right->dag_uid.top() == dummy.top()->uniqueID)
			{
				vr = dummy.top();
				break;
			}
		}

		//Connect the reactant vertex to the Mix vertex.
		dag->addEdge(vr, vm);
	}

}

DagGen* Remia :: RunRemia(int argc, char* argv[])
{
	DagGen* dag = new DagGen();
	Remia r;
	if(argc == 0 || argv == NULL )
		return NULL;

	node *root = new node();
	vector < node* > F;	//Forest
	int num, deno;
	node *T;

	num = atoi(argv[1]);
	deno = atoi(argv[2]);
	
	CV* t_cv = new CV(num,deno,(float)num/(float)deno);	//prime concentration value

	
	root = r.BuildMixingTree(t_cv);
	/*cout<<"printing mixing tree received from BuildMixingTree\n";
	printLevelOrder(root);
*/
	r.BuilEDTforest(root, F);

//	cout<<"printing forest received from BuilEDTforest\n";
/*	for(int i=0; i<F.size();i++)
	{
		cout<<"\n\nprinting tree number "<<i+1<<endl;
		printLevelOrder(F[i]);
	}
*/
	stack< Vertex *> vertices;
	for(unsigned int i=0; i<F.size();i++)
	{
		T = F[i];
		r.convertDataStructureForPCV(T, dag, vertices);
	}

	if(root->left == NULL && root->right == NULL)
	{
		Vertex *vs;
		for(stack <Vertex *> dummy = vertices; !dummy.empty(); dummy.pop())
		{
			vs = NULL; //cdcurtis
			if(root->dag_uid.top() == dummy.top()->uniqueID)
			{
				vs = dummy.top();
				break;
			}
		}
		Vertex *vo = r.createVertex(r.o_count, r.o, dag, OUTPUT);
		//Connect the Split vertex to the OUTPUT vertex.
		dag->addEdge(vs, vo);
	}
	else
		r.convertDataStructureForMixingTree(root, dag, vertices, t_cv);

//	cout<<"Your Dag files are in ../output folder\n";

/*    	dag.generateJSON("../output/Example.json");
	dag.generateDropletDag("../output/DropletDag.cpp");
	dag.generateDotyGraph("../output/Example.dot");
*/
	return dag;
}

void Remia :: BuilEDTforest(vector < node* >& SMT, vector < node* >& F)
{
	priority_queue<node*, vector<node*>, comparator> Q;	//min-heap to store all the leaf nodes of T.

	node *x;
	node *y;
	for(int i=0; i<SMT.size(); i++)
	{
		x = SMT[i];

		while(x != NULL)
		{
			if(x->left != NULL && x->left->status == non_reusable)
			{
				if(!checkIfUniqueInStack(Q,x->left))
					Q.push(x->left);
			}
			else if(x->status == non_reusable || (x == SMT[i] && x->left == NULL && x->right == NULL))
			{
				if(!checkIfUniqueInStack(Q,x))
					Q.push(x);
			}

			x = x->right;
		}
	}

	while(!Q.empty())
	{
		x = Q.top();	//Extracting the first element in the min-heap
		Q.pop();

		if(x->cv->val != 1.0)
		{
			node *z = new node();
			z->id = UID++;
			z->cv = new CV(x->cv->num*2, x->cv->deno, x->cv->val*2);
			z->status = reusable;
			z->target = no;
			z->ac = 0;
			z->left = x;

			if(!Q.empty())
			{
				y = Q.top();
				if(x->cv->val == y->cv->val)
				{
					Q.pop();
					z->right = y;
				}
			}
			else
				z->right = NULL;
			Q.push(z);
		}
		else
			F.push_back(x);
	}
}
