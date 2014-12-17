#include "../Headers/WRemia.h"

W_Remia :: W_Remia()
{
	UID = 0;
}

void W_Remia :: printLevelOrder(W_node *T)
{
	if (!T)
		return;
	queue<W_node*> currentLevel, nextLevel;
	currentLevel.push(T);

	while (!currentLevel.empty())
	{
		W_node *currNode = currentLevel.front();
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


W_CV * W_Remia :: extractMSNZB(W_CV *ccv)
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

	W_CV *lcv = new W_CV(c*ccv->deno,ccv->deno,c);
	return lcv;
}

W_node* W_Remia :: BuildMixingTree(W_CV *Ct)
{
	W_node *root = new W_node();
	root->id = UID++;
	root->cv = Ct;
	root->status = W_reusable;
	root->target = W_yes;
	root->ac = 0;

	W_node *cur = root;
	W_CV * ccv = new W_CV (0,0,0);
	ccv = Ct;

	bool pcv = false;
	while(!pcv)
	{
		cur->left = new W_node();
		cur->right = new W_node();

		W_CV * lcv = new W_CV (0,0,0);
		W_CV * rcv = new W_CV (0,0,0);

		lcv = extractMSNZB(ccv);

		if(lcv->num/2 == ccv->num)	//check if the lcv is pcv. if yes, break out of loop else continue
		{
			cur->left = NULL;
			cur->right = NULL;
			if(cur != root)
				cur->status = W_non_reusable;
			cur->ac = numeric_limits<int>::max();
			pcv = true;
		}
		else
		{
			cur->left->id = UID++;
			cur->left->cv = lcv;
			cur->left->status = W_non_reusable;
			cur->left->target = W_no;
			cur->left->ac = numeric_limits<int>::max();

			rcv = new W_CV(((2 * ccv->num) - lcv->num), ccv->deno, ((long double)((2 * ccv->num) - lcv->num)/ccv->deno));

			cur->right->id = UID++;
			cur->right->cv = rcv;
			cur->right->status = W_reusable;
			cur->right->target = W_no;
			cur->right->ac = 0;

			cur = cur->right;
			ccv = rcv;
		}
	}

	return root;

}

//This function is for Wara use only. Not a part of original Remia Code
bool W_Remia :: checkIfUniqueInStack(priority_queue<W_node*, vector<W_node*>, W_comparator> Q, W_node *n)
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

void W_Remia :: BuilEDTforest(vector < W_node* >& SMT, vector < W_node* >& F)
{
	priority_queue<W_node*, vector<W_node*>, W_comparator> Q;	//min-heap to store all the leaf nodes of T.

	W_node *x;
	W_node *y;
	for(int i=0; i<SMT.size(); i++)
	{
		x = SMT[i];

		while(x != NULL)
		{
			if(x->left != NULL && x->left->status == W_non_reusable)
			{
				if(!checkIfUniqueInStack(Q,x->left))
					Q.push(x->left);
			}
			else if(x->status == W_non_reusable || (x == SMT[i] && x->left == NULL && x->right == NULL))
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
			W_node *z = new W_node();
			z->id = UID++;
			z->cv = new W_CV(x->cv->num*2, x->cv->deno, x->cv->val*2);
			z->status = W_reusable;
			z->target = W_no;
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

