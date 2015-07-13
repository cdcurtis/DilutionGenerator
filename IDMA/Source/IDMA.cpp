/*
 * IDMA_dilution.cc
 *
 *  Created on: Apr 23, 2013
 *      Author: Kenneth O'Neal
 */

#include "../Headers/IDMA.h"
#include "../../DagGen/Headers/DagGen.h"

using namespace std;

vector<int> IDMA::CalcSkew(DagGen * M) //calculates skew of all nodes in graph
{
	bool Debug = false;
	vector<int> skewVals;
	vector<Vertex*> vertices = M->Vertices();

	for(unsigned i = 0; i < vertices.size(); i++)
	{
		Vertex* cur = vertices.at(i);
		if(Debug){cout<<"cur is: "<<cur->label<<flush<<endl;}
		int Vid = cur->uniqueID;
		vector<Edge*> CurOutgoing = M->findOutgoingEdges(Vid);
		skewVals.push_back(CurOutgoing.size());
		if(Debug){cout<<"skewVal is: "<<CurOutgoing.size()<<flush<<endl;}
	}
	return skewVals;
}

void IDMA::outputSkewVals(vector<int> skewVals) //output skew values of nodes in order calculated
{
	for(unsigned i = 0; i < skewVals.size(); i++)
	{
		cout<<skewVals.at(i)<<flush<<endl;
	}
	return;
}

int IDMA::PruneDMRW(DagGen* M, vector<int> skewVals, DiluteRet *valid) //If a node exists with skew(ni) >= 3, then prun G = M up to this node, resultant M is G
{
	bool Debug = false;
	bool Flag = false;
	if(Debug){cout<<"prune0"<<flush<<endl;}
	vector<Vertex*> vertices = M->Vertices();
	if(Debug){cout<<"prune0.1"<<flush<<endl;}

	int Vid;
	for(unsigned i = 0; i < skewVals.size(); i++)
	{
		if(Debug){cout<<"prune1"<<flush<<endl;}
		int skewVal = skewVals.at(i);
		if(Debug){cout<<"Skew value being checked is: "<<vertices.at(i)->label<<vertices.at(i)->type<<flush<<", skew value is: "<<skewVals.at(i)<<endl;}
		if(skewVal >= 3 and vertices.at(i)->type != DISPENSE)
		{
			if(Debug){cout<<"prune2"<<flush<<endl;}
			if(Debug){cout<<"found a skewval at: "<<i<<", "<<vertices.at(i)->label<<flush<<endl;}
			Flag = true;
			Vid = i;
			break;
		}
	}

	if(!Flag)
	{
		if(Debug){cout<<"prune3"<<flush<<endl;}
		int Vid = -1;
		int ret = Vid;
		return ret;
		//should do nothing, M' = original M.
	}
	else
	{
		if(Debug){cout<<"prune4"<<flush<<endl;}
		//should prune M to include Dag up until Node with Skew >2
		if(Debug){cout<<"skew value found is: "<<skewVals.at(Vid)<<flush<<endl;}
		TrimDag(M, Vid, valid);
	}

	int ret = Vid; //Vid is index into DilutionVals for droplet. 4 == 4
	return ret;
}

int IDMA::PruneDiGraph(DiluteRet* valid)
{
	bool Debug = false;
	bool Flag = false;
	int Vid;
	if(Debug){cout<<"prune0"<<flush<<endl;}
	for(unsigned i = 0; i < valid->DilutionVals.second.size(); i++)
	{
		if(Debug){cout<<"prune1"<<flush<<endl;}
		int skewVal = valid->DilutionVals.second.at(i)->count;
		if(Rational::RattoFloat(valid->DilutionVals.second.at(i)->Concentration) != 0 and Rational::RattoFloat(valid->DilutionVals.second.at(i)->Concentration) != 1)
		{
			if(Debug){cout<<"prune 2"<<flush<<endl;}
			if(skewVal >= 3)
			{
				Flag = true;
				Vid = i;
				break;
			}
		}
	}

	if(!Flag)
	{
		if(Debug){cout<<"prune2"<<flush<<endl;}
		int Vid = -1;
		int ret = Vid;
		return ret;
		//should do nothing, M' = original M.
	}
	else
	{
		Vid = Vid+2; //Vid was skew, must delete after skew
		if(Debug){cout<<"prune4"<<flush<<endl;}
		//should prune M to include Dag up until Node with Skew >2
		if(Debug){cout<<"skew value found is: "<<Vid<<flush<<endl;}
		TrimDiGraph(valid, Vid);
	}

	int ret = Vid;
	return ret;
}

void IDMA::TrimDag(DagGen* M, int Vid, DiluteRet *valid)
{
	bool Debug = false;
	vector<Vertex*> vertices = M->Vertices();
	vector<Edge*> edges = M->Edges();
	if(Debug){cout<<"skew found at: "<<Vid<<" TRIM!!!!! "<<flush<<endl;}
	Vertex* V = vertices[Vid];
	if(Debug){cout<<"Vertex id:"<<V->uniqueID<<", label = "<<V->label<<flush<<endl;}
	//everything following skew node should be removed.
	unsigned i = Vid+1;

	//remove all vertices following, along with all edges it points to.
	while(i < vertices.size())
	{
		if(Debug){cout<<"before remove"<<flush<<endl;}
		if(Debug){cout<<"vertices[i] = "<<vertices.at(i)->label<<flush<<endl;}
		M->removeVertex(vertices.at(i)->uniqueID);
		if(Debug){cout<<"after remove"<<flush<<endl;}
		M->removeAssociatedEdges(vertices.at(i)->uniqueID);
		i++;
		vector<Vertex*> vertices = M->Vertices();
	}

	//remove all driluteDroplets after skewnode output droplets

	RoyDilute R2D2;
	if(Debug)
	{
		cout<<"outputting dilutionVals before trim"<<flush<<endl;
		outputAvailableDroplets(valid->DilutionVals.second);
		outputMixSplits(valid->DilutionVals.first);
	}

	//find Mix/Split number
	string skewLabel = vertices.at(Vid)->label;
	string labelNum;
	for(unsigned i = 0; i < skewLabel.size(); i++)
	{
		if(isalpha(skewLabel.at(i)))
		{
			continue;
		}
		else if(isdigit(skewLabel.at(i)))
		{
			labelNum = labelNum+skewLabel.at(i);
		}
	}
	int skewNum = atoi(labelNum.c_str());
	if(Debug){cout<<"skewNum is: "<<skewNum<<flush<<endl;}

	int diluteID = valid->DilutionVals.first.at(skewNum-1)->DropletsPostMix.second->uID;
//	cout<<"fist MixOp deleted in trimDag() is (pre1, pre2): ("<<valid->DilutionVals.first.at(skewNum)->DropletsPreMix.first->uID<<", "<<valid->DilutionVals.first.at(skewNum)->DropletsPreMix.second->uID<<")"<<flush<<endl;
	for(unsigned i = skewNum; i < valid->DilutionVals.first.size(); i++)
	{
		delete (valid->DilutionVals.first.at(i));
	}
	valid->DilutionVals.first.erase(valid->DilutionVals.first.begin()+skewNum, valid->DilutionVals.first.end());

	//remove all DiluteDroplets after MixOp* outgoing
//	cout<<"first droplet deleted is: "<<valid->DilutionVals.second.at(diluteID)->uID<<flush<<endl;
	valid->DilutionVals.second.at(diluteID)->Concentration.display();
	for(unsigned i = diluteID; i < valid->DilutionVals.second.size(); ++i)
	{
		delete (valid->DilutionVals.second.at(i));
	}
	valid->DilutionVals.second.erase(valid->DilutionVals.second.begin()+diluteID, valid->DilutionVals.second.end());

	if(true)
	{
	//	cout<<"outputting dilutionVals after trim"<<flush<<endl;
		outputAvailableDroplets(valid->DilutionVals.second);
		outputMixSplits(valid->DilutionVals.first);
	}

	return;
}

void IDMA::TrimDiGraph(DiluteRet* valid, int Vid)
{
	bool Debug = false;
	int MixOPID = 0;
	if(Debug){cout<<"Vid is: "<<Vid<<flush<<endl;}

	//find MixOp location to delete
	if(Debug){cout<<"MixOps size is: "<<valid->DilutionVals.first.size()<<flush<<endl;}
	for(unsigned i = 0; i < valid->DilutionVals.first.size(); i++)
	{
		if(Debug){cout<<"MixOp is: ("<<valid->DilutionVals.first.at(i)->DropletsPostMix.first->uID<<", ";}
		if(Debug){cout<<valid->DilutionVals.first.at(i)->DropletsPostMix.second->uID<<")"<<flush<<endl;}
		if(valid->DilutionVals.first.at(i)->DropletsPostMix.first->uID == valid->DilutionVals.second.at(Vid)->uID or valid->DilutionVals.first.at(i)->DropletsPostMix.second->uID == valid->DilutionVals.second.at(Vid)->uID)
		{
			MixOPID = i;
		}
	}

	if(Debug){cout<<"the first mixop to be deleted is: ("<<valid->DilutionVals.first.at(MixOPID)->DropletsPreMix.first->uID<<", "<<valid->DilutionVals.first.at(MixOPID)->DropletsPreMix.second->uID<<")"<<flush<<endl;}

	for(unsigned i = MixOPID; i < valid->DilutionVals.first.size(); i++)
	{
		delete valid->DilutionVals.first.at(i);
	}
	valid->DilutionVals.first.erase(valid->DilutionVals.first.begin()+MixOPID, valid->DilutionVals.first.end());


	if(Debug){cout<<"the first droplet to be deleted is: "<<valid->DilutionVals.second.at(Vid)->uID<<flush<<endl;}
	for(unsigned i = Vid; i < valid->DilutionVals.second.size(); i++)
	{
		delete valid->DilutionVals.second.at(i);
	}
	if(Debug){cout<<"erasing DilutionVals0"<<flush<<endl;}
	valid->DilutionVals.second.erase(valid->DilutionVals.second.begin()+Vid, valid->DilutionVals.second.end());

	if(Debug){cout<<"after erasing dilutionvals"<<flush<<endl;}

	return;
}

pair< pair<int, DiluteDroplet*>, pair<int, DiluteDroplet*> > IDMA::findSkewParentConcDag(int skewID, DiluteRet* valid, DagGen* M)
{
	bool Debug = false;
	//assuming skew node will always be on a split at this point.
	vector<Vertex*> vertices = M->Vertices();
	vector<Edge*> edges = M->Edges();
	string skewLabel;
	int Vid;
	for(unsigned i = 0; i <vertices.size(); i++)
	{
		if(vertices.at(i)->uniqueID == skewID)
		{
			//skew node found.
			skewLabel = vertices.at(i)->label;
			Vid = i;
		}
	}

	string labelNum;
	for(unsigned i = 0; i < skewLabel.size(); i++)
	{
		if(isalpha(skewLabel.at(i)))
		{
			continue;
		}
		else if(isdigit(skewLabel.at(i)))
		{
			labelNum = labelNum+skewLabel.at(i);
		}
	}
	int skewNum = atoi(labelNum.c_str());
	if(Debug){cout<<"skewNum in findDiluteDroplet is:" <<skewNum<<flush<<endl;}

	//Right parent is ret.first to match top down instead of bottom up dag building
	//Left parent is ref.second to match top down instead of bottom up dag building
	if(Debug){valid->DilutionVals.first.at(skewNum-1)->DropletsPreMix.second->Concentration.display();}
	if(Debug){valid->DilutionVals.first.at(skewNum-1)->DropletsPreMix.first->Concentration.display();}

	double concLPar = Rational::RattoFloat(valid->DilutionVals.first.at(skewNum-1)->DropletsPreMix.second->Concentration);
	double concRPar = Rational::RattoFloat(valid->DilutionVals.first.at(skewNum-1)->DropletsPreMix.first->Concentration);

	if(Debug){cout<<"concL dropl= "<<concLPar<<flush<<endl;}
	if(Debug){cout<<"concR drop = "<<concRPar<<flush<<endl;}

	if(Debug){cout<<"L parent drop is: "<<valid->DilutionVals.first.at(skewNum-1)->DropletsPreMix.second->uID<<flush<<endl;}
	if(Debug){cout<<"R parent drop is: "<<valid->DilutionVals.first.at(skewNum-1)->DropletsPreMix.first->uID<<flush<<endl;}

	//Find Skew Node parent in Dag
	int MixID;
	if(Debug){cout<<"skewID is: "<<skewID<<flush<<endl;}
	for(unsigned i = 0; i < edges.size(); i++)
	{
		if(edges.at(i)->child == Vid) //if the child is skew, then parent is Mix
		{
			MixID = edges.at(i)->parent;
			if(Debug){cout<<"MixID vert is: "<<MixID<<flush<<endl;}
		}
	}

	//Find skew nodes preMix nodes in Dag
	int leftParID;
	int rightParID;
	vector<int> MixPars;
	for(unsigned i = 0; i < edges.size(); i++)
	{
		if(edges.at(i)->child == MixID)
		{
			MixPars.push_back(edges.at(i)->parent);
			if(Debug){cout<<"edge pushed to mixPars is: "<<edges.at(i)->parent<<","<<edges.at(i)->child<<flush<<endl;}
		}
	}
	if(Debug){cout<<"MixPars size is: "<<MixPars.size()<<flush<<endl;}

	leftParID = MixPars.at(0);
	rightParID = MixPars.at(1);

	pair<int, DiluteDroplet*> LPar = make_pair(rightParID, valid->DilutionVals.first.at(skewNum-1)->DropletsPreMix.second);
	//cout<<"the LParent in findskewparent is: "<<valid->DilutionVals.first.at(skewNum-1)->DropletsPreMix.second->uID<<flush<<endl;
	pair<int, DiluteDroplet*> RPar = make_pair(leftParID, valid->DilutionVals.first.at(skewNum-1)->DropletsPreMix.first);
	//cout<<"the RParent in findskewparent is: "<<valid->DilutionVals.first.at(skewNum-1)->DropletsPreMix.first->uID<<flush<<endl;
	pair< pair<int, DiluteDroplet*>, pair<int, DiluteDroplet*> > ret = make_pair(LPar, RPar);

	return ret;

}

pair< pair<int, DiluteDroplet*>, pair<int, DiluteDroplet*> > IDMA::findSkewParentConcDiGraph(int skewID, DiluteRet* valid)
{
	bool Debug = false;
	if(Debug){cout<<"skew ID is: "<<skewID<<flush<<endl;}
	if(Debug){cout<<"size of dil vals is: "<<valid->DilutionVals.first.size()<<flush<<endl;}

	int DropletID = valid->DilutionVals.second.at(skewID-1)->uID;
	if(Debug){cout<<"DropletID is: "<<DropletID<<flush<<endl;}
	if(Debug)
	{
		cout<<"DropletID conc is: ";
		valid->DilutionVals.second.at(skewID-1)->Concentration.display();
	}
	int MixIndex  = 0;
	for(unsigned i = 0; i < valid->DilutionVals.first.size(); i++)
	{
		if(valid->DilutionVals.first.at(i)->DropletsPostMix.first->uID == DropletID or valid->DilutionVals.first.at(i)->DropletsPostMix.second->uID == DropletID)
		{
			if(Debug){cout<<"MixIndex set to: "<<i<<flush<<endl;}
			MixIndex = i;
		}
	}

	if(Debug){cout<<"MixID is: "<<MixIndex<<flush<<endl;}

	//skewID is index into dilutionVals for skewdroplet.
	double concLPar = Rational::RattoFloat(valid->DilutionVals.first.at(MixIndex)->DropletsPreMix.second->Concentration);
	double concRPar = Rational::RattoFloat(valid->DilutionVals.first.at(MixIndex)->DropletsPreMix.first->Concentration);
	if(Debug){cout<<"concL dropl= "<<concLPar<<flush<<endl;}
	if(Debug){cout<<"concR drop = "<<concRPar<<flush<<endl;}

	if(Debug){cout<<"L parent drop is: "<<valid->DilutionVals.first.at(MixIndex)->DropletsPreMix.second->uID<<flush<<endl;}
	if(Debug){cout<<"R parent drop is: "<<valid->DilutionVals.first.at(MixIndex)->DropletsPreMix.first->uID<<flush<<endl;}

	pair<int, DiluteDroplet*> LPar = make_pair(valid->DilutionVals.first.at(MixIndex)->DropletsPreMix.second->uID, valid->DilutionVals.first.at(MixIndex)->DropletsPreMix.second);
	pair<int, DiluteDroplet*> RPar = make_pair(valid->DilutionVals.first.at(MixIndex)->DropletsPreMix.first->uID, valid->DilutionVals.first.at(MixIndex)->DropletsPreMix.first);
	pair< pair<int, DiluteDroplet*>, pair<int, DiluteDroplet*> > ret = make_pair(LPar, RPar);
	return ret;
}

double IDMA::findSkewConc(int skewID, DiluteRet* valid, DagGen* M)
{
	bool Debug = false;
	//assuming skew node will always be on a split at this point.
	vector<Vertex*> vertices = M->Vertices();
	string skewLabel;
	for(unsigned i = 0; i <vertices.size(); i++)
	{
		if(vertices.at(i)->uniqueID == skewID)
		{
			//skew node found.
			skewLabel = vertices.at(i)->label;
		}
	}

	string labelNum;
	for(unsigned i = 0; i < skewLabel.size(); i++)
	{
		if(isalpha(skewLabel.at(i)))
		{
			continue;
		}
		else if(isdigit(skewLabel.at(i)))
		{
			labelNum = labelNum+skewLabel.at(i);
		}
	}
	int skewNum = atoi(labelNum.c_str());
	if(Debug){cout<<"skewNum in findDiluteDroplet is:" <<skewNum<<flush<<endl;}

	if(Debug){valid->DilutionVals.first.at(skewNum-1)->DropletsPostMix.first->Concentration.display();}

	double skewConc = Rational::RattoFloat(valid->DilutionVals.first.at(skewNum-1)->DropletsPostMix.first->Concentration);


	if(Debug){cout<<"skew Conc = "<<skewConc<<flush<<endl;}

	return skewConc;

}
int IDMA::findSkewDroplet(double skewConc, DiluteRet* valid)
{

	for(unsigned i = 0; i < valid->DilutionVals.second.size(); i++)
	{
		if(Rational::RattoFloat(valid->DilutionVals.second.at(i)->Concentration) == skewConc)
		{
			return i;
		}
	}

}
vector<DiluteDroplet*> IDMA::findAvailDroplets(int skewID, DiluteRet* valid, DagGen* M)
{
	bool Debug = false;
	//assuming skew node will always be on a split at this point.
	vector<Vertex*> vertices = M->Vertices();
	string skewLabel;
	for(unsigned i = 0; i <vertices.size(); i++)
	{
		if(vertices.at(i)->uniqueID == skewID)
		{
			//skew node found.
			skewLabel = vertices.at(i)->label;
		}
	}

	string labelNum;
	for(unsigned i = 0; i < skewLabel.size(); i++)
	{
		if(isalpha(skewLabel.at(i)))
		{
			continue;
		}
		else if(isdigit(skewLabel.at(i)))
		{
			labelNum = labelNum+skewLabel.at(i);
		}
	}
	int skewNum = atoi(labelNum.c_str());
	if(Debug){cout<<"skewNum in findavailDroplet is:" <<skewNum<<flush<<endl;}

	vector<DiluteDroplet*> AvailDroplets;
	for(int i = 0; i < skewNum; i++)
	{
		AvailDroplets.push_back(valid->DilutionVals.first.at(i)->DropletsPreMix.first);
		AvailDroplets.push_back(valid->DilutionVals.first.at(i)->DropletsPreMix.second);
		AvailDroplets.push_back(valid->DilutionVals.first.at(i)->DropletsPostMix.first);
		AvailDroplets.push_back(valid->DilutionVals.first.at(i)->DropletsPostMix.second);

	}

	if(Debug)
	{
		for(unsigned i = 0; i < AvailDroplets.size(); i++)
		{
			cout<<"AvailDroplet ID is: "<<AvailDroplets.at(i)->uID<<", conc is: "<<Rational::RattoFloat(AvailDroplets.at(i)->Concentration)<<flush<<endl;
		}
	}

	return AvailDroplets;
}

pair< pair<DiluteDroplet*, DiluteDroplet*>, int > IDMA::LR_Calc(DiluteRet *valid, int n, int skewID, DagGen* M, DagGen* createDagBackup, pair< pair<int, DiluteDroplet*>, pair<int, DiluteDroplet*> > parentConcentrations)
{
	bool Debug = false;
	bool Debug2 = false;
	int Vid = skewID;
	int LRid;
	double CTarget = valid->endConcentration.at(0); //only one final diltion, always first

	if(Debug){ cout<<"target concentration is: "<<valid->endConcentration.at(0)<<flush<<endl;}

	//int j = 0; //WTH IS J???? bad paper ill defined, using my closest approximation
	if(Debug)
		cout<<"before getting the vertices"<<flush<<endl;
	vector<Vertex*> curParents;
	vector<Vertex*> vertices = M->Vertices();
	vector<Edge*> edges = M->Edges();
	if(Debug)
		cout<<"after getting the vertices"<<flush<<endl;

	double leftParent = Rational::RattoFloat(parentConcentrations.first.second->Concentration);
	int leftParVertID = parentConcentrations.first.first;
	if(Debug){cout<<"leftParent Concentration is: "<<leftParent<<flush<<endl;}
	double rightParent = Rational::RattoFloat(parentConcentrations.second.second->Concentration);
	int rightParVertID = parentConcentrations.second.first;
	if(Debug){cout<<"rightParent Concentration is: "<<rightParent<<flush<<endl;}

	double skewConc = Rational::RattoFloat(valid->DilutionVals.second.at(skewID)->Concentration);
	int id = valid->DilutionVals.second.at(valid->DilutionVals.second.size()-1)->uID;

	if(Debug){cout<<"leftParent = "<<leftParent<<flush<<endl;}
	if(Debug){cout<<"rightParent = "<<rightParent<<flush<<endl;}

	if(Debug){cout<<"!!!!!!!!! values before LR_Calc!!!!!!!!!! skew: "<<skewConc<<", leftP = "<<leftParent<<", rightP = "<<rightParent<<flush<<endl;}

	DiluteDroplet* Left = new DiluteDroplet;
	id++;
	Left->uID = id;
	Left->count = 0;
	Left->Concentration = Rational::FloatToRat(skewConc, n);

	DiluteDroplet* Right = new DiluteDroplet;
	id++;
	Right->uID = id;
	Right->count = 0;
	Right->Concentration = Rational::FloatToRat(skewConc, n);

	DiluteDroplet* Rcopy = new DiluteDroplet;
	id++;
	Rcopy->uID = id;
	Rcopy->count = 0;

	DiluteDroplet* Lcopy = new DiluteDroplet;
	id++;
	Lcopy->uID = id;
	Lcopy->count = 0;

	MixOp* IDMAMix = new MixOp;

	if(Debug){cout<<"skewID is: "<<skewID<<", conc is: "<<skewConc<<flush<<endl;}
	if(Debug){cout<<"Vid is: "<<Vid<<", leftparVertID is: "<<leftParVertID<<", rightParVertID is: "<<rightParVertID<<flush<<endl;}

	if(skewID == 3) //i.e if conc(j?) == 2^(n-1)/2^n
	{
		if(skewConc < CTarget)
		{
			if(Debug){cout<<"LR1, right should be added"<<flush<<endl;}
			Left->Concentration = Rational::ConctoRat(leftParent, n);
			Right->Concentration = Rational::ConctoRat((skewConc+rightParent)/2, n);

			//add appropriate dilute droplet
			valid->DilutionVals.second.push_back(Right); //add right
			Rcopy->Concentration = Right->Concentration;
			valid->DilutionVals.second.push_back(Rcopy);

			//add the mix op
			int skewLoc = findSkewDroplet(skewConc, valid);
			IDMAMix->DropletsPreMix = make_pair(valid->DilutionVals.second.at(skewLoc), parentConcentrations.second.second); //second.second == right parent
			if(Debug)
				cout<<"premixIDMA L = "<<IDMAMix->DropletsPreMix.first<<flush<<endl;
			if(Debug)
				cout<<"premixIDMA R = "<<IDMAMix->DropletsPreMix.second<<flush<<endl;
			IDMAMix->DropletsPostMix = make_pair(Rcopy, Right);
			valid->DilutionVals.first.push_back(IDMAMix);

			//Add N(R) and two directed edges to G; right is skew and rightparent
			string MixLabel = "IDMA_mix";
			Vertex2* rightMix = addVertex(createDagBackup, MIX, MixLabel, Right, Left, 1, 1);
			string SplitLabel = "IDMA_split";
			Vertex2* rightSplit = addVertex(createDagBackup, SPLIT, SplitLabel, Right, Left, 1, 1);
			createDagBackup->addEdge(Vid, rightMix->uniqueID);
			createDagBackup->addEdge(rightParVertID, rightMix->uniqueID);
			createDagBackup->addEdge(rightMix->uniqueID, rightSplit->uniqueID);
			LRid = rightSplit->uniqueID;
		}
		else
		{
			if(Debug){cout<<"LR2, left should be added"<<flush<<endl;}
			Left->Concentration = Rational::ConctoRat((skewConc+leftParent)/2, n);
			Right->Concentration = Rational::ConctoRat(rightParent, n);

			valid->DilutionVals.second.push_back(Left);
			Lcopy->Concentration = Left->Concentration;
			valid->DilutionVals.second.push_back(Lcopy);

			//add the mix op
			int skewLoc = findSkewDroplet(skewConc, valid);
			IDMAMix->DropletsPreMix = make_pair(parentConcentrations.first.second, valid->DilutionVals.second.at(skewLoc)); //first.second == right parent
			if(Debug)
				cout<<"premixIDMA L = "<<IDMAMix->DropletsPreMix.first<<flush<<endl;
			if(Debug)
				cout<<"premixIDMA R = "<<IDMAMix->DropletsPreMix.second<<flush<<endl;

			IDMAMix->DropletsPostMix = make_pair(Left, Lcopy);
			valid->DilutionVals.first.push_back(IDMAMix);

			//Add N(L) and two directed edges to G;
			string MixLabel = "IDMA_Mix1";
			Vertex2* leftMix = addVertex(createDagBackup, MIX, MixLabel, Right, Left, 1, 1);
			string SplitLabel = "IDMA_SPLIT1";
			Vertex2* leftSplit = addVertex(createDagBackup, SPLIT, SplitLabel, Right, Left, 1, 1);
			createDagBackup->addEdge(Vid, leftMix->uniqueID);
			createDagBackup->addEdge(leftParVertID, leftMix->uniqueID);
			createDagBackup->addEdge(leftMix->uniqueID, leftSplit->uniqueID);
			LRid = leftSplit->uniqueID;
		}
	}
	else
	{
		if(skewConc < CTarget)
		{
			if(Debug){cout<<"LR3, left should be added"<<flush<<endl;}
			Left->Concentration = Rational::ConctoRat((skewConc+leftParent)/2, n);
			Right->Concentration = Rational::ConctoRat(rightParent, n);

			//add the appropriate dilution droplet
			valid->DilutionVals.second.push_back(Left);
			Lcopy->Concentration = Left->Concentration;
			valid->DilutionVals.second.push_back(Lcopy);

			//add the mix op
			int skewLoc = findSkewDroplet(skewConc, valid);
			IDMAMix->DropletsPreMix = make_pair(parentConcentrations.first.second, valid->DilutionVals.second.at(skewLoc)); //first.second == left parent
			if(Debug){
				cout<<"premixIDMA L = "<<IDMAMix->DropletsPreMix.first<<flush<<endl;

				cout<<"premixIDMA R = "<<IDMAMix->DropletsPreMix.second<<flush<<endl;
			}
			IDMAMix->DropletsPostMix = make_pair(Left, Lcopy);
			valid->DilutionVals.first.push_back(IDMAMix);

			//Add N(L) and two directed edges to G;
			string MixLabel = "IDMA_Mix1";
			Vertex2* leftMix = addVertex(createDagBackup, MIX, MixLabel, Right, Left, 1, 1);
			string SplitLabel = "IDMA_SPLIT1";
			Vertex2* leftSplit = addVertex(createDagBackup, SPLIT, SplitLabel, Right, Left, 1, 1);
			createDagBackup->addEdge(Vid, leftMix->uniqueID);
			createDagBackup->addEdge(leftParVertID, leftMix->uniqueID);
			createDagBackup->addEdge(leftMix->uniqueID, leftSplit->uniqueID);
			LRid = leftSplit->uniqueID;
		}
		else
		{
			if(Debug){cout<<"LR4, right should be added"<<flush<<endl;}
			Left->Concentration = Rational::ConctoRat(leftParent,n);
			Right->Concentration = Rational::ConctoRat((skewConc+rightParent)/2,n);

			//add the appropriate Dilution droplet
			valid->DilutionVals.second.push_back(Right);
			Rcopy->Concentration = Right->Concentration;
			valid->DilutionVals.second.push_back(Rcopy);

			//add the mix op
			int skewLoc = findSkewDroplet(skewConc, valid);
			IDMAMix->DropletsPreMix = make_pair(valid->DilutionVals.second.at(skewLoc), parentConcentrations.second.second); //second.second == right parent
			IDMAMix->DropletsPostMix = make_pair(Left, Rcopy);
			if(Debug) {
			cout<<"premixIDMA L = "<<IDMAMix->DropletsPreMix.first<<flush<<endl;
			cout<<"premixIDMA R = "<<IDMAMix->DropletsPreMix.second<<flush<<endl;
			}
			valid->DilutionVals.first.push_back(IDMAMix);


			//Add N(R) and two directed edges to G;
			string MixLabel = "IDMA_Mix1";
			Vertex2* rightMix = addVertex(createDagBackup, MIX, MixLabel, Right, Left, 1, 1);
			string SplitLabel = "IDMA_SPLIT1";
			Vertex2* rightSplit = addVertex(createDagBackup, SPLIT, SplitLabel, Right, Left, 1, 1);
			createDagBackup->addEdge(Vid, rightMix->uniqueID);
			createDagBackup->addEdge(rightParVertID, rightMix->uniqueID);
			createDagBackup->addEdge(rightMix->uniqueID, rightSplit->uniqueID);
			LRid = rightSplit->uniqueID;
		}
	}

	if(Debug2){cout<<"Left result's Concentration is: "<<flush<<endl;}
	if(Debug2){Left->Concentration.display();}
	if(Debug2){cout<<"Right result's Concentration is: "<<flush<<endl;}
	if(Debug2){Right->Concentration.display();}

	pair<DiluteDroplet*, DiluteDroplet*> LRpair = make_pair(Left, Right);
	pair< pair<DiluteDroplet*, DiluteDroplet*> , int > LRret = make_pair(LRpair, LRid);
	return LRret;
}

pair< pair<DiluteDroplet*, DiluteDroplet*>, int> IDMA::LR_CalcDiGraph(DiluteRet *valid, int n, int skewID, pair< pair<int, DiluteDroplet*>, pair<int, DiluteDroplet*> > parentConcentrations, DiluteDroplet* Right, DiluteDroplet* Rcopy, DiluteDroplet* Left, DiluteDroplet* Lcopy, MixOp* IDMAMix)
{
	bool Debug = false;
	bool Debug2 = false;
	int Vid = skewID-1;
	int LRid;
	double CTarget = valid->endConcentration.at(0); //only one final diltion, always first
	if(Debug){ cout<<"target concentration is: "<<valid->endConcentration.at(0)<<flush<<endl;}

	double leftParent = Rational::RattoFloat(parentConcentrations.first.second->Concentration);
	int leftParVertID = parentConcentrations.first.first;
	if(Debug){cout<<"leftParent Concentration is: "<<leftParent<<flush<<endl;}

	double rightParent = Rational::RattoFloat(parentConcentrations.second.second->Concentration);
	int rightParVertID = parentConcentrations.second.first;
	if(Debug){cout<<"rightParent Concentration is: "<<rightParent<<flush<<endl;}

	double skewConc = Rational::RattoFloat(valid->DilutionVals.second.at(Vid)->Concentration);
	if(Debug){cout<<"skewConc is: "<<skewConc<<flush<<endl;}

	int id = valid->DilutionVals.second.at(valid->DilutionVals.second.size()-1)->uID;
	if(Debug){cout<<"id is: "<<id<<flush<<endl;}
	if(Debug){cout<<"leftParent = "<<leftParent<<flush<<endl;}
	if(Debug){cout<<"rightParent = "<<rightParent<<flush<<endl;}
	if(Debug){cout<<"!!!!!!!!! values before LR_Calc!!!!!!!!!! skew: "<<skewConc<<", leftP = "<<leftParent<<", rightP = "<<rightParent<<flush<<endl;}

	Left->count = 0;
	Left->Concentration = Rational::FloatToRat(skewConc, n);

	Right->count = 0;
	Right->Concentration = Rational::FloatToRat(skewConc, n);

	Rcopy->count = 0;
	Lcopy->count = 0;

	if(Debug){cout<<"vID is: "<<Vid<<", conc is: "<<skewConc<<flush<<endl;}
	if(Debug){cout<<"Vid is: "<<Vid<<", leftparVertID is: "<<leftParVertID<<", rightParVertID is: "<<rightParVertID<<flush<<endl;}

	if(Vid == 3) //i.e if conc(j?) == 2^(n-1)/2^n
	{
		if(skewConc < CTarget)
		{
			//Left = CF(LP(j)) and R = CF(j)+CF(RP(j))/2
			if(Debug){cout<<"LR1, right should be added"<<flush<<endl;}
			Left->Concentration = Rational::ConctoRat(leftParent, n);
			Left->uID = leftParVertID;
			Right->Concentration = Rational::ConctoRat((skewConc+rightParent)/2, n);

			//add appropriate dilute droplet
			valid->DilutionVals.second.push_back(Right); //add righta
			id++;
			Right->uID = id;
			Rcopy->Concentration = Right->Concentration;
			valid->DilutionVals.second.push_back(Rcopy);
			id++;
			Rcopy->uID = id;

			//add the mix op
			int skewLoc = findSkewDroplet(skewConc, valid);
			IDMAMix->DropletsPreMix = make_pair(valid->DilutionVals.second.at(skewLoc), parentConcentrations.second.second); //second.second == right parent
			IDMAMix->DropletsPostMix = make_pair(Right, Rcopy);

			if(Debug){cout<<"premixIDMA L = "<<IDMAMix->DropletsPreMix.first->uID<<flush<<endl;}
			if(Debug){cout<<"premixIDMA R = "<<IDMAMix->DropletsPreMix.second->uID<<flush<<endl;}
			if(Debug){cout<<"postmixIDMA L = "<<IDMAMix->DropletsPostMix.first->uID<<flush<<endl;}
			if(Debug){cout<<"postmixIDMA R = "<<IDMAMix->DropletsPostMix.second->uID<<flush<<endl;}

			valid->DilutionVals.first.push_back(IDMAMix);
			Right->Mixes = IDMAMix;
			Rcopy->Mixes = IDMAMix;
			Right->Mix_Splits = valid->DilutionVals.first;
			Rcopy->Mix_Splits = valid->DilutionVals.first;

			Left->Mixes = parentConcentrations.first.second->Mixes;
			Left->Mix_Splits = parentConcentrations.first.second->Mix_Splits;
		}
		else
		{
			//Left = CF(j)+CF(LP(j))/2 and R = CF(RP(j));
			if(Debug){cout<<"LR2, left should be added"<<flush<<endl;}
			Left->Concentration = Rational::ConctoRat((skewConc+leftParent)/2, n);
			Right->Concentration = Rational::ConctoRat(rightParent, n);
			Right->uID = rightParVertID;
			valid->DilutionVals.second.push_back(Left);
			Lcopy->Concentration = Left->Concentration;


			valid->DilutionVals.second.push_back(Lcopy);
			id++;
			Left->uID = id;
			id++;
			Lcopy->uID = id;

			//add the mix op
			int skewLoc = findSkewDroplet(skewConc, valid);
			IDMAMix->DropletsPreMix = make_pair(parentConcentrations.first.second, valid->DilutionVals.second.at(skewLoc)); //first.second == right parent

			IDMAMix->DropletsPostMix = make_pair(Left, Lcopy);
			if(Debug){cout<<"premixIDMA L = "<<IDMAMix->DropletsPreMix.first<<flush<<endl;}
			if(Debug){cout<<"premixIDMA R = "<<IDMAMix->DropletsPreMix.second<<flush<<endl;}
			if(Debug){cout<<"postmixIDMA L = "<<IDMAMix->DropletsPostMix.first->uID<<flush<<endl;}
			if(Debug){cout<<"postmixIDMA R = "<<IDMAMix->DropletsPostMix.second->uID<<flush<<endl;}

			valid->DilutionVals.first.push_back(IDMAMix);
			Left->Mixes = IDMAMix;
			Lcopy->Mixes = IDMAMix;
			Left->Mix_Splits = valid->DilutionVals.first;
			Lcopy->Mix_Splits = valid->DilutionVals.first;

			Right->Mixes = parentConcentrations.second.second->Mixes;
			Right->Mix_Splits = parentConcentrations.second.second->Mix_Splits;
		}
	}
	else
	{
		if(skewConc < CTarget)
		{
			//Left = CF(j)+CF(LP(j))/2 and R = CF(RP(j));
			if(Debug){cout<<"LR3, left should be added"<<flush<<endl;}
			Left->Concentration = Rational::ConctoRat((skewConc+leftParent)/2, n);
			Right->Concentration = Rational::ConctoRat(rightParent, n);
			Right->uID = rightParVertID;

			//add the appropriate dilution droplet
			valid->DilutionVals.second.push_back(Left);

			Lcopy->Concentration = Left->Concentration;
			valid->DilutionVals.second.push_back(Lcopy);

			//add the mix op
			int skewLoc = findSkewDroplet(skewConc, valid);
			IDMAMix->DropletsPreMix = make_pair(parentConcentrations.first.second, valid->DilutionVals.second.at(skewLoc)); //first.second == left parent
			id++;
			Left->uID= id;
			id++;
			Lcopy->uID = id;
			IDMAMix->DropletsPostMix = make_pair(Left, Lcopy);

			if(Debug){cout<<"premixIDMA L = "<<IDMAMix->DropletsPreMix.first<<flush<<endl;}
			if(Debug){cout<<"postmixIDMA L = "<<IDMAMix->DropletsPostMix.first->uID<<flush<<endl;}
			if(Debug){cout<<"postmixIDMA R = "<<IDMAMix->DropletsPostMix.second->uID<<flush<<endl;}

			valid->DilutionVals.first.push_back(IDMAMix);
			Left->Mixes = IDMAMix;
			Lcopy->Mixes = IDMAMix;
			Left->Mix_Splits = valid->DilutionVals.first;
			Lcopy->Mix_Splits = valid->DilutionVals.first;

			Right->Mixes = parentConcentrations.second.second->Mixes;
			Right->Mix_Splits = parentConcentrations.second.second->Mix_Splits;
		}
		else
		{
			//Left = CF(LP(j)) and R = CF(j)+CF(RP(j))/2
			if(Debug){cout<<"LR4, right should be added"<<flush<<endl;}
			Left->Concentration = Rational::ConctoRat(leftParent,n);
			Left->uID = leftParVertID;
			Right->Concentration = Rational::ConctoRat((skewConc+rightParent)/2,n);

			//add the appropriate Dilution droplet
			valid->DilutionVals.second.push_back(Right);
			Rcopy->Concentration = Right->Concentration;
			valid->DilutionVals.second.push_back(Rcopy);

			//add the mix op
			int skewLoc = findSkewDroplet(skewConc, valid);
			IDMAMix->DropletsPreMix = make_pair(valid->DilutionVals.second.at(skewLoc), parentConcentrations.second.second); //second.second == right parent
			id++;
			Right->uID = id;
			id++;
			Rcopy->uID = id;
			id++;
			IDMAMix->DropletsPostMix = make_pair(Right, Rcopy);

			if(Debug){cout<<"premixIDMA L = "<<IDMAMix->DropletsPreMix.first<<flush<<endl;}
			if(Debug){cout<<"premixIDMA R = "<<IDMAMix->DropletsPreMix.second<<flush<<endl;}
			if(Debug){cout<<"postmixIDMA L = "<<IDMAMix->DropletsPostMix.first->uID<<flush<<endl;}
			if(Debug){cout<<"postmixIDMA R = "<<IDMAMix->DropletsPostMix.second->uID<<flush<<endl;}

			valid->DilutionVals.first.push_back(IDMAMix);
			Right->Mixes = IDMAMix;
			Rcopy->Mixes = IDMAMix;
			Right->Mix_Splits = valid->DilutionVals.first;
			Rcopy->Mix_Splits = valid->DilutionVals.first;

			Left->Mixes = parentConcentrations.first.second->Mixes;
			Left->Mix_Splits = parentConcentrations.first.second->Mix_Splits;
		}
	}

	if(Debug2){cout<<"Left result's Concentration is: "<<flush<<endl;}
	if(Debug2){Left->Concentration.display();}
	if(Debug2){cout<<"Right result's Concentration is: "<<flush<<endl;}
	if(Debug2){Right->Concentration.display();}

	pair<DiluteDroplet*, DiluteDroplet*> LRpair = make_pair(Left, Right);
	pair< pair<DiluteDroplet*, DiluteDroplet*> , int > LRret = make_pair(LRpair, LRid);
	return LRret;
}


DagGen* IDMA::IntegrateDags(DagGen* M, DagGen* H) //returns union of G and DRMW return H. called M'
{
	VertexCounts* countVals = findVertexCounts(M);

	bool Debug = false;
	vector<Edge*> Hedges = H->Edges();
	vector<Vertex*> Hvertices = H->Vertices();

	vector<Edge*> Medges = M->Edges();
	vector<Vertex*> Mvertices = M->Vertices();

	vector<int> edgesToConnect;
	int increase = Mvertices.size();
	int MverticesSize = Mvertices.at(Mvertices.size()-1)->uniqueID;


	for(unsigned i = 0; i < Hedges.size(); i++)
	{
		unsigned Eid = Hedges.at(i)->parent;
		if(Eid < Hvertices.size() and Eid >= 0)
		{
			continue;
		}
		else
		{
			if(Debug){cout<<"edge with bad parent found at "<<i<<flush<<endl;}
			edgesToConnect.push_back(i);
		}
	}

	//add H's vertices
	for(unsigned i = 0; i < Hvertices.size(); i++)
	{
		string label = findLabel(Hvertices.at(i));
		for(unsigned j = 0; j < label.size(); j++)
		{
			if(isalpha(label.at(j)))
			{
				continue;
			}
			else if(isdigit(label.at(j)))
			{
				label.erase(label.begin()+j, label.begin()+j+1);
			}
		}
		int cval = findCounter(countVals, Hvertices.at(i));
		ostringstream convert1;
		convert1 << cval;
		label = label + convert1.str();
		M->addVertex(Hvertices.at(i)->type, label);
	}

	for(int i = 0; i < Hedges.size(); i++)
	{
		int ilocation = -1;
		int jlocation = -1;

		for(int j = 0; j < edgesToConnect.size(); j++)
		{
			if(i == edgesToConnect.at(j))
			{
				ilocation = i;
				if(Debug){cout<<"ilocation when found is: "<<i<<flush<<endl;}
				jlocation = j;
			}
			else
			{
				continue;
			}
		}
		if(ilocation == -1)
		{
			if(Debug){cout<<"edge added if is: "<<Hedges.at(i)->parent+increase<<", "<<Hedges.at(i)->child+increase<<flush<<endl;}
			M->addEdge(Hedges.at(i)->parent+increase, Hedges.at(i)->child+increase);
		}
		else
		{
			if(Debug){cout<<"edge added else is: "<<Hvertices.at(MverticesSize)->uniqueID<<", "<< (Hedges.at(edgesToConnect.at(jlocation))->child+increase)<<flush<<endl;}
			M->addEdge(Mvertices.at(MverticesSize)->uniqueID, (Hedges.at(edgesToConnect.at(jlocation))->child+increase));
		}
	}

	DagGen* Mprime = new DagGen(*M);
	return Mprime;

}

void IDMA::IntegrateDigraphs(DiluteRet* validM, DiluteRet* validH, DiluteDroplet* LeftOrig, DiluteDroplet* RightOrig) //upon return, M will contain the final Digraph, H will be deleted
{
	int Debug = false;
	bool Debug2 = false;
	if(Debug)
	{
		cout<<"left orig is: "<<LeftOrig->uID<<flush<<endl;
		cout<<"right orig is: "<<RightOrig->uID<<flush<<endl;
	}

	int incrVal = validM->DilutionVals.second.size();

	if(Debug2)
	{
		//modify uID of DilutionDroplets and MixOps to reflect deletions
		cout<<"validM in integrate is: "<<flush<<endl;
		RoyDilute temp1;
		outputAvailableDroplets(validM->DilutionVals.second);
		outputMixSplits(validM->DilutionVals.first);
		cout<<endl;

		cout<<"validH in integrate is: "<<flush<<endl;
		outputAvailableDroplets(validH->DilutionVals.second);
		outputMixSplits(validH->DilutionVals.first);
		cout<<endl;
	}

	DiluteDroplet* del1 = validH->DilutionVals.second.at(0);
	DiluteDroplet* del2 = validH->DilutionVals.second.at(1);

	if(Debug){cout<<"before replace inputs with orig IntegrateDigraphs"<<flush<<endl;}
	//Replace inputs to validH with originals in validM they were copied from
	for(unsigned i = 0; i < validH->DilutionVals.second.size(); i++)
	{
		//cout<<"validH @ i is: "<<validH->DilutionVals.second.at(i)->uID<<flush<<endl;
		if(Rational::RattoFloat(validH->DilutionVals.second.at(i)->Concentration) != 1 and Rational::RattoFloat(validH->DilutionVals.second.at(i)->Concentration) != 0)
		{
			//cout<<"validH@i Pre mix is: "<<validH->DilutionVals.second.at(i)->Mixes->DropletsPreMix.first->uID<<", "<<validH->DilutionVals.second.at(i)->Mixes->DropletsPreMix.second->uID<<flush<<endl;;

			if(validH->DilutionVals.second.at(i)->Mixes->DropletsPreMix.first == del1)
			{
				//replace premix with Lorig
				validH->DilutionVals.second.at(i)->Mixes->DropletsPreMix.first = LeftOrig;
			}
			if(validH->DilutionVals.second.at(i)->Mixes->DropletsPreMix.second == del1)
			{
				validH->DilutionVals.second.at(i)->Mixes->DropletsPreMix.second = LeftOrig;
			}

			if(validH->DilutionVals.second.at(i)->Mixes->DropletsPreMix.second == del2)
			{
				//replace premix with Rorig
				validH->DilutionVals.second.at(i)->Mixes->DropletsPreMix.second = RightOrig;
			}
			if(validH->DilutionVals.second.at(i)->Mixes->DropletsPreMix.first == del2)
			{
				validH->DilutionVals.second.at(i)->Mixes->DropletsPreMix.first = RightOrig;
			}
		}
		else
		{
			continue;
		}
	}

	if(Debug){cout<<"before delete copies of orig IntegrateDigraphs"<<flush<<endl;}

	//delete these new copies now that they have been replaced
	delete(validH->DilutionVals.second.at(0));
	delete(validH->DilutionVals.second.at(1));

	validH->DilutionVals.second.erase(validH->DilutionVals.second.begin(), validH->DilutionVals.second.begin()+2);


	if(Debug){cout<<"before increase counts of validH"<<flush<<endl;}
	//adjust ids of remaining droplets
	int increase = validM->DilutionVals.second.size()-2; //minus two accuonts for above2 deletions

	if(Debug){cout<<"increase = "<<increase<<flush<<endl;}

	for(unsigned i = 0; i < validH->DilutionVals.second.size(); i++)
	{
		validH->DilutionVals.second.at(i)->uID = validH->DilutionVals.second.at(i)->uID + increase;
	}

	if(Debug2)
	{
		RoyDilute temp1;
		cout<<"validH in integrate after increase is: "<<flush<<endl;
		outputAvailableDroplets(validH->DilutionVals.second);
		outputMixSplits(validH->DilutionVals.first);
		cout<<endl;
	}

	//append H DiluteDroplets to M DiluteDroplets
	validM->DilutionVals.second.insert(validM->DilutionVals.second.end(), validH->DilutionVals.second.begin(), validH->DilutionVals.second.end());

	//append H MixOps to M MixOps
	validM->DilutionVals.first.insert(validM->DilutionVals.first.end(), validH->DilutionVals.first.begin(), validH->DilutionVals.first.end());
	if(Debug2)
	{
		cout<<"outputting validM after IntegrateDigraphs"<<flush<<endl;
		RoyDilute R2D2;
		outputAvailableDroplets(validM->DilutionVals.second);
		outputMixSplits(validM->DilutionVals.first);
	}
	return;
}

void IDMA::DispenseCorrection(DiluteRet* validM)
{
	//correcting multiple Dispenses in Digraph
	bool first = true;
	int firstID = 0;
	int dupdisID = 0;
	for(unsigned i = 0; i < validM->DilutionVals.second.size(); i++)
	{
		if(Rational::RattoFloat(validM->DilutionVals.second.at(i)->Concentration) == 0 and first == true)
		{
			first = false;
			firstID = i;
		}
		else if(Rational::RattoFloat(validM->DilutionVals.second.at(i)->Concentration) == 0 and first == false)
		{
			dupdisID = validM->DilutionVals.second.at(i)->uID;
		//	cout<<"found is: "<<dupdisID<<flush<<endl;
			delete(validM->DilutionVals.second.at(i));
			validM->DilutionVals.second.erase(validM->DilutionVals.second.begin()+i, validM->DilutionVals.second.begin()+i+1);

			//replace with first Dispense, and increase count by 1
			for(unsigned j = 0; j < validM->DilutionVals.first.size(); j++)
			{
				if(validM->DilutionVals.first.at(j)->DropletsPreMix.first->uID == dupdisID)
				{
					validM->DilutionVals.first.at(j)->DropletsPreMix.first = validM->DilutionVals.second.at(firstID);
					validM->DilutionVals.second.at(firstID)->count++;
				//	cout<<"replaced with:"<<validM->DilutionVals.second.at(firstID)->uID<<flush<<endl;
				}
				if(validM->DilutionVals.first.at(j)->DropletsPreMix.second->uID == dupdisID)
				{
					validM->DilutionVals.first.at(j)->DropletsPreMix.second = validM->DilutionVals.second.at(firstID);
					validM->DilutionVals.second.at(firstID)->count++;
				//	cout<<"replaced with:"<<validM->DilutionVals.second.at(firstID)->uID<<flush<<endl;
				}
			}

		}

	}

	//adjust remaining uID;
	for(unsigned j = dupdisID-1; j < validM->DilutionVals.second.size(); j++)
	{
		validM->DilutionVals.second.at(j)->uID--;
	}
	return;
}

pair< int, int > IDMA::IDMA_Process(int num_ops, double DesiredConcentrate, double tolerance, DagGen * Mprime)
{

	bool Debug1 = false; //for outputting dropletdags
	bool Debug2 = false; //for outputting edges and vertices
	bool Debug3 = false; //for function points

	IDMA ID;
	DagGen* M;
	DagGen* H;
	//	DagGen* Mprime;
	DagGen* createDagBackup;

	int numW = 0;
	int numMS = 0;
	pair<int, int> ret;

	pair<vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals;
	DiluteRet* validM;
	DiluteRet* validH;

	//cout<<"the Desired concentrate is: "<<DesiredConcentrate<<flush<<endl;

	if(Debug3){cout<<"----------IDMA_Process::Before IDMA_M----------- "<<flush<<endl;}
	validM = RoyDilute::populateIDMA_M(tolerance, DesiredConcentrate, M, num_ops);
	createDagBackup = validM->populateIDMA_ret.first;
	if(Debug3){cout<<"-----------IDMA_Process::after IDMA_M----------"<<flush<<endl;}

	if(false)
	{
	//	cout<<"validM Dilution vals before Pruning"<<flush<<endl;
		Dilute Dil;
		Dil.outputAvailableDroplets(validM->DilutionVals.second);
		Dil.outputMixSplits(validM->DilutionVals.first);
//		cout<<"after validM create dag and display"<<flush<<endl;
	}

	//pruning graph in preparation for IDMA algorithms
	if(Debug3){cout<<"----------IDMA_Process::Before PruneDMRW----------"<<flush<<endl;}
	int skewID = ID.PruneDiGraph(validM);
	if(Debug3){cout<<"----------IDMA_Process::After PruneDMRW---------"<<flush<<endl;}

	if(false)
	{
		cout<<"validM after prune Digraph"<<flush<<endl;
		Dilute Dil;
		Dil.outputAvailableDroplets(validM->DilutionVals.second);
		Dil.outputMixSplits(validM->DilutionVals.first);
	}

	if(false){cout<<"----------IDMA_Process::before generate DropletDag-----------"<<flush<<endl;}
	if(false){M->generateDropletDag("MAfterPruneT.cpp");}
	if(false){cout<<"----------IDMA_Process::after generate DropletDag----------"<<flush<<endl;}

	if(false)
	{
	//	cout<<"validM Dilution vals before Pruning"<<flush<<endl;
		Dilute Dil;
		Dil.outputAvailableDroplets(validM->DilutionVals.second);
		Dil.outputMixSplits(validM->DilutionVals.first);
	//	cout<<"after validM create dag and display"<<flush<<endl;
	}

	if(skewID == -1)
	{
	//	cout<<"NO SKEW FOUND! M' == M  == DMRW Digraph, USE DMRW expanded dags"<<flush<<endl;
		DiluteDroplet* db = new DiluteDroplet;
		DiluteDroplet* di = new DiluteDroplet;
		db->Concentration = Rational(0, pow(2,num_ops));
		di->Concentration = Rational(pow(2,num_ops), pow(2,num_ops));
		if(true){cout<<"IDMA_Process IDMA fails start RoyDilute_Process"<<flush<<endl;}
		RoyDilute::RoyDilute_Process(db, di, num_ops, DesiredConcentrate, tolerance, Mprime);
		if(true){cout<<"IDMA_Process IDMA fails end RoyDilute_Process"<<flush<<endl;}
		exit(1);
	}
	else
	{

		if(Debug1)
		{
			cout<<"IDMA_Process::outputting M before lr_calc, == M after prune"<<flush<<endl;
			M->outputVertices();
			M->outputEdges();
		}

		if(Debug3){cout<<"---------- IDMA_Process::before parentConcentrations----------"<<flush<<endl;}
		pair< pair<int, DiluteDroplet*>, pair<int, DiluteDroplet*> > parentConcentrations = ID.findSkewParentConcDiGraph(skewID, validM);
		if(Debug2){cout<<"Lpare before LR_Calc is: "<<parentConcentrations.first.second->uID<<flush<<endl;}
		if(Debug2){cout<<"Rpare before LR_Calc is: "<<parentConcentrations.second.second->uID<<flush<<endl;}
		if(Debug3){cout<<"---------- IDMA_Process::after parentConcentrations----------"<<flush<<endl;}

		DiluteDroplet* Right = new DiluteDroplet;
		DiluteDroplet* Left = new DiluteDroplet;
		DiluteDroplet* Rcopy = new DiluteDroplet;
		DiluteDroplet* Lcopy = new DiluteDroplet;
		MixOp* IDMAMix = new MixOp;

		if(Debug3){cout<<"----------IDMA_Process::before LR_CALC-----------"<<flush<<endl;}
		pair< pair<DiluteDroplet*, DiluteDroplet*>, int> LRpair = ID.LR_CalcDiGraph(validM, num_ops, skewID, parentConcentrations, Right, Rcopy, Left, Lcopy, IDMAMix);
		if(Debug3){cout<<"----------IDMA_Process::after LR_CALC-----------"<<flush<<endl;}



		if(Debug1)
		{
			DagGen* dag2 = new DagGen();
			cout<<"validM Dilution vals after LR_Calc"<<flush<<endl;
			Dilute Dil;
			Dil.outputMixSplits(validM->DilutionVals.first);
			Dil.outputAvailableDroplets(validM->DilutionVals.second);
			cout<<"size of droplets: "<<validM->DilutionVals.second.size()<<flush<<endl;
			cout<<"size of MixOp: "<<validM->DilutionVals.first.size()<<flush<<endl;
			RoyDilute DagOps2;
			VertexCounts* VC2 = Dil.CreateDagRoy(dag2, validM->DilutionVals);
			cout<<"after LR_Calc create dag and display"<<flush<<endl;
			delete dag2;
		}

		DiluteDroplet* LeftOrig = LRpair.first.first;
		DiluteDroplet* RightOrig = LRpair.first.second;

		//copy droplets to pass to RoyDilute_IDMA to generate ValidH
		int id = 1;
		DiluteDroplet* L = new DiluteDroplet;
		L->uID = id;
		L->Concentration = LRpair.first.first->Concentration;
		L->Mixes = LRpair.first.first->Mixes;
		DiluteDroplet* R = new DiluteDroplet;
		id++;
		R->uID = id;
		R->Concentration = LRpair.first.second->Concentration;
		R->Mixes = LRpair.first.second->Mixes;

		if(Debug3){cout<<"-----------DMA_Process::Before RoyDilute_IDMA-----------"<<flush<<endl;}
		validH = RoyDilute::RoyDilute_IDMA(L, R, tolerance, DesiredConcentrate, num_ops);
		if(Debug3){cout<<"-----------IDMA_Process::after RoyDilute_IDMA----------"<<flush<<endl;}

		if(false)
		{
		//	cout<<"output validH dilution vals after RoyDilute_IDMA H"<<flush<<endl;
			Dilute Dil;
			Dil.outputAvailableDroplets(validH->DilutionVals.second);
			Dil.outputMixSplits(validH->DilutionVals.first);
		}


		if(Debug3){cout<<"-----------IDMA_Process::before integreateDigraphs----------"<<flush<<endl;}
		ID.IntegrateDigraphs(validM, validH, LRpair.first.first, LRpair.first.second); //upon return, M will contain the final Digraph, H will be deleted
		if(Debug3){cout<<"-----------IDMA_Process::after integreateDigraphs-----------"<<flush<<endl;}

		if(Debug1)
		{
			cout<<"output DilutionVals H, after integrate digraphs"<<flush<<endl;
			Dilute Dil;
			Dil.outputAvailableDroplets(validH->DilutionVals.second);
			Dil.outputMixSplits(validH->DilutionVals.first);
		}

		if(Debug3){cout<<"----------IDMA_Process::before dispensecorrection----------"<<flush<<endl;}
		ID.DispenseCorrection(validM); //removed additional dispense if any left over from above (shouldn't be)
		if(Debug3){cout<<"----------IDMA_Process::after dispensecorrection----------"<<flush<<endl;}

		/*if(true)
		{
			cout<<"output DilutionVals M, after integrate digraphs"<<flush<<endl;
			Dilute Dil;
			Dil.outputAvailableDroplets(validM->DilutionVals.second);
			Dil.outputMixSplits(validM->DilutionVals.first);
		}*/


		if(Debug3){cout<<"----------IDMA_Process::before createDagRoy----------"<<flush<<endl;}
		DagGen dag;
		Dilute Dil;
		VertexCounts* VC = Dil.CreateDagRoy(Mprime, validM->DilutionVals);
		Mprime = VC->H;
		if(Debug3){cout<<"----------IDMA_Process::after createDagRoy----------"<<flush<<endl;}


		if(Debug2)
		{
			cout<<"outputting after return from CreateDagRoy()"<<flush<<endl;
			Mprime->outputVertices();
			Mprime->outputEdges();
		}

		/*//resultant dag should be correct, check with isvalidsinglereactant
		if(false){cout<<"-----------IDMA_Process::before generate DropletDag------------"<<flush<<endl;}
		Mprime->generateDropletDag("MprimeCreateT.cpp");
		if(false){cout<<"-----------after generate DropletDag-----------"<<flush<<endl;}

		if(false){cout<<"-----------IDMA_Process::before generate DotyGraph------------"<<flush<<endl;}
		Mprime->generateDotyGraph("MprimeCreateT.dot");
		if(false){cout<<"-----------IDMA_Process::after generate DotyGrpah------------"<<flush<<endl;}
		 */
		//now, call expander after verifying digraph
		if(Debug3){cout<<"---------- IDMA_Process::Before Dag expander ---------- "<<flush<<endl;}
		Dil.expander(Mprime, VC);
		if(Debug3){cout<<"---------- IDMA_Process:: Afer Dag expander ---------- "<<flush<<endl;}
		/*
		//resultant dag should be correct, check with isvalidsinglereactant
		if(true){cout<<"-----------IDMA_Process::before generate DropletDag------------"<<flush<<endl;}
		Mprime->generateDropletDag("MprimeafterExpanderT.cpp");
		if(true){cout<<"-----------after generate DropletDag------------"<<flush<<endl;}

		if(true){cout<<"------------IDMA_Process::before generate DotyGraph------------"<<flush<<endl;}
		Mprime->generateDotyGraph("MprimeafterExpander.dot");
		if(true){cout<<"------------IDMA_Process::after generate DotyGrpah------------"<<flush<<endl;}
		 */
		numW = Mprime->calcNumWaste();
		numMS = Mprime->calcNumMixSplits();

		ret = make_pair(numW, numMS);

		if(Debug3){cout<<"------------IDMA_Process::before validation testing ------------"<<flush<<endl;}
		int base = validH->base;
		vector<double> endConcentration = validH->endConcentration;
		bool value = Mprime->isValidSingleReactantDilution(endConcentration, base);
		if(Debug3){cout<<"-----------IDMA_Process::end validation testing ------------"<<flush<<endl;}

		/*if(value)
		{
			cout<<"Validity test passed"<<flush<<endl;
		}
		else
		{
			cout<<"validity test not passed"<<flush<<endl;
		}*/

	}

	if(Debug3)cout<<"---------- IDMA_PRocess::before mem management IDMA_Process ---------"<<flush<<endl;
	//delete valid
	validM->deleteMixOps();
	validM->deleteDiluteDroplets();
	//validH->deleteMixOps(); //now a part of ValidM after insert in integrate will cause double free
	//cout<<"4"<<flush<<endl;
	//validH->deleteDiluteDroplets();
	//cout<<"5"<<flush<<endl;
	//Mprime->~DagGen();

	//cout<<Mprime->isEmpty();
	//[kenneth] Print this:// Mprime->Vertices()
	return ret;
}

void IDMA:: RunIDMA(vector<string> parameters, DagGen * dag)
{
	if(dag == NULL)
		dag = new DagGen();
	/*numOps, tolerance, DesiredConcentrate*/
	/*if(argc<4){
		cerr<<"In correct Input: <NumOps>, <tolerance> <DesiredConcentration>" << endl;
		std::exit(-1);
	}*/
	int numOps = atoi(parameters[0].c_str());
	double tolerance = atof(parameters[1].c_str());
	double DesiredConcentrate = atof(parameters[2].c_str());

	IDMA::IDMA_Process(numOps,DesiredConcentrate,tolerance,dag);
}

/*void IDMA::IDMA_Wrapper(DiluteDroplet* db, DiluteDroplet* di, int num_ops, double DesiredConcentrate, double tolerance)
{
		//RoyDilute RD;
	//DagGen dag;

//check if IDMA is applicable;
	bool Debug1 = false; //for outputting dropletdags
	bool Debug2 = false; //for outputting edges and vertices
	bool Debug3 = true; //for function points

	IDMA ID;
	DagGen* M;
	DagGen* H;
	DagGen* Mprime;
	DagGen* createDagBackup;

	int numW = 0;
	int numMS = 0;

//note: pair.first == numwaste
//note: pair.second == numMS
	pair<int, int> DMRWvals;
	int DMRWnumW = 0;
	int DMRWnumMS = 0;
	pair<int, int> IDMAvals;
	int IDMAnumW = 0;
	int IDMAnumMS = 0;
	pair<int, int> BSvals;
	int BSnumW = 0;
	int BSnumMS = 0;
	pair<int, int> ret;
	int finalnumW = 0;
	int finalnumMS = 0;

	//GenerateDigraph(Cl, Ch, precision, DesiredConcentrate);
	pair<vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals;
	DiluteRet* valid;

	if(Debug3){cout<<" //////////IDMA_Process::Before IDMA_M\\\\\\\\\\ "<<flush<<endl;}
	valid = RoyDilute::populateIDMA_M(tolerance, DesiredConcentrate, M);
	createDagBackup = valid.populateIDMA_ret.first;
	M = valid.populateIDMA_ret.second;
	if(Debug3){cout<<" //////////IDMA_Process::after IDMA_M\\\\\\\\\\ "<<flush<<endl;}


	//RoyDilute::RoyDilute_Process(tolerance, DesiredConcentrate);
	if(Debug1)
	{
		cout<<"after RoyDilute Copy in IDMA"<<flush<<endl;
		M->outputVertices();
		M->outputEdges();
	}

	if(Debug1){
		cout<<"IDMA_Process::M test after DMRW completion"<<flush<<endl;
		M->outputVertices();
		M->outputEdges();
	}

	if(Debug3){cout<<"IDMA_Process::before getvertices after IDMA_M"<<flush<<endl;}
	vector<Vertex*> temp = M->getVertices();
	int val = temp.size();
	if(Debug3){cout<<"IDMA_Process::after getvertices after IDMA_M"<<flush<<endl;}

	if(Debug3){	cout<<" /////////IDMA_Process::Before skewCalc\\\\\\\\\ "<<flush<<endl;}
	vector<int> skewVals = ID.CalcSkew(M);
	if(Debug3){cout<<" /////////IDMA_Process::After skewCalc\\\\\\\\\ "<<flush<<endl;}

	//outputSkewVals

	//building graph returned from DMRW
	if(Debug2){cout<<" /////////IDMA_Process::before generate DropletDag\\\\\\\\\\ "<<flush<<endl;}
	if(Debug2){M->generateDropletDag("MBeforePruneT.cpp");}
	if(Debug2){cout<<" /////////IDMA_Process::after generate DropletDag\\\\\\\\\\ "<<flush<<endl;}

	if(Debug2){cout<<" /////////IDMA_Process::before generate DotyGraph\\\\\\\\\\ "<<flush<<endl;}
	if(Debug2){M->generateDotyGraph("MBeforePrune.dot");}
	if(Debug2){cout<<" /////////IDMA_Process::after generate DotyGrpah\\\\\\\\\\ "<<flush<<endl;}

	//pruning graph in preparation for IDMA algorithms
	if(Debug3){cout<<" /////////IDMA_Process::Before PruneDMRW\\\\\\\\\ "<<flush<<endl;}
	int skewID = ID.PruneDMRW(M, skewVals, valid);
	if(Debug3){cout<<" /////////IDMA_Process::After PruneDMRW\\\\\\\\\ "<<flush<<endl;}

	if(Debug2){cout<<" /////////IDMA_Process::before generate DropletDag\\\\\\\\\\ "<<flush<<endl;}
	if(Debug2){M->generateDropletDag("MAfterPruneT.cpp");}
	if(Debug2){cout<<" /////////IDMA_Process::after generate DropletDag\\\\\\\\\\ "<<flush<<endl;}

	if(Debug2){cout<<" /////////IDMA_Process::before generate DotyGraph\\\\\\\\\\ "<<flush<<endl;}
	if(Debug2){M->generateDotyGraph("MAfterPrune.dot");}
	if(Debug2){cout<<" /////////IDMA_Process::after generate DotyGrpah\\\\\\\\\\ "<<flush<<endl;}

	if(Debug2){cout<<" /////////IDMA_Process::before generate DropletDag\\\\\\\\\\ "<<flush<<endl;}
	if(Debug2){createDagBackup->generateDropletDag("createDagBackupB4PruneT.cpp");}
	if(Debug2){cout<<" /////////IDMA_Process::after generate DropletDag\\\\\\\\\\ "<<flush<<endl;}

	if(Debug2){cout<<" /////////IDMA_Process::before generate DotyGraph\\\\\\\\\\ "<<flush<<endl;}
	if(Debug2){createDagBackup->generateDotyGraph("createDagBackupB4Prune.dot");}
	if(Debug2){cout<<" /////////IDMA_Process::after generate DotyGrpah\\\\\\\\\\ "<<flush<<endl;}

	if(Debug3){	cout<<" /////////IDMA_Process::Before skewCalc\\\\\\\\\ "<<flush<<endl;}
	vector<int> backupSkewVals = ID.CalcSkew(createDagBackup);
	if(Debug3){cout<<" /////////IDMA_Process::After skewCalc\\\\\\\\\ "<<flush<<endl;}

	//createDag backup should be the same as M after prune, but have multiple buffers and sample rather than just one.
	if(Debug3){cout<<" /////////IDMA_Process::Before prune createDagBackup\\\\\\\\\ "<<flush<<endl;}
	int skewIDbackup = ID.PruneDMRW(createDagBackup, backupSkewVals, valid);
	if(Debug3){cout<<" /////////IDMA_Process::Afer prune createDagBackup\\\\\\\\\ "<<flush<<endl;}//M->outputVertices();

	if(Debug2){cout<<" /////////IDMA_Process::before generate DropletDag\\\\\\\\\\ "<<flush<<endl;}
	if(Debug2){createDagBackup->generateDropletDag("createDagBackupAfterPruneT.cpp");}
	if(Debug2){cout<<" /////////IDMA_Process::after generate DropletDag\\\\\\\\\\ "<<flush<<endl;}

	if(Debug2){cout<<" /////////IDMA_Process::before generate DotyGraph\\\\\\\\\\ "<<flush<<endl;}
	if(Debug2){createDagBackup->generateDotyGraph("createDagBackupAfterPrune.dot");}
	if(Debug2){cout<<" /////////IDMA_Process::after generate DotyGrpah\\\\\\\\\\ "<<flush<<endl;}

	if(skewIDbackup == -1) //IF IDMA IS NOT APPLICABLE
	{
		//DiluteDroplet* db = new DiluteDroplet;
		//DiluteDroplet* di = new DiluteDroplet;
		db->Concentration = Rational(0, pow(2,num_ops));
		di->Concentration = Rational(pow(2,num_ops), pow(2,num_ops));
		DMRWvals = RoyDilute::RoyDilute_Process(db, di, num_ops, DesiredConcentrate, tolerance);
		DMRWnumW = DMRWvals.first;
		DMRWnumMS = DMRWvals.second;

		//find number of bits in binary representation for BS;
		//BSnumW = m-1
		//BSnumMS = m

		//TODO:::: CURRENTLY I DO NOT HAVE THE actual BINSEARCH CODE. I am using Griffith_Dilution as a substitute

		if(BSnumW == IDMAnumW and BSnumMS < IDMAnumMS)
		{
			//use BS
			cout<<"the final dag is the BinarySearch Dag"<<flush<<endl;
			//DiluteDroplet* db = new DiluteDroplet;
			//DiluteDroplet* di = new DiluteDroplet;
			db->Concentration = Rational(0, pow(2,num_ops));
			di->Concentration = Rational(pow(2,num_ops), pow(2,num_ops));
			BSvals = GriffDilute::GriffithDilute_Process(db, di, num_ops, DesiredConcentrate, tolerance);
			BSnumW = BSvals.first;
			BSnumMS = BSvals.second;
			finalnumW = BSvals.first;
			finalnumMS = BSvals.second;

		}
		else
		{
			//use DMRW
			cout<<"the final dag is the DMRW dag"<<flush<<endl;
			finalnumW = DMRWvals.first;
			finalnumMS = DMRWvals.second;
		}


		exit(1);
	}
	else //IDMA IS APPLICABLE
	{
		//DiluteDroplet* db = new DiluteDroplet;
		//DiluteDroplet* di = new DiluteDroplet;
		db->Concentration = Rational(0, pow(2,num_ops));
		di->Concentration = Rational(pow(2,num_ops), pow(2,num_ops));
		IDMAvals = IDMA_Process(num_ops, DesiredConcentrate, tolerance);
		IDMAnumW = IDMAvals.first;
		IDMAnumMS = IDMAvals.second;

		if(IDMAnumW <= DMRWnumW and IDMAnumMS < DMRWnumMS)
		{
			//use IDMA
			cout<<"the final dag is the IDMA dag"<<flush<<endl;
			finalnumW = IDMAvals.first;
			finalnumMS = IDMAvals.second;
		}
		else
		{
			//find number of bits in binary representation for BS;
			//BSnumW = m-1
			//BSnumMS = m

			//TODO:::: CURRENTLY I DO NOT HAVE THE actual BINSEARCH CODE. I am using Griffith_Dilution as a substitute

			if(BSnumW == IDMAnumW and BSnumMS < IDMAnumMS)
			{
				//use BS
				cout<<"the final dag is the BinarySearch Dag"<<flush<<endl;
				//DiluteDroplet* db = new DiluteDroplet;
				//DiluteDroplet* di = new DiluteDroplet;
				db->Concentration = Rational(0, pow(2,num_ops));
				di->Concentration = Rational(pow(2,num_ops), pow(2,num_ops));
				BSvals = GriffDilute::GriffithDilute_Process(db, di, num_ops, DesiredConcentrate, tolerance);
				BSnumW = BSvals.first;
				BSnumMS = BSvals.second;
				finalnumW = BSvals.first;
				finalnumMS = BSvals.second;
			}
			else
			{
				//use DMRW
				cout<<"the final dag is the DMRW dag"<<flush<<endl;
				//DiluteDroplet* db = new DiluteDroplet;
				//DiluteDroplet* di = new DiluteDroplet;
				db->Concentration = Rational(0, pow(2,num_ops));
				di->Concentration = Rational(pow(2,num_ops), pow(2,num_ops));
				DMRWvals = RoyDilute::RoyDilute_Process(db, di, num_ops, DesiredConcentrate, tolerance);
				DMRWnumW = DMRWvals.first;
				DMRWnumMS = DMRWvals.second;
			}
		}
	}
	cout<<"the final number of waste droplets is: "<<finalnumW<<flush<<endl;
	cout<<"the final number of MS droplets is: "<<finalnumMS<<flush<<endl;
	return;
}*/
