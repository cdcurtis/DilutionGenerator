#include "../Headers/NRemia.h"

NCKU_Remia :: NCKU_Remia()
{
	UID = 0;
}

void NCKU_Remia :: printLevelOrder(N_node *T)
{
	if (!T)
		return;
	queue<N_node*> currentLevel, nextLevel;
	currentLevel.push(T);

	while (!currentLevel.empty())
	{
		N_node *currNode = currentLevel.front();
		currentLevel.pop();
		if (currNode)
		{
      			cout<<currNode->id<<"->["<<currNode->cv->num<<"/"<<currNode->cv->deno<<" = "<<currNode->status<<"]\t";
			if(currNode->left != NULL)
				nextLevel.push(currNode->left);
			if(currNode->right != NULL)
			nextLevel.push(currNode->right);
		}

		if (currentLevel.empty())
		{
			cout <<"\n\n";
			swap(currentLevel, nextLevel);
		}
	}
}


NCKU_CV * NCKU_Remia :: extractMSNZB(NCKU_CV *ccv)
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

	NCKU_CV *lcv = new NCKU_CV(c*ccv->deno,ccv->deno,c);
	return lcv;
}

N_node* NCKU_Remia :: BuildMixingTree(NCKU_CV *Ct)
{
	N_node *root = new N_node();
	root->id = UID++;
	root->cv = Ct;
	root->status = reusable;
	root->target = yes;
	root->ac = 0;

	N_node *cur = root;
	NCKU_CV * ccv = new NCKU_CV (0,0,0);
	ccv = Ct;

	bool pcv = false;
	while(!pcv)
	{
		cur->left = new N_node();
		cur->right = new N_node();

		NCKU_CV * lcv = new NCKU_CV (0,0,0);
		NCKU_CV * rcv = new NCKU_CV (0,0,0);

		lcv = extractMSNZB(ccv);

		if(lcv->num/2 == ccv->num)	//check if the lcv is pcv. if yes, break out of loop else continue
		{
			cur->left = NULL;
			cur->right = NULL;
			if(cur != root)
				cur->status = non_reusable;
			cur->ac = numeric_limits<int>::max();
			pcv = true;
		}
		else
		{
			cur->left->id = UID++;
			cur->left->cv = lcv;
			cur->left->status = non_reusable;
			cur->left->target = no;
			cur->left->ac = numeric_limits<int>::max();

			rcv = new NCKU_CV(((2 * ccv->num) - lcv->num), ccv->deno, ((long double)((2 * ccv->num) - lcv->num)/ccv->deno));

			cur->right->id = UID++;
			cur->right->cv = rcv;
			cur->right->status = reusable;
			cur->right->target = no;
			cur->right->ac = 0;

			cur = cur->right;
			ccv = rcv;
		}
	}

	return root;

}

//This function is for Wara use only. Not a part of original Remia Code
bool NCKU_Remia :: checkIfUniqueInStack(priority_queue<N_node*, vector<N_node*>, NCKU_comparator> Q, N_node *n)
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

void NCKU_Remia :: BuilEDTforest(vector < N_node* >& SMT, vector < N_node* >& F)
{
	priority_queue<N_node*, vector<N_node*>, NCKU_comparator> Q;	//min-heap to store all the leaf nodes of T.

	N_node *x;
	N_node *y;
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
			N_node *z = new N_node();
			z->id = UID++;
			z->cv = new NCKU_CV(x->cv->num*2, x->cv->deno, x->cv->val*2);
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

