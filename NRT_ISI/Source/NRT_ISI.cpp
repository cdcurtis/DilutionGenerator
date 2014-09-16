#include "../Headers/NRT_ISI.h"

NRT_ISI :: NRT_ISI()
{
	UID = 0;
	n = 0;
	r_count = 0; m_count = 0; s_count = 0; b_count = 0; w_count = 0; o_count = 0;
}

//this function returns false if the new node to be created has unique CF in the dilution tree else true
bool NRT_ISI :: checkIfUniqueNode(int cf)
{
	minHeap node_list = nodes;
	while(!node_list.empty())
	{
		if(node_list.top() == cf)
			return true;
		node_list.pop();
	}
	return false;
}

//Function to find the parent of a node in the tree
NRTISINode* NRT_ISI :: Find_parent(NRTISINode *T, NRTISINode *p, NRTISINode *m)
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
		NRTISINode *tmp = Find_parent(T->left, T, m);
		if(tmp != NULL) 
			return tmp;
	}

	if(T->right != NULL)
	{
		NRTISINode *tmp = Find_parent(T->right, T, m);
		if(tmp != NULL) 
			return tmp;
	}

	return NULL;
}

//This function removes the unnecessary nodes from the dilution tree
void NRT_ISI :: removeUnnecessaryNodes()
{
	minHeap tmp_T = T;	//temporarily store the CF target set into tmp_T
	NRTISINode *parent;
	NRTISINode *child;
	while(!tmp_T.empty())	//for each CF in the target set, do
	{
		for(stack < NRTISINode * > dummy = DTNodes; !dummy.empty(); dummy.pop())
		{
			if(tmp_T.top() == dummy.top()->cf)	//find the node with target CF currently on top of tmp_T
			{
				//for the found CF, set the corresponding node's target value to yes.
				dummy.top()->target = yes;

				if(dummy.top()->need == no)	//only if the found node is marked not needed
				{
					dummy.top()->need = yes;	//mark it needed
					child = dummy.top();

//find all the parents and grandparents of the found node and mark them as needed as well. If any parent or grandparent is encountered as needed, that means the ancestors from that node above are already marked need by some other target CF.
					while(1)	
					{
						found = false;
						parent = Find_parent(root, NULL, child);
						if(parent->need == yes)
							break;
						else
							parent->need = yes;

						child = parent;
					}
				}
				break;
			}
		}
		tmp_T.pop();
	}

	//after all the needed nodes have been found, get rid of the onces that are not needed.
	//the below queues begin with the root of the dilution tree and continue to every node on every level.
	currentLevel.push(root);

	while (!currentLevel.empty()) //until all the nodes in the current level has been checked
	{
		NRTISINode *currNode = currentLevel.front();
		currentLevel.pop();

		if(currNode->left != NULL)	//if left child of currNode is not NULL ...
		{
			if(currNode->left->need == no)	//and if its not need, NULLify it ...
				currNode->left = NULL;
			else
				nextLevel.push(currNode->left);	//else consider its left child for the next level to be checked
		}

		if(currNode->right != NULL)	//if right child of currNode is not NULL ...
		{
			if(currNode->right->need == no)	//and if its not need, NULLify it ...
				currNode->right = NULL;
			else
				nextLevel.push(currNode->right); //else consider its right child for the next level to be checked
		}

//Updating the waste cost. If the currNode has both children, its cost is -1. Also if it has only one child but its a target CF, its cost should be -1.
		if((currNode->left != NULL && currNode->right != NULL) || (((currNode->left != NULL && currNode->right == NULL) || (currNode->left == NULL && currNode->right != NULL)) && currNode->target == yes))
			currNode->cost = -1;

//Since in the previous if statement we updated the cost of the node with 2 children or node with 1 child but with target CF as -1, the remaining nodes have atleast one waste droplet. At this point, the cost of those remaining droplets is 0 and is updated in the function assignCost().
		if(currNode->cost != -1)
			waste.push(currNode);

		//If a node has no children, that node will be a leaf node. so push that node to leaf stack.
		if(currNode->left == NULL && currNode->right == NULL)
			leaf.push(currNode);

		if (currentLevel.empty()) 
			swap(currentLevel, nextLevel);
	}
}

void NRT_ISI :: generateDilutionTree()
{
	//first create the root node with CF = 2^(n-1) and push it in the min heap 'nodes' to keep trace of unique nodes
	root->id = UID++;
	root->dagID = -1;
	root->cf = pow(2,n-1);
	root->cost = 0;
	root->need = yes;
	root->target = no;
	root->target_path_ends = NULL;
	root->which_child = 0;
	root->which_target_path = NULL;
	nodes.push(root->cf);
	DTNodes.push(root);

	//begin the currentLevel node with the root of the tree.
	currentLevel.push(root);
	while (!currentLevel.empty()) 
	{
		NRTISINode *currNode = currentLevel.front();
		currentLevel.pop();
		
		currNode->left = new NRTISINode();
		currNode->right = new NRTISINode();

		//if the left node of the current node has unique CF in the tree, create that node and link it to the left else make the left node NULL. As per the paper, CF of left node = 1 right shift of the CF of the current node and append 0 at the MSB i.e., divide the CF of the current node by 2.
		if(!checkIfUniqueNode(currNode->cf/2))
		{
			currNode->left->id = UID++;
			currNode->left->dagID = -1;
			currNode->left->cf = currNode->cf/2;
			currNode->left->need = no;
			currNode->left->cost = 0;
			currNode->left->target = no;
			currNode->left->target_path_ends = NULL;
			currNode->left->which_child = 0;
			currNode->left->which_target_path = NULL;
			nodes.push(currNode->left->cf);
			nextLevel.push(currNode->left);
			DTNodes.push(currNode->left);
		}
		else
			currNode->left = NULL;

		//if the right node of the current node has unique CF in the tree, create that node and link it to the right else make the right node NULL. As per the paper, CF of right node = 1 right shift of the CF of the current node & append 1 at the MSB instead of 0. i.e., divide the CF of the current node by 2 and add one reagent.
		if(!checkIfUniqueNode((currNode->cf + pow(2,n))/2))
		{
			currNode->right->id = UID++;
			currNode->right->dagID = -1;
			currNode->right->cf = (currNode->cf + pow(2,n))/2;
			currNode->right->need = no;
			currNode->right->cost = 0;
			currNode->right->target = no;
			currNode->right->target_path_ends = NULL;
			currNode->right->which_child = 0;
			currNode->right->which_target_path = NULL;
			nodes.push(currNode->right->cf);
			nextLevel.push(currNode->right);
			DTNodes.push(currNode->right);
		}
		else
			currNode->right = NULL;

		if (currentLevel.empty()) 
			swap(currentLevel, nextLevel);
	}

	removeUnnecessaryNodes();
}

//This function corresponds to Algorithm 1 from the paper.
void NRT_ISI :: assignCost()
{
	int extra_cost, node_count;	//node_count corresponds to count from Algo 1 in paper.
	NRTISINode *parent;
	NRTISINode *child;
	stack<NRTISINode*> target_path;

	for(stack<NRTISINode*>dummy = leaf; !dummy.empty(); dummy.pop())	//for every leaf node
	{
		extra_cost = 0;	//set the extra cost and node count to 0.
		node_count = 0;
		child = dummy.top();	//child points to each leaf in the loop one by one

		while(1)
		{	
			found = false;
			parent = Find_parent(root, NULL, child);	//get the parent of the current child
			//since we have set the cost of all the nodes with 2 children or nodes with 1 child but with target CF to -1, the below if condition corresponds to the exit criteria given in the algo., i.e., "Visit through parent pointer until an intermediate target node or an intermediate concentration with two children is found"

			if(parent->cost == -1)
				break;
			target_path.push(parent);	//keeping the parents traversed for each leaf node
			node_count++;	//counting the number of nodes traversed
			//make the current parent as child so that we can go one level up to find the ancestors of the leaf node
			child = parent;	
		}


//Store the node where the search ended for ancestors. Also save whether the last accepted ancestor of dummy.top() was the left or right child.
		dummy.top()->target_path_ends = parent;
		if(parent->left == child)
			dummy.top()->which_child = 1;
		else
			dummy.top()->which_child = 2;

		//if the last found parent had 2 children and it was a target CF, try to reach the root from there and count the extra_cost.
		child = parent;	
		if(child->left != NULL && child->right != NULL && child->target == yes)
		{
			while(1)
			{
				found = false;
				parent = Find_parent(root, NULL, child);
				extra_cost++;
				child = parent;
				if(parent == root || parent == NULL)
					break;
			}
		}

		//set the cost of the leaf node to node_count + extra_cost.
		dummy.top()->cost = node_count + extra_cost;
		//Also, set the cost of all the ancestors in the target_path stack to node_count + extra_cost
		while(!target_path.empty())
		{
			target_path.top()->cost = dummy.top()->cost;
			target_path.top()->which_target_path = dummy.top();
			target_path.pop();
		}
	}
}

//This function is trying to achieve the same result which Algorithm 2 in the paper is trying to get. Algorithm 2 first creates a cost matrix whose entries are the cost of the result produced on combining two wastes, if the result is leaf target concentration. Also, the which_target_path of both wastes should not be the resulting leaf node. This matrix is then used in Algo 3 to get the waste pair which produces a leaf target with highest cost but has least cost of the sum of the waste pairs cost.

//To achieve the same result, for every possible waste pair, I check if any leaf target can be obtained. Then I compare the sum of the cost of the waste pair to the sum of the cost of the previously selected pair & the new cost should be less than old one. Also, the cost of the result leaf to that of the previously obtained result leaf & the new cost should be greater or equal to the old one. At the end, the which_target_path of new waste pair should not be pointing to the newly obtained leaf node. If all these three conditions are true, the new waste pair is selected and used in Algo 3. Initially, the previously selected pairs cost sum is set to max limit of int and the cost of result is set to -1.
void NRT_ISI :: selectWastePair()
{
	NRTISINode *x;
	NRTISINode *y;
	int result;
	int xy_cost = numeric_limits<int>::max();
	int result_cost = -1;

	while(!waste.empty())
	{
		x = waste.top();
		waste.pop();
		for(stack < NRTISINode * > dump = waste; !dump.empty(); dump.pop())
		{
			y = dump.top();
			result = (x->cf + y->cf)/2;
			for(stack<NRTISINode*>tmp = leaf; !tmp.empty(); tmp.pop())
			{
				if(result == tmp.top()->cf)
				{
					if((x->cost + y->cost < xy_cost) && tmp.top()->cost >= result_cost && tmp.top()->cost != 0 && x->which_target_path != tmp.top() && y->which_target_path != tmp.top())
					{
						xy_cost = x->cost + y->cost;
						result_cost = tmp.top()->cost;
						waste1 = x;
						waste2 = y;
						result_leaf = tmp.top();
					}
					break;
				}
			}
		}
	}
}

void NRT_ISI :: pruning()
{
	while(1)
	{
		//set the waste pair to NULL initially so that if no waste pair is returned from selectWastePair() function, we can break out of the loop.
		waste1 = NULL;	
		waste2 = NULL;
		selectWastePair();

		if(waste1 == NULL && waste2 == NULL)	//exit condition.
			break;

		//NULLify the left or right child of the node where the search for ancestor ended in assignCost() function. This means, for example, in reference to fig 1 from paper, if the result_leaf is node with CF 57, its target_path_ends will point to node with CF 8 and which_child will be 2 i.e., right child. Since nodes with CF 36 and 50 are no longer needed, so the right child of node with CF 8 will now point to NULL.
		if(result_leaf->which_child == 1)
			result_leaf->target_path_ends->left = NULL;
		else
			result_leaf->target_path_ends->right = NULL;

		//since the waste nodes have used their waste droplet, either they have 0 wastes left or 1 waste left in case that node is a target CF. In both cases, that waste node pair should be removed from waste list.
		waste1->cost = -1;
		waste2->cost = -1;

		//if the target_path_ends node has atleast one child and is a target CF, set its cost to -1 so that it will not be considered as waste next time.
		if(((result_leaf->target_path_ends->left != NULL && result_leaf->target_path_ends->right == NULL) || (result_leaf->target_path_ends->left == NULL && result_leaf->target_path_ends->right != NULL)) && result_leaf->target_path_ends->target == yes)
			result_leaf->target_path_ends->cost = -1;
		else	//else if target_path_ends node does not have any childern, then its a leaf node and must be a target CF. In that case, use assignCost() to assign cost to that leaf. But before that, all other leaves should be popped out and only target_path_ends node should be pushed in the leaf stack. 
		{
			while(!leaf.empty())
				leaf.pop();
			leaf.push(result_leaf->target_path_ends);
			assignCost();
		}

		//store the waste pair. Later used in while generating the DAG.
		vector<NRTISINode*> wp;
		wp.push_back(waste1);
		wp.push_back(waste2);
		wastePair.push_back(wp);

		while(!leaf.empty())
			leaf.pop();

		currentLevel.push(root);

		while (!currentLevel.empty()) //until all the nodes in the current level has been checked
		{
			NRTISINode *currNode = currentLevel.front();
			currentLevel.pop();

			if(currNode->left != NULL)	//if left child of currNode is not NULL ...
				nextLevel.push(currNode->left);	//consider its left child for the next level to be checked

			if(currNode->right != NULL)	//if right child of currNode is not NULL ...
				nextLevel.push(currNode->right); //consider its right child for the next level to be checked

			if(currNode->cost != -1)
				waste.push(currNode);

			if(currNode->left == NULL && currNode->right == NULL)
				leaf.push(currNode);

			if (currentLevel.empty()) 
				swap(currentLevel, nextLevel);
		}
	}
}

//Function to print the tree in level order.
void NRT_ISI :: printLevelOrder(NRTISINode *R) 
{
	if (!R) 
		return;

	currentLevel.push(R);

	while (!currentLevel.empty()) 
	{
		NRTISINode *currNode = currentLevel.front();
		currentLevel.pop();
		if (currNode) 
		{
      			//cout<<"[CF:"<<currNode->cf<<"->ID:"<<currNode->id<<"->Need:"<<currNode->need<<"]\t";
      			//cout<<"[CF:"<<currNode->cf<<"->c:"<<currNode->cost<<"]\t";
      			cout<<currNode->cf<<"\t";
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

Vertex* NRT_ISI :: createVertex(int &count, string str,DagGen &dag, VertexType type)
{
	ostringstream oss;
	Vertex* v;
	count++;
	oss << count;
	str += oss.str();

	v = dag.addVertex(type, str);
	return (v);
}

void NRT_ISI :: getDag(NRTISINode *R, DagGen &dag)
{
	currentLevel.push(R);	//start reading the tree received from the root

	//For the root, create a reagent and a buffer. Perfrom mix and split.
	vr = createVertex(r_count, "Reagent", dag, DISPENSE);
	vb = createVertex(b_count, "Buffer", dag, DISPENSE);
	vm = createVertex(m_count, "Mix", dag, MIX);
	vs = createVertex(s_count, "Split", dag, SPLIT);
	//Store the split vertex id in root's dagID variable.
	R->dagID = vs->uniqueID;
	//Also push the split vertex to vertices stack
	vertices.push(vs);

	//Connect buffer and reagent to mix vertex and mix to split. This split vertex is the root of the tree.
	dag.addEdge(vr, vm);
	dag.addEdge(vb, vm);
	dag.addEdge(vm, vs);

	while(!currentLevel.empty())
	{
		NRTISINode *currNode = currentLevel.front();
		currentLevel.pop();

		//for the node currently in currNode, get its equivalent split vertex in vps already created previously
		for(stack < Vertex *> dumpV = vertices; !dumpV.empty(); dumpV.pop())
		{
			if(currNode->dagID == dumpV.top()->uniqueID)
			{
				vps = dumpV.top();
				break;
			}
		}

		if(currNode->left != NULL)	//if the left node is not null
		{
			//create a vertex for buffer - vb, mix - vm and split - vs.
			vb = createVertex(b_count, "Buffer", dag, DISPENSE);
			vm = createVertex(m_count, "Mix", dag, MIX);
			vs = createVertex(s_count, "Split", dag, SPLIT);
			//connect vb and vps to vm. connect vm to vs.
			dag.addEdge(vb, vm);
			dag.addEdge(vps, vm);
			dag.addEdge(vm, vs);
			//again push the newly created split to the vertices stack.
			vertices.push(vs);
			//Store the split vertex id in currNode->left node's dagID variable.
			currNode->left->dagID = vs->uniqueID;
			nextLevel.push(currNode->left);
		}

		if(currNode->right != NULL)	//if the right node is not null
		{
			//create a vertex for buffer - vb, mix - vm and split - vs.
			vr = createVertex(r_count, "Reagent", dag, DISPENSE);
			vm = createVertex(m_count, "Mix", dag, MIX);
			vs = createVertex(s_count, "Split", dag, SPLIT);
			//connect vb and vps to vm. connect vm to vs.
			dag.addEdge(vr, vm);
			dag.addEdge(vps, vm);
			dag.addEdge(vm, vs);
			//again push the newly created split to the vertices stack.
			vertices.push(vs);
			//Store the split vertex id in currNode->right node's dagID variable.
			currNode->right->dagID = vs->uniqueID;
			nextLevel.push(currNode->right);
		}

	//if currNode is a target CF and has atmost 1 child, then create a output vertex and connect the vps to output vertex
		if(currNode->target == yes && !(currNode->left != NULL && currNode->right != NULL))
		{
			vo = createVertex(o_count, "Output", dag, OUTPUT);
			dag.addEdge(vps, vo);
		}
//case 1 -> if the current node is not a target and its current cost is not -1, it means it has a waste droplet, e.g., node with CF 52 in fig 3 of the paper. This logic is true because if a non-target node is being used to generate another target in pruning stage, that node's cose is marked -1, like for node with CF 56 in fig 3.
//case 2 -> if currNode has no child, & its cost is not marked -1, it has a waste. This logic is true because, in the pruning stage, if a target leaf node is used to generate another target CF, that leaf node's cost is marked -1.
//In both case 1 or 2, create a waste verte and connect it to vps.
		if((currNode->target == no && currNode->cost != -1) || ((currNode->left == NULL && currNode->right == NULL) && currNode->cost != -1))
		{
			vw = createVertex(w_count, "Waste", dag, WASTE);
			dag.addEdge(vps, vw);
		}

		if (currentLevel.empty()) 	//when done with one level, go to next
			swap(currentLevel, nextLevel);
	}

}

//Calls all the functions.
DagGen NRT_ISI :: RunNRT_ISI(int argc, char* argv[])
{
	//CV1, CV2, CV3, CV4, CV5, .... -> All the target CV's to be achieved
	DagGen dag;
	NRT_ISI nrt;

	int cf = 0;

	for(int i = 1; i<argc; i++)
	{
		nrt.T.push(atoi(argv[i]));
		if(atoi(argv[i]) > cf)
			cf = atoi(argv[i]);
	}

	while(cf != 0)
	{
		nrt.n++;
		cf /= 2;
	}

	while(1)	//untill all the target CF's have been obtained
	{	
		nrt.root = new NRTISINode();	//create a new root
		nrt.generateDilutionTree();	//generate the dilution tree
		nrt.assignCost();	//assign cost to the nodes in the dilution tree created using Algo 1.
		nrt.pruning();	//prune the tree using algo 3.

		nrt.forest.push(nrt.root);	//push the root to the forest

		while(!nrt.T.empty())	//reset the set of target CF's read initially.
			nrt.T.pop();

		nrt.currentLevel.push(nrt.root);	//start reading the tree just obtained from the root

		while (!nrt.currentLevel.empty())
		{
			NRTISINode *currNode = nrt.currentLevel.front();
			nrt.currentLevel.pop();
			//if a node with target CF is found but has both left and right node NULL, that target CF needs to be regenerated. So puch that CF back to the set T.
			if (currNode->target == yes && currNode->left != NULL && currNode->right != NULL) 
				nrt.T.push(currNode->cf);
			if(currNode->left != NULL)
				nrt.nextLevel.push(currNode->left);
			if(currNode->right != NULL)
				nrt.nextLevel.push(currNode->right);
			if (nrt.currentLevel.empty()) 	//when done with one level, go to next
				swap(nrt.currentLevel, nrt.nextLevel);
		}

		if(nrt.T.empty())	//if all the target CF's have been obtained, exit the loop.
			break;
		else	//else NULLify all the stacks and heaps before creating another tree.
		{
			while(!nrt.nodes.empty())
				nrt.nodes.pop();

			while(!nrt.DTNodes.empty())
				nrt.DTNodes.pop();

			while(!nrt.leaf.empty())
				nrt.leaf.pop();

			while(!nrt.waste.empty())
				nrt.waste.pop();
		}

	}

	//Printing all the trees in the forest in level order
	for(stack<NRTISINode*>dummy = nrt.forest; !dummy.empty(); dummy.pop())
	{
		nrt.printLevelOrder(dummy.top());
		nrt.getDag(dummy.top(), dag);
	}

	cout<<"All the files corresponding to the DAG are in ../output folder.\n";

	for(int i=0; i<nrt.wastePair.size(); i++)
	{
		nrt.waste1 = nrt.wastePair[i][0];
		nrt.waste2 = nrt.wastePair[i][1];

		for(stack < Vertex *> dumpV = nrt.vertices; !dumpV.empty(); dumpV.pop())
		{
			if(nrt.waste1->dagID == dumpV.top()->uniqueID)
				nrt.vps = dumpV.top();
			else if(nrt.waste2->dagID == dumpV.top()->uniqueID)
				nrt.vr = dumpV.top();
		}

		nrt.vm = nrt.createVertex(nrt.m_count, "Mix", dag, MIX);
		nrt.vs = nrt.createVertex(nrt.s_count, "Split", dag, SPLIT);
		nrt.vo = nrt.createVertex(nrt.o_count, "Output", dag, OUTPUT);
		nrt.vw = nrt.createVertex(nrt.w_count, "Waste", dag, WASTE);

		dag.addEdge(nrt.vr, nrt.vm);
		dag.addEdge(nrt.vps, nrt.vm);
		dag.addEdge(nrt.vm, nrt.vs);
		dag.addEdge(nrt.vs, nrt.vo);
		dag.addEdge(nrt.vs, nrt.vw);
	}

    return dag;
}

