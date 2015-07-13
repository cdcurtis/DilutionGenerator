/*
 * IDMA_dilution.cc
 *
 *  Created on: Apr 23, 2013
 *      Author: Kenneth O'Neal
 */


 //TODO:: FOR EACH PROJECT!!! Ensure that all object files and built items go to new subfolder in the hierarchy. 
 //i.e all .o will go to ../obj/filename
 //all generated droplet dags and associated source will go to ../out/filename
 //all executables will remain in source folder
#include "../Headers/common.h"
#include "../../DagGen/Headers/DagGen.h"

using namespace std;

bool Dilute::CompareuID(DiluteDroplet* A, DiluteDroplet* B)
{
	return A->uID < B->uID;
}


Vertex2* Dilute::addVertex(DagGen * dag, VertexType vertexType, std:: string label, DiluteDroplet* dh, DiluteDroplet* dl, int dropsAvail, int dropsNeeded)
{
	int IDs = dag->getID();
	Vertex2* v =  new Vertex2(vertexType,label,IDs++, dl, dh, dropsAvail, dropsNeeded);
	dag->addVertex(vertexType, label);
	vertices.push_back(v);
	return v;
}

int Dilute::findVertex(int vertexID)
{
	int ret;
	for(unsigned i = 0;i < vertices.size(); i++)
	{
		if(vertices.at(i)->uniqueID == vertexID)
		{
			ret = i;
		}
		else
		{
			continue;
		}
	}
	return ret;
}


 void Dilute::assignVertCounts(DagGen * dag)
{
	//should modify to find all outgoing edges, then ndrops needed = sum(values from each)
	vector<Edge*> edges = dag->Edges();
	bool Debug = false;

	if(Debug)
	{
		cout<<"counts before assign counts"<<flush<<endl;
		for(unsigned i = 0; i < vertices.size(); i++)
		{
			cout<<"vertex "<<vertices.at(i)->label<<", dA = "<<vertices.at(i)->dropsAvail<<", dN = "<<vertices.at(i)->dropsNeeded<<flush<<endl;
		}
		cout<<endl;
	}

	for(int i = vertices.size()-1; i-- > 0; )
	{
		Vertex2* cur = vertices.at(i);
		if(Debug){cout<<"cur is: "<<cur->label<<", id is: "<<cur->uniqueID<<flush<<endl;}
		if(cur->type == DISPENSE)
		{
			if(Debug){cout<<"dispense case"<<flush<<endl;}
			cur->dropsAvail = 1;
			cur->dropsNeeded = 1;
		}
		else if(cur->type == SPLIT)
		{
			if(Debug){cout<<"split case"<<flush<<endl;}

			if(Debug){cout<<"split, count < 2"<<flush<<endl;}
			int value = 0;
			vector<Edge*> curOutgoing = dag->findOutgoingEdges(cur->uniqueID);
			if(Debug){cout<<"cur outgoing size is: "<<curOutgoing.size()<<flush<<endl;}
			for(unsigned j = 0; j < curOutgoing.size(); j++)
			{
				if(curOutgoing.at(j)->parent == cur->uniqueID)
				{
					//if(Debug){cout<<"edge being checked is: ("<<edges.at(j)->parent<<", "<<edges.at(j)->child<<"), cur is: "<<cur->uniqueID<<flush<<endl;}
					value = value + ceil((double)vertices.at(curOutgoing.at(j)->child)->dropsNeeded);
					if(Debug)
					{
						cout<<"edge is: ("<<vertices.at(curOutgoing.at(j)->parent)->label<<", "<<vertices.at(curOutgoing.at(j)->child)->label<<")"<<flush<<endl;
						cout<<"child is: "<<vertices.at(curOutgoing.at(j)->child)->label<<", with dN = "<<vertices.at(curOutgoing.at(j)->child)->dropsNeeded<<flush<<endl;
						//value = ceil((double)vertices.at(edges.at(j)->child)->dropsNeeded/2);
						cout<<"value ="<<value<<flush<<endl;
					}
				}
			}
			cur->dropsAvail = 2;
			cur->dropsNeeded = value;
		}
		else if(cur->type == MIX)
		{
			if(Debug){cout<<"Mix case"<<flush<<endl;}
			//Mixes drops needed should be equal to ceil(drops needed at child /2)
			int value = 0;
			for(unsigned j = 0; j < edges.size(); j++)
			{
				if(edges.at(j)->parent == cur->uniqueID)
				{
					value = ceil((double)vertices.at(edges.at(j)->child)->dropsNeeded/2);
					if(Debug)
					{
						cout<<"edge is: ("<<vertices.at(edges.at(j)->parent)->label<<", "<<vertices.at(edges.at(j)->child)->label<<")"<<flush<<endl;
						cout<<"child is: "<<vertices.at(edges.at(j)->child)->label<<", with dN = "<<vertices.at(edges.at(j)->child)->dropsNeeded<<flush<<endl;
						cout<<"value ="<<value<<flush<<endl;
					}
				}
			}
			cur->dropsNeeded = value; //assume one from each parent, only set to 0 if both have dropsavail
			cur->dropsAvail = 1;
		}
		else
		{
			if(Debug){cout<<"else case"<<flush<<endl;}
			cur->dropsNeeded = 1;
			cur->dropsAvail = 1;
		}
		if(Debug){cout<<endl;}
	}

	if(Debug)
	{
		cout<<"counts after assign counts"<<flush<<endl;
		for(unsigned i = 0; i < vertices.size(); i++)
		{
			cout<<"vertex "<<vertices.at(i)->label<<", dA = "<<vertices.at(i)->dropsAvail<<", dN = "<<vertices.at(i)->dropsNeeded<<flush<<endl;
		}
	}

	//exit(1);
	return;
}

 int Dilute::findCounter(VertexCounts* VC, Vertex* vertex)
{
	bool Debug = false;
	if(Debug){cout<<"start"<<flush<<endl;}
	string sample = "sample";
	string waste = "waste";
	int retCount;

	if(vertex->type == DISPENSE)
	{
		if(vertex->label.find(sample) != std::string::npos) //if sample
		{
			if(Debug){cout<<"value found was sample"<<flush<<endl;}
			VC->sampleCount = VC->sampleCount+1;
			retCount =  VC->sampleCount;
		}
		else //else buffer
		{
			if(Debug){cout<<"value found was buffer"<<flush<<endl;}
			VC->bufferCount = VC->bufferCount+1;
			retCount =  VC->bufferCount;
		}
	}
	else if(vertex->type == MIX)
	{
		if(Debug){cout<<"value found was mix"<<flush<<endl;}
		VC->mixCount= VC->mixCount+1;
		retCount = VC->mixCount;
	}
	else if(vertex->type == SPLIT)
	{
		if(Debug){cout<<"vale found was split"<<flush<<endl;}
		VC->splitCount = VC->splitCount+1;
		retCount =  VC->splitCount;
	}
	else if(vertex->type == WASTE)
	{
		if(Debug){cout<<"type was waste ???"<<flush<<endl;}
		retCount = 0;
	}
	else if(vertex->type == OUTPUT)
	{
		if(vertex->label.find(waste) != std::string::npos) //if waste
		{
			if(Debug){cout<<"value found was waste"<<flush<<endl;}
			VC->wasteCount = VC->wasteCount+1;
			retCount = VC->wasteCount;
		}
		else //else output, and how!
		{
			if(Debug){cout<<"case output, not waste, shouldn't happen"<<flush<<endl;}
			retCount = 0;
		}
	}
	if(Debug){cout<<"end"<<flush<<endl;}
	return retCount;
}

 string Dilute::findLabel(Vertex* vertex)
{
	bool Debug = false;
	if(Debug){cout<<"start"<<flush<<endl;}
	string sample = "sample";
	string waste = "waste";
	string retLabel;

	if(vertex->type == DISPENSE)
	{
		if(vertex->label.find(sample) != std::string::npos) //if sample
		{
			if(Debug){cout<<"value found was sample"<<flush<<endl;}
			retLabel = "sample";
		}
		else //else buffer
		{
			if(Debug){cout<<"value found was buffer"<<flush<<endl;}
			retLabel = "buffer";
		}
	}
	else if(vertex->type == MIX)
	{
		if(Debug){cout<<"value found was mix"<<flush<<endl;}
		retLabel = "mix";
	}
	else if(vertex->type == SPLIT)
	{
		if(Debug){cout<<"vale found was split"<<flush<<endl;}
		retLabel = "split";
	}
	else if(vertex->type == WASTE)
	{
		if(Debug){cout<<"type was waste ???"<<flush<<endl;}
		retLabel = "oops";
	}
	else if(vertex->type == OUTPUT)
	{
		if(vertex->label.find(waste) != std::string::npos) //if waste
		{
			if(Debug){cout<<"value found was waste"<<flush<<endl;}
			retLabel = "waste";
		}
		else //else output, and how!
		{
			if(Debug){cout<<"case output, not waste, shouldn't happen"<<flush<<endl;}
			retLabel = "output";
		}
	}
	if(Debug){cout<<"end"<<flush<<endl;}
	return retLabel;
}

 VertexCounts* Dilute::findVertexCounts(DagGen* dag)
{
	VertexCounts* counters = new VertexCounts;
	vector<Vertex*> vertices = dag->Vertices();
	counters->sampleCount = 0;
	counters->bufferCount = 0;
	counters->mixCount = 0;
	counters->splitCount = 0;
	counters->wasteCount = 0;
	for(unsigned i = 0; i < vertices.size(); i++)
	{
		string label = findLabel(vertices.at(i));
		if(label == "sample")
		{
			counters->sampleCount++;
		}
		else if(label == "buffer")
		{
			counters->bufferCount++;
		}
		else if(label == "mix")
		{
			counters->mixCount++;
		}
		else if(label == "split")
		{
			counters->splitCount++;
		}
		else if(label == "waste")
		{
			counters->wasteCount++;
		}
		else if(label == "output")
		{
			continue; //output has not counter value. This function has only one. 
		}
	}	

	return counters;
}

 void Dilute::outputAvailableDroplets(vector<DiluteDroplet*> AvailableDroplets)
{
	for(unsigned i = 0; i < AvailableDroplets.size(); i++)
	{
		//cout<<"ID: "<<AvailableDroplets.at(i)->uID<<", count = "<<AvailableDroplets.at(i)->count<<", concentration = "<<RattoFloat(AvailableDroplets.at(i)->Concentration)<<flush<<endl;
		cout<<"ID: "<<AvailableDroplets.at(i)->uID<<", count = "<<AvailableDroplets.at(i)->count<<", concentration = "<<flush;
		AvailableDroplets.at(i)->Concentration.display();
		//cout<<endl;
	    
	    if((Rational::RattoFloat(AvailableDroplets.at(i)->Concentration) != 1) and (Rational::RattoFloat(AvailableDroplets.at(i)->Concentration) != 0) )
	    {
	    	cout<<"preMix are : ("<<AvailableDroplets.at(i)->Mixes->DropletsPreMix.first->uID;
		cout<<", "<<AvailableDroplets.at(i)->Mixes->DropletsPreMix.second->uID<<")"<<flush<<endl;
	    }
	}
	return;
}

 void Dilute::outputMixSplits(vector<MixOp*> MixSplits)
{
	for(unsigned i = 0; i < MixSplits.size(); i++)
	{
		cout<<endl;
		cout<<"Mix "<<i<<" is droplet: "<<MixSplits.at(i)->DropletsPreMix.first->uID<<" and droplet: "<<MixSplits.at(i)->DropletsPreMix.second->uID<<" with counts "<<MixSplits.at(i)->DropletsPreMix.first->count<<","<<MixSplits.at(i)->DropletsPreMix.second->count<<endl;
		cout<<"resulting in Mixdroplet: "<<MixSplits.at(i)->DropletsPostMix.first->uID<<" and wastedroplet: "<<MixSplits.at(i)->DropletsPostMix.second->uID<<" with counts "<<MixSplits.at(i)->DropletsPostMix.first->count<<","<<MixSplits.at(i)->DropletsPostMix.second->count<<endl;
		//cout<<"with Lconcentration: "<<RattoFloat(MixSplits.at(i)->DropletsPostMix.first->Concentration)<<flush<<endl;
		//cout<<"with Rconcentration: "<<RattoFloat(MixSplits.at(i)->DropletsPostMix.second->Concentration)<<flush<<endl;
		cout<<"with Lconcentration: "<<flush;
		MixSplits.at(i)->DropletsPostMix.first->Concentration.display();
		cout<<"with Rconcentration: "<<flush;
		MixSplits.at(i)->DropletsPostMix.second->Concentration.display();
	}
	cout<<endl;
	return;
}

 void Dilute::clearDropletCounts(vector<DiluteDroplet*> AvailableDroplets)
{
	for(unsigned i = 0; i < AvailableDroplets.size(); i++) //remove after removing counting processs from above
	{
		AvailableDroplets.at(i)->count = 0;
	}
	return;
}

 void Dilute::adjustCounts(pair <vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals)
{
	vector<MixOp*> Mixes = DilutionVals.first;
	vector<DiluteDroplet*> Droplets = DilutionVals.second;

	for(unsigned i = 0; i < Droplets.size(); i++)
	{
		for(unsigned j = 0; j < Mixes.size(); j++)
		{
			if(Mixes.at(j)->DropletsPreMix.first == Droplets.at(i) or Mixes.at(j)->DropletsPreMix.second == Droplets.at(i))
			{
				if(Mixes.at(j)->DropletsPostMix.first->count >1)
				{
					int multiple = 0;
					multiple = Mixes.at(j)->DropletsPostMix.first->count / 2;
					if(Mixes.at(j)->DropletsPostMix.first->count %2 == 0)
					{
						Droplets.at(i)->count = Droplets.at(i)->count + multiple;
					}
					else
					{
						Droplets.at(i)->count = Droplets.at(i)->count + multiple + 1;
					}
				}
				else if(Mixes.at(j)->DropletsPostMix.second->count >1)
				{
					//current droplet count = count+postdropletcount-1
					Droplets.at(i)->count = Droplets.at(i)->count + Mixes.at(j)->DropletsPostMix.second->count -1;
				}
			}
		}
	}
	return;
}

 Rational Dilute::GetConcentrate(DiluteDroplet* node)
{
	return node->Concentration;
}

//sets the concentrate of a dilute droplet (to be used after mixing)
 void Dilute::SetConcentrate(DiluteDroplet* node, double ConcNum, double ConcDenom)
{
	node->Concentration.numer = ConcNum;
	node->Concentration.denom = ConcDenom;
}

//Gets the desire concentrate of a droplet
 Rational Dilute::GetDesiredConcentrate(DiluteDroplet* node) //returns c from variable list
{
	return node->DesiredConcentrate;
}

//Gets the tolerance allowed for a droplet
 Rational Dilute::GetTolerance(DiluteDroplet* node) {
	return node->Tolerance;
}

 DiluteDroplet* Dilute::CreateDroplet(Rational Concentration) {
	DiluteDroplet* temp = new DiluteDroplet;
	temp->Concentration = Concentration;
	return temp;
}

Rational Dilute::Dilute::CalcMixConc(DiluteDroplet* MixCl, DiluteDroplet* MixCh) {
	int numer = (MixCl->Concentration.numer + MixCh->Concentration.numer) / 2;
	Rational PostMix = Rational(numer, MixCl->Concentration.denom);
	return PostMix;
}

void Dilute::calcNumDropsFin(pair <vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals)
{
	bool Debug = false;
	//RoyDilute dag;
	vector<MixOp*> Mixes = DilutionVals.first;
	vector<DiluteDroplet*> AvailableDroplets = DilutionVals.second;

	std::sort(AvailableDroplets.begin(), AvailableDroplets.end(), CompareuID);

	//wipe old counts
	clearDropletCounts(AvailableDroplets);

	//establish new counts
	queue<DiluteDroplet*> myQueue;
	myQueue.push(AvailableDroplets.at(AvailableDroplets.size()-1)); //push last
	AvailableDroplets.at(AvailableDroplets.size()-1)->count++;

	while(! myQueue.empty())
	{
		DiluteDroplet* cur = myQueue.front();
		myQueue.pop();
	if(Debug){cout<<"cure droplet is : "<<cur->uID<<flush<<endl;}

		if(Rational::RattoFloat(cur->Mixes->DropletsPreMix.first->Concentration) != 1 and Rational::RattoFloat(cur->Mixes->DropletsPreMix.first->Concentration) != 0)
		{
			myQueue.push(cur->Mixes->DropletsPreMix.first);
			if(Debug){cout<<"droplet pushed1 in count is: "<<cur->Mixes->DropletsPreMix.first->uID<<flush<<endl;}
			cur->Mixes->DropletsPreMix.first->count++;
		}
		else
		{
			cur->Mixes->DropletsPreMix.first->count++;
		}
		if(Rational::RattoFloat(cur->Mixes->DropletsPreMix.second->Concentration) != 1 and Rational::RattoFloat(cur->Mixes->DropletsPreMix.second->Concentration) != 0)
		{
			myQueue.push(cur->Mixes->DropletsPreMix.second);
			if(Debug){cout<<"droplet pushed2 in count is: "<<cur->Mixes->DropletsPreMix.second->uID<<flush<<endl;}
			cur->Mixes->DropletsPreMix.second->count++;
		}
		else
		{
			cur->Mixes->DropletsPreMix.second->count++;
		}
	}

	return;
}

 void Dilute::DeleteM(vector<MixOp*> M) {
	for (unsigned i = 0; i < M.size(); i++) {
		delete M.at(i)->DropletsPostMix.first;
		delete M.at(i)->DropletsPostMix.second;
		delete M.at(i)->DropletsPreMix.first;
		delete M.at(i)->DropletsPreMix.second;
		delete M.at(i);
	}
}

 void Dilute::DeleteD(vector<pair<DiluteDroplet*, DiluteDroplet*> > D) {
	for (unsigned i = 0; i < D.size(); i++) {
		delete D.at(i).first;
		delete D.at(i).second;
	}
	D.clear();
}

VertexCounts* Dilute::CreateDagRoy(DagGen * dag, pair< vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals)
{

	bool Debugger = false;
	bool Debug2 = false;
	//RoyDilute dag;
	vector<MixOp*> Mixes = DilutionVals.first;
	vector<DiluteDroplet*> AvailableDroplets = DilutionVals.second;

	std::sort(AvailableDroplets.begin(), AvailableDroplets.end(), CompareuID);

	if(Debug2)
	{
		cout<<"output available droplets in create dag after sort"<<flush<<endl;
		outputAvailableDroplets(AvailableDroplets);
		outputMixSplits(Mixes);
	}

	//given counts and given sequence of mixes I can produce the correct sequence of nodes
	//optimality of node and edge generation is not considered here as ordering will be handled by the scheduling
	//phase of the synthesis process.
	//Attempting to utilize BFS to peform backtrace/generate nodes.

	//establish new counts
	queue<DiluteDroplet*> myQueue;
	int iter = 2;

	myQueue.push(AvailableDroplets.at(iter)); //push last


	int sampleCount = 1;
	int bufferCount = 1;
	int mixCount = 1;
	int splitCount = 1;
	int wasteCount = 1;

	string sample = "sample";
	string buffer = "buffer";
	string mix = "mix";
	string split = "split";
	string waste = "waste";

	vector<DiluteDroplet*> prevPushed;

	while(!myQueue.empty())
	{
		if(Debugger) { cout<<"start while"<<flush<<endl;}
		DiluteDroplet* cur = myQueue.front();
		if(Debugger) { cout<<"the cur droplet after pop is: "<<cur->uID<<flush<<endl;}
		if(Debugger){cout<<"cur's prelim droplets are: ("<<flush;}
		if(Debugger){cout<<cur->Mixes->DropletsPreMix.first->uID<<","<<cur->Mixes->DropletsPreMix.second->uID<<")"<<flush<<endl;}
		if(Debugger)
		{
			cout<<"with concentrations: ";
			cur->Mixes->DropletsPreMix.first->Concentration.display();
			cout<<", ";
			cur->Mixes->DropletsPreMix.second->Concentration.display();
			cout<<")"<<flush<<endl;
		}
		myQueue.pop();


		//establish incoming mix droplets pre1
		Vertex2* pre1;
		Vertex2* pre2;		

		if(Rational::RattoFloat(cur->Mixes->DropletsPreMix.first->Concentration) == 1) //is sample dispense
		{
			if(Debugger){cout<<"pre1 if"<<flush<<endl; }
			ostringstream convert1;
			convert1 << sampleCount;
			string label1 = sample + convert1.str();

			pre1 = addVertex(dag, DISPENSE, label1, NULL, NULL, 1, 1);
			pre1->type = DISPENSE;
			pre1->dh = NULL; //added for dh check in expanddag
			pre1->dl = NULL; //added for dh check in expanddag
			sampleCount++;
		}
		else if(Rational::RattoFloat(cur->Mixes->DropletsPreMix.first->Concentration) == 0)
		{
			if(Debugger){cout<<"pre1 else if"<<flush<<endl; }
			cout<<"buffer count in pre1 else if is: "<<bufferCount<<flush<<endl;
			ostringstream convert2;
			convert2 << bufferCount;
			string label2 = buffer + convert2.str();
			pre1 = addVertex(dag, DISPENSE, label2, NULL, NULL, 1, 1);
			pre1->type = DISPENSE;
			pre1->dh = NULL; //added for dh check in expanddag
			pre1->dl = NULL; //added for dh check in expanddag
			bufferCount++;
		}
		else
		{
			if(Debugger){cout<<"pre1 else"<<flush<<endl;}
			for(unsigned v = 0; v < vertices.size(); v++) //check all vertices
			{
				if(vertices.at(v)->label.find(split) != std::string::npos) //check if it's a split (outgoing)
				{
					//cout<<"vertices at (v) in pre1 else is: "<<vertices.at(v)->label<<", "<<vertices.at(v)->dh<<flush<<endl;
				//	cout<<"cur pre1 droplet first is: "<<cur->Mixes->DropletsPreMix.first<<flush<<endl;
					if(vertices.at(v)->dh->uID == cur->Mixes->DropletsPreMix.first->uID)// or vertices.at(v)->dh == cur->Mixes->DropletsPreMix.second) //if the non wastedroplet is the current one
					{
						//do not create new vertex, just connect to current mix vertex.
						if(Debugger){cout<<"pre1 else inner"<<flush<<endl; }
						pre1 = vertices.at(v);
					//	cout<<"pre 1 in else inner is: "<<pre1->dh->uID<<" conc = ";
					//	pre1->dh->Concentration.display();
					}
				}
			}
		}

		//establish incoming mix droplets pre2
		if(Rational::RattoFloat(cur->Mixes->DropletsPreMix.second->Concentration) == 1) //is sample dispense
		{
			if(Debugger){cout<<"pre2 if"<<flush<<endl; }
			ostringstream convert1;
			convert1 << sampleCount;
			string label1 = sample + convert1.str();
			pre2 = addVertex(dag, DISPENSE, label1, NULL, NULL, 1, 1);
			pre2->type = DISPENSE;
			pre2->dh = NULL; //added for dh check in expanddag
			pre2->dl = NULL; //added for dh check in expanddag
			sampleCount++;
		}
		else if(Rational::RattoFloat(cur->Mixes->DropletsPreMix.second->Concentration) == 0)
		{
			if(Debugger){cout<<"pre2 else if"<<flush<<endl; }
			//cout<<"buffer count pre2 else if is: "<<bufferCount<<flush<<endl;
			ostringstream convert2;
			convert2 << bufferCount;
			string label2 = buffer + convert2.str();
			pre2 = addVertex(dag, DISPENSE, label2, NULL, NULL, 1, 1);
			if(Debugger){cout<<"output after addvertex pre2 else if"<<flush<<endl;}
			pre2->type = DISPENSE;
			pre2->dh = NULL; //added for dh check in expanddag
			pre2->dl = NULL; //added for dh check in expanddag
			bufferCount++;
		}
		else
		{
			if(Debugger){cout<<"pre 2 else"<<flush<<endl;}
			for(unsigned v = 0; v < vertices.size(); v++) //check all vertices
			{
				if(vertices.at(v)->label.find(split) != std::string::npos) //check if it's a split (outgoing)
				{
					if(Debugger){cout<<"vertices at (v) in pre2 else is: "<<vertices.at(v)->label<<", "<<vertices.at(v)->dh<<flush<<endl;}
					if(Debugger){cout<<"cur pre2 droplet second is: "<<cur->Mixes->DropletsPreMix.second<<flush<<endl;}
					if(vertices.at(v)->dh->uID == cur->Mixes->DropletsPreMix.second->uID) //if the non wastedroplet is the current one
					{
						if(Debugger){cout<<"pre 2 else inner"<<flush<<endl; }
						//do not create new vertex, just connect to current mix vertex.
						pre2 = vertices.at(v);
						//cout<<"pre 2 in else inner is: "<<pre2->dh->uID<<" conc = ";
						//pre2->dh->Concentration.display();
					}
				}
			}
		}

		//create Mix vertex
		if(Debugger){cout<<"before create Mix vertex"<<flush<<endl;}
		ostringstream convertMix;
		convertMix << mixCount;

		string mixLabel = mix + convertMix.str();
		Vertex2* mix = addVertex(dag, MIX, mixLabel, NULL, NULL, 1, 1);
		if(Debugger){cout<<"after add vertex end of while before add mix"<<flush<<endl;}

		mix->type = MIX;
		mixCount++;


		if(Debugger){cout<<"before add edges, pre1 = "<<pre1->label<<", pre2 = "<<pre2->label<<", mix is: "<<mix->label<<flush<<endl;}
		dag->addEdge(pre1, mix);
		dag->addEdge(pre2, mix);

		if(Debugger){ cout<<"mix added"<<flush<<endl;}

		mix->dh = cur->Mixes->DropletsPreMix.first;
		mix->dl = cur->Mixes->DropletsPreMix.second;

		//create split vertex, connect mix to split, and populate dh, dl (outgoing drops of split)
		ostringstream convertSplit;
		convertSplit << splitCount;
		string splitLabel = split + convertSplit.str();
		Vertex2* split = addVertex(dag, SPLIT, splitLabel, NULL, NULL, 1, 1);
		split->type = SPLIT;
		splitCount++;

		dag->addEdge(mix, split);
		if(Debugger){cout<<"split added"<<flush<<endl; }

		split->dh = cur->Mixes->DropletsPostMix.first;
		split->dl = cur->Mixes->DropletsPostMix.second;

		if(Debugger){cout<<"the post split droplets are: ("<<split->dh->uID<<","<<split->dh->count<<"),  and ("<<split->dl->uID<<","<<split->dl->count<<")"<<flush<<endl;}
		if(Debugger){cout<<"the current droplet is:  ("<<cur->uID<<","<<cur->count<<")"<<flush<<endl;}
		if(Debugger){cout<<"the pre cur droplets are: ("<<cur->Mixes->DropletsPreMix.first->uID<<","<<cur->Mixes->DropletsPreMix.first->count<<") and ("<<cur->Mixes->DropletsPreMix.second->uID<<","<<cur->Mixes->DropletsPreMix.second->count<<")"<<flush<<endl;}
		if(Debugger){cout<<"the post cur droplet is: ("<<cur->Mixes->DropletsPostMix.first->uID<<","<<cur->Mixes->DropletsPostMix.first->count<<") and ("<<cur->Mixes->DropletsPostMix.second->uID<<","<<cur->Mixes->DropletsPostMix.second->count<<")"<<flush<<endl;}


		//ensures that in the case of a parent being the subsequent child
		//the parent, nor the child will be added more than once.
		//each vertex is added and explored only once, avoiding circular dependencies.
		for(unsigned p = 0; p < Mixes.size(); p++)
		{
		    if(Debugger){cout<<"prevals @p are: ("<<Mixes.at(p)->DropletsPreMix.first->uID<<", "<<Mixes.at(p)->DropletsPreMix.second->uID<<")"<<flush<<endl;}
		    //removed to shoren output, put back in
			//if(Debug){cout<<"Mixes first == cur, mix is: "<<Mixes.at(p)->DropletsPreMix.first->uID<<flush<<endl;}
			//if(Debug){cout<<"Mixes second == cur, mix is: "<<Mixes.at(p)->DropletsPreMix.second->uID<<flush<<endl;}
			if(Mixes.at(p)->DropletsPreMix.first == cur)
			{
				bool pushed = false;
				for(unsigned t = 0; t < prevPushed.size(); t++) //ensure pushed only once
				{
					if(Mixes.at(p)->DropletsPostMix.first->uID == prevPushed.at(t)->uID)
					{
						pushed = true;
					}
				}
				if(pushed == false)
				{
					if(Debugger){cout<<"pushed first"<<flush<<endl;}
					if(Debugger){cout<<"the droplet pushed is: "<<Mixes.at(p)->DropletsPostMix.first->uID<<flush<<endl;}
					myQueue.push(Mixes.at(p)->DropletsPostMix.first); //do not push second, always waste
					prevPushed.push_back(Mixes.at(p)->DropletsPostMix.first);
				}
			}
			if(Mixes.at(p)->DropletsPreMix.second == cur)
			{
			    //cout<<"the preMix.second droplet for checking pushed second is: "<<Mixes.at(p)->DropletsPreMix.second->uID<<", cur is: "<<cur->uID<<flush<<endl;

				bool pushed = false;
				for(unsigned t = 0; t < prevPushed.size(); t++)
				{
					if(Mixes.at(p)->DropletsPostMix.first->uID == prevPushed.at(t)->uID)
					{
						if(Debugger){cout<<"it appears that pushed = true "<<Mixes.at(p)->DropletsPostMix.first->uID<<", "<<prevPushed.at(t)->uID<<flush<<endl;}
						pushed = true;
					}
				}
				if(pushed == false)
				{
					if(Debugger){cout<<"pushed second"<<flush<<endl; }
					myQueue.push(Mixes.at(p)->DropletsPostMix.first); //do not push second, always waste
					if(Debugger){cout<<"the droplet pushed is: "<<Mixes.at(p)->DropletsPostMix.first->uID<<flush<<endl;}
					prevPushed.push_back(Mixes.at(p)->DropletsPostMix.first);
				}
			}
		}
	}

	if(Debugger){cout<<"add output"<<flush<<endl; }
	//add output droplet and edge to final split node.
	int fsplitIndex = 0;
	for(unsigned i = 0; i < vertices.size(); i++)
	{
		if(vertices.at(i)->type == SPLIT)
		{
			fsplitIndex = i;
		}
	}

	ostringstream convert;
	convert << 1;
	string outLabel = "Output"+convert.str();
	Vertex2 * out = addVertex(dag, OUTPUT, outLabel, NULL, NULL, 1, 1);

	dag->addEdge(fsplitIndex, out->uniqueID);
	out->dh = NULL;
	out->dl = NULL;

	if(Debug2)
	{
		cout<<"dag outptuVertices after createDag"<<flush<<endl;
		dag->outputVertices();
		dag->outputEdges();
	}

	VertexCounts* ret = new VertexCounts;
	ret->bufferCount = bufferCount;
	ret->mixCount = mixCount;
	ret->sampleCount = sampleCount;
	ret->splitCount = splitCount;
	ret->wasteCount = wasteCount;

//	DagGen* copy = new DagGen(*dag);
//	if(Debug2)
//	{
//		cout<<"copy outputVertices = "<<flush<<endl;
//		copy->outputVertices();
//		copy->outputEdges();
//	}
	//TODO CDCURTIS CHANGE
	ret->H = dag;
	//need this copy?
	return ret;
}

 void Dilute::expander(DagGen * dag, VertexCounts* VC)
{
	bool Debug = false;
	bool Debug2 = false;
	bool Debug3 = false;
	int childLoc;
	int parentLoc;

	int sampleCount = VC->sampleCount;
	int bufferCount = VC->bufferCount;
	int mixCount = VC->mixCount;
	int splitCount = VC->splitCount;
	vector< pair <Vertex2*, Vertex2*> > allAddedPairs;
	vector<Edge*> edges = dag->Edges();


	if(Debug)
	{
		cout<<"dag before call assignVertCounts"<<flush<<endl;
		dag->outputVertices();
		dag->outputEdges();
	}
	assignVertCounts(dag);

	for(unsigned i = 0; i < vertices.size(); i++)
	{
		Vertex2* cur = vertices.at(i);
		if(cur->dropsAvail < cur->dropsNeeded)
		{
			if(Debug){cout<<"dA<dN"<<flush<<endl;}

			while(cur->dropsAvail < cur->dropsNeeded)
			{
				if(Debug3){cout<<"while < "<<flush<<endl<<endl<<endl;}
				int tempor;

				queue<Vertex2*> curCopies;
				curCopies.push(cur);
				vector< pair <Vertex2*, Vertex2*> > vertexAdded;
				while(!curCopies.empty())
				{
					int tempor2;
					if(Debug3){cout<<"while 2"<<flush<<endl;}

					Vertex2* check = curCopies.front();
					curCopies.pop();
					vector<Edge*> curChilds;
					vector<Edge*> curParents;

					for(unsigned j = 0; j < edges.size(); j++)
					{
						if(edges.at(j)->parent == check->uniqueID)
						{
							Edge* temp = edges.at(j);
							curChilds.push_back(edges.at(j));
						}
					}

					for(unsigned j = 0; j < edges.size(); j++)
					{
						if(edges.at(j)->child == check->uniqueID)
						{
							Edge* temp = edges.at(j);
							curParents.push_back(edges.at(j));
						}
					}
					if(Debug){cout<<"vertex being checked from top of queue is: "<<check->label<<", num parents: "<<curParents.size()<<", num childs: "<<curChilds.size()<<flush<<endl;}


					Vertex2* pre1;
					int countVal = findCounter(VC, check);
					ostringstream convert1;
					convert1 << countVal;
					string label1 = findLabel(check) + convert1.str();
					pre1 = addVertex(dag, check->type, label1, NULL, NULL, 1, 1);

					pre1->type =  check->type;
					pre1->dh = check->dh; //added for dh check in expanddag
					pre1->dl = check->dl; //added for dh check in expanddag
					sampleCount++;
					pre1->dropsAvail = 2;
					pre1->dropsNeeded = 0;
					if(Debug){cout<<"check was copied"<<flush<<endl;}

					pair<Vertex2*, Vertex2*> pusher = make_pair(pre1, check);
					if(Debug3){cout<<"pusher vertex added pair is: ("<<pusher.first->label<<", "<<pusher.second->label<<")"<<flush<<endl;}
					vertexAdded.push_back(pusher);
					allAddedPairs.push_back(pusher);

					if(check->type == SPLIT){check->dropsNeeded = check->dropsNeeded - 2;}
					else{check->dropsNeeded = check->dropsNeeded-1;}
					if(Debug3){cout<<"check: "<<check->label<<", dN = "<<check->dropsNeeded<<", dA = "<<check->dropsAvail<<flush<<endl;}

					if(Debug3){cout<<"current vertex, before enter while is: "<<vertices.at(i)->label<<"dn = "<<vertices.at(i)->dropsNeeded<<", da = "<<vertices.at(i)->dropsAvail<<flush<<endl;}
					bool firstCopy = true;
					for(unsigned j = 0; j < curParents.size(); j++) //for each parent of check, if its dispense make new and connect to copy.
					{
						int curParentIndex = curParents.at(j)->parent;
						Vertex2* curParentVertex = vertices.at(curParents.at(j)->parent);
						if(Debug){cout<<"checking parents: "<<curParentVertex->label<<flush<<endl;}

						if(curParentVertex->type == DISPENSE)
						{
							if(Debug){cout<<"the type is dispense"<<flush<<endl;}

							//duplicate and connect
							int countVal = findCounter(VC, curParentVertex);
							Vertex2* pre2;
							ostringstream convert1;
							convert1 << countVal;
							string label1 = findLabel(curParentVertex)+ convert1.str();

							pre2 = addVertex(dag, DISPENSE, label1, NULL, NULL, 1, 1);
							pre2->type = DISPENSE;
							pre2->dh = NULL; //added for dh check in expanddag
							pre2->dl = NULL; //added for dh check in expanddag
							pre2->dropsAvail = 1;
							pre2->dropsNeeded = 1;
							sampleCount++;

							pair<Vertex2*, Vertex2*> pusher2 = make_pair(pre2, curParentVertex);
							if(Debug3){cout<<"pusher2 vertex added pair is: ("<<pusher2.first->label<<", "<<pusher2.second->label<<")"<<flush<<endl;}
							vertexAdded.push_back(pusher2);
							allAddedPairs.push_back(pusher2);

							//find corresponding copy
							if(Debug){cout<<"dispense copied"<<flush<<endl;}
							for(unsigned v = 0; v < vertexAdded.size(); v++)
							{
								if(vertexAdded.at(v).second == check)
								{
									//connect edge from copy dispense to newly added copy of vertex check
									if(vertexAdded.at(v).first->type != DISPENSE)
									{
										dag->addEdge(pre2, pre1); //was previously pre2, vertexAdded.at(v).first
										if(Debug3){cout<<"dispense added("<<pre2->label<<", with edge: ("<<pre2->label<<", "<<vertexAdded.at(v).first->label<<")"<<flush<<endl;}
									}
									else
									{
										if(Debug3){cout<<"dispense not added, first was dispense"<<flush<<endl;}
									}
								}
							}
							if(Debug){cout<<"dispense connected"<<flush<<endl;}
						}
						else
						{
							//if numavail<numneeded push entry onto queue. and not dispense
							if(curParentVertex->dropsAvail >= curParentVertex->dropsNeeded) //swapped conditions and remove push in if
							{
								if(Debug){cout<<"dropsAvail >= dropsNeeded push onto queue, parent is: "<<curParentVertex->label<<flush<<endl;}
								//FORMER
								if(curParentVertex->type == SPLIT)
								{
									curParentVertex->dropsAvail = curParentVertex->dropsAvail -2;//vertices.at(curParents.at(j)->parent)->dropsAvail;
									curParentVertex->dropsNeeded = curParentVertex->dropsNeeded - 2;
								}
								else
								{
									curParentVertex->dropsAvail = curParentVertex->dropsAvail -1;//vertices.at(curParents.at(j)->parent)->dropsAvail;
									curParentVertex->dropsNeeded = curParentVertex->dropsNeeded - 1;
								}

								if(Debug3){cout<<"curParent: "<<curParentVertex->label<<", dN = "<<curParentVertex->dropsNeeded<<", dA = "<<curParentVertex->dropsAvail<<flush<<endl;}
								///CRITICAL EDIT BELOW, changed which values is start of edge...
								//if curParentVertex copy exists, then use that copy not the curParent
								for(unsigned c = 0; c < vertexAdded.size(); c++)
								{
									if(vertexAdded.at(c).second->uniqueID == check->uniqueID)
									{
										if(Debug3){cout<<"this1 mofo edge added: ("<<curParentVertex->label<<", "<<vertexAdded.at(c).first->label<<")"<<flush<<endl;}
										dag->addEdge(curParentVertex, vertexAdded.at(c).first);//connecting actual and not copy
									}
								if(Debug){cout<<"pushed to curCopies >= than case: "<<vertices.at(curParents.at(j)->parent)->label<<flush<<endl;}
								}
							}
							else if(curParentVertex->dropsAvail < curParentVertex->dropsNeeded)
							{
								if(Debug3){ cout<<"else dropsAvail < DropsNeeded, copy and push onto queue, parent is: "<<curParentVertex->label<<flush<<endl;}
								int countVal = findCounter(VC, curParentVertex);
								Vertex2* pre3;
								ostringstream convert1;
								convert1 << countVal;
								string label1 = findLabel(curParentVertex) + convert1.str();

								pre3 = addVertex(dag, curParentVertex->type, label1, NULL, NULL, 1, 1);
								pre3->type = curParentVertex->type;
								pre3->dh = NULL; //added for dh check in expanddag
								pre3->dl = NULL; //added for dh check in expanddag
								sampleCount++;

								//establish pre3 counts
								if(pre3->type == SPLIT)
								{
									pre3->dropsAvail = 2;
									pre3->dropsNeeded = 0;
								}
								else
								{
									pre3->dropsAvail = 1;
									pre3->dropsNeeded = 1;
								}
								if(Debug3){cout<<"curparent b4 decrement needed"<<curParentVertex->label<<", dN = "<<curParentVertex->dropsNeeded<<", dA = "<<curParentVertex->dropsAvail<<flush<<endl;}
								if(curParentVertex->type == SPLIT){curParentVertex->dropsNeeded = curParentVertex->dropsNeeded - 2;}
								else{curParentVertex->dropsNeeded = curParentVertex->dropsNeeded-1;}
								if(Debug3){cout<<"curParent after decrement: "<<curParentVertex->label<<", dN = "<<curParentVertex->dropsNeeded<<", dA = "<<curParentVertex->dropsAvail<<flush<<endl;}


								pair<Vertex2*, Vertex2*> pusher3 = make_pair(pre3, curParentVertex);
								if(Debug3){cout<<"pusher3 vertex added pair is: ("<<pusher3.first->label<<", "<<pusher3.second->label<<")"<<flush<<endl;}
								vertexAdded.push_back(pusher3);
								allAddedPairs.push_back(pusher3);

								//find corresponding copy and connect all outgoing edges properly
								vector<Edge*> copyOut; //
								for(unsigned e = 0; e < edges.size(); e++) //find all edges where parent is the parent
								{
									if(edges.at(e)->parent == curParentIndex and vertices.at(edges.at(e)->child)->type != OUTPUT)
									{
										if(Debug3){cout<<"edge pushed is: ("<<vertices.at(edges.at(e)->child)->label<<", "<<vertices.at(edges.at(e)->parent)->label<<")"<<flush<<endl;}
										copyOut.push_back(edges.at(e));
									}
								}

								for(unsigned v = 0; v < vertexAdded.size(); v++) //set all parents of parent to point to edge copied
								{
									if(vertexAdded.at(v).second == curParentVertex) //firs == copy second== original
									{
										for(unsigned e = 0; e < copyOut.size(); e++)
										{
											if(Debug){cout<<"pure copy added ("<<pre1->label<<"with edge ("<<vertexAdded.at(v).first->label<<", "<<pre1->label<<")"<<flush<<endl;}
											dag->addEdge(vertexAdded.at(v).first, pre1); //add edge from vertex to copy of queue.pop
											curCopies.push(curParentVertex);

											if(Debug){cout<<"pushed to curCopies < case is: "<<curParentVertex->label<<flush<<endl;}
										}
									}
								}

								if(Debug){cout<<"edges added"<<flush<<endl;}
							}
						}
					}
				}
			}
		}
	}

	if(Debug3)
	{
		cout<<"all added pairs size is: "<<allAddedPairs.size()<<flush<<endl;
		for(unsigned i = 0; i < allAddedPairs.size(); i++)
		{
		cout<<"pair: ("<<allAddedPairs.at(i).first->label<<", "<<allAddedPairs.at(i).second->label<<")"<<flush<<endl;
		}
	}

	if(Debug3){cout<<"before edge corrections"<<flush<<endl;}
	edgeCorrection(dag, allAddedPairs);
	if(Debug3){cout<<"after edge corrections"<<flush<<endl;}
	
	if(Debug3){cout<<"before add waste"<<flush<<endl;}
	addWaste(dag);
	if(Debug3){cout<<"after add waste"<<flush<<endl;}
	return;
}

 void Dilute::edgeCorrection(DagGen * dag, vector< pair <Vertex2*, Vertex2*> > allAddedPairs)
{
	bool Debug = false;
	if(Debug)
	{
		dag->outputVertices();
		dag->outputEdges();
	}

	/*vector<Edge*> temp = dag.findOutgoingEdges(32);
	cout<<"split 14 outgoing in edgecorrection"<<flush<<endl;
	cout<<"vertices.at(32) is: "<<vertices.at(32)->label<<flush<<endl;
	cout<<"temp size is: "<<temp.size()<<flush<<endl;
	for(unsigned i = 0; i < temp.size(); i++)
	{
		cout<<'edge '<<i<<" i is: ("<<temp.at(i)->parent<<", "<<temp.at(i)->child<<")"<<flush<<endl;
	}
	exit(1);
	*/
	//fix outgoing edges
	for(unsigned i = 0; i < vertices.size(); i++)
	{
		if(vertices.at(i)->type == DISPENSE or vertices.at(i)->type == OUTPUT)
		{
			if(Debug){cout<<"if dispense or output"<<flush<<endl;}
			continue;
		}
		else if(vertices.at(i)->type == MIX)
		{
			if(Debug){cout<<"else if vertices.at(i)-> type == MIX, vertex is: "<<vertices.at(i)->label<<flush<<endl;}
			vector<Vertex2*> copies;
			for(unsigned j = 0; j < allAddedPairs.size(); j++)
			{
				if(allAddedPairs.at(j).second->uniqueID == vertices.at(i)->uniqueID)
				{
					copies.push_back(allAddedPairs.at(j).first);
				}
			}
			if(Debug){cout<<"copies Mix size is:"<<copies.size()<<flush<<endl;}

			vector<Edge*> outgoing = dag->findOutgoingEdges(vertices.at(i)->uniqueID);
			if(outgoing.size() > 1) //if a mix has more than one outgoing edge, move extras to copies
			{
				if(Debug){cout<<"type mix, outgoing > 1"<<flush<<endl;}
				while(outgoing.size() > 1)
				{
					if(Debug){cout<<"vertex is: "<<vertices.at(i)->label<<flush<<endl;}					//exit(1);
					for(unsigned l = 0; l < copies.size(); l++)
					{
						vector<Edge*> outgoing2 = dag->findOutgoingEdges(copies.at(l)->uniqueID);
						if(Debug){cout<<"copy is: "<<copies.at(l)->label<<flush<<endl;}
						if(outgoing2.size() > 1)
						{
							continue;
						}
						else
						{
							Edge* temp = outgoing.back();
							if(Debug){cout<<"temp edge = ("<<vertices.at(temp->parent)->label<<", "<<vertices.at(temp->child)->label<<")"<<flush<<endl;}
							outgoing.pop_back();
							temp->parent = copies.at(l)->uniqueID;
							outgoing2.push_back(temp);
							if(Debug){cout<<"edge pushed to copy= ("<<vertices.at(temp->parent)->label<<", "<<vertices.at(temp->child)->label<<")"<<flush<<endl;}
						}
					}
				}
			}
			else
			{
				continue;
			}
		}
		else if(vertices.at(i)->type == SPLIT)
		{
			if(Debug){cout<<"else if vertices.at(i)-> type == SPLIT, vertex is: "<<vertices.at(i)->label<<flush<<endl;}
			vector<Vertex2*> copies;
			for(unsigned j = 0; j < allAddedPairs.size(); j++)
			{
				if(Debug){cout<<"all added pairs first is: "<<allAddedPairs.at(j).first->uniqueID<<", second is: "<<allAddedPairs.at(j).second->uniqueID<<flush<<endl;}
				if(Debug){cout<<"vertex id is: "<<vertices.at(i)->uniqueID<<flush<<endl;}
				if(allAddedPairs.at(j).second->uniqueID == vertices.at(i)->uniqueID)
				{
					copies.push_back(allAddedPairs.at(j).first);
				}
			}
			if(Debug){cout<<"copies SPLIT size is: "<<copies.size()<<flush<<endl;}
			if(Debug){cout<<"vertices at i is: "<<vertices.at(i)->uniqueID<<","<<vertices.at(i)->label<<flush<<endl;}
			vector<Edge*> outgoing = dag->findOutgoingEdges(vertices.at(i)->uniqueID);
			if(Debug){cout<<"outgoing size is: "<<outgoing.size()<<flush<<endl;}
			if(Debug){cout<<"outgoing size is: "<<outgoing.size()<<flush<<endl;}
			vector<Edge*> temp = dag->findOutgoingEdges(vertices.at(i)->uniqueID);
			if(Debug)
			{
				for(unsigned iter = 0; iter < temp.size(); iter++)
				{
					cout<<"edge "<<iter<<" i is: ("<<temp.at(iter)->parent<<", "<<temp.at(iter)->child<<")"<<flush<<endl;
				}
			}
	
			if(outgoing.size() > 2) //if a mix has more than one outgoing edge, move extras to copies
			{
				if(Debug){cout<<"if outoing.size > 2"<<flush<<endl;}
				while(outgoing.size() > 2)
				{
					if(Debug){cout<<"vertex is: "<<vertices.at(i)->label<<flush<<endl;}
					//exit(1);
					for(unsigned l = 0; l < copies.size(); l++)
					{
						vector<Edge*> outgoing2 = dag->findOutgoingEdges(copies.at(l)->uniqueID);
						if(Debug){cout<<"copy is: "<<copies.at(l)->uniqueID<<flush<<endl;}
						if(outgoing2.size() > 2)
						{
							if(Debug){cout<<"outgoing2 size > 2"<<flush<<endl;}
							continue;
						}
						else
						{
							Edge* temp = outgoing.back();
							if(Debug){cout<<"temp edge2 = ("<<vertices.at(temp->parent)->label<<", "<<vertices.at(temp->child)->label<<")"<<flush<<endl;}
							outgoing.pop_back();
							temp->parent = copies.at(l)->uniqueID;
							outgoing2.push_back(temp);
							if(Debug){cout<<"edge pushed to copy2 = ("<<vertices.at(temp->parent)->label<<", "<<vertices.at(temp->child)->label<<")"<<flush<<endl;}
						}
					}
				}
			}
			else
			{
				continue;
			}
		}
	}
}

 void Dilute::addWaste(DagGen * dag)
{
	bool Debug = false;
	vector<Edge*> edges = dag->Edges();
	int wasteCount = 1;
	for(unsigned i = 0; i < vertices.size(); i++)
	{
		if(vertices.at(i)->type == SPLIT)
		{
			int numChild = 0;

			//find outgoing edges(children)
			for(unsigned j = 0; j < edges.size(); j++)
			{
				if(edges.at(j)->parent == vertices.at(i)->uniqueID)
				{
					numChild++;
				}
			}

			if(numChild == 0)
			{
				if(Debug){cout<<"whoopsy there shouldn't be vertices without outgoing edges"<<flush<<endl;}
			}
			else if(numChild == 1)
			{
				if(Debug){cout<<"adding waste to split vertex: "<<vertices.at(i)->uniqueID<<flush<<endl;}
				string label = "waste";
				ostringstream count;
				count << wasteCount;
				label = label + count.str();
				Vertex2* waste = addVertex(dag, OUTPUT, label, NULL, NULL, 1, 1);
				wasteCount++;
				dag->addEdge(vertices.at(i)->uniqueID, waste->uniqueID);
			}
			else //numchild == 2
			{
				if(Debug){cout<<"the number of children should be two, actual size: "<<numChild<<flush<<endl;}
			}
		}
	}
	return;
}
