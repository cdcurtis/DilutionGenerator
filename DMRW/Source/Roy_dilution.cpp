/*
 * Roy_dilution.cc
 *
 *  Created on: Apr 23, 2013
 *      Author: Kenneth O'Neal
 */
#include "../Headers/Roy_dilution.h"
#include "../../DagGen/Headers/DagGen.h"

using namespace std;

DiluteRet* RoyDilute::PerformDilution(DiluteDroplet* di,DiluteDroplet* db, double tolerance, double DesiredConcentrate, int num_ops)
{
	bool Debug = false;
	bool Debug2 = false;
	bool Debug3 = false;

	if(Debug){cout << "desired concentration is:" << DesiredConcentrate << endl;}
	if(Debug3)
	{
		cout<<" the input droplets are di: "<<di->uID<<", ";
		di->Concentration.display();
		cout<<"and db: "<<db->uID<<", ";
		db->Concentration.display();
	}
	if(Debug3){cout<<"PD0.0"<<flush<<endl;}

	int id = 0; //id represents the value assigned as nodeid. Iterate after each dilutedroplet creation
	vector<MixOp*> Mix_Splits;
	int denominator = pow(2, num_ops);
	int num_MS = 0;

	vector<DiluteDroplet*> AvailDroplets;

	if(Debug3){cout<<"PD1.0"<<flush<<endl;}
	Rational buffConc = Rational(0, denominator);
	Rational SampleConc = Rational(denominator, denominator);

	//di->Concentration = SampleConc; //2^n/2^n (sample)
	//di->Concentration.display();
	id++;
	di->uID = id;
	di->count = 1;
	di->DesiredConcentrate = Rational::ConctoRat(DesiredConcentrate, num_ops);

	//db->Concentration = buffConc; //0/2^n initially (buffer)
	//db->Concentration.display();
	id++;
	db->uID = id;
	db->count = 1;
	db->DesiredConcentrate = Rational::ConctoRat(DesiredConcentrate, num_ops);

	if(Debug3){cout<<"PD2.0"<<flush<<endl;}
	AvailDroplets.push_back(db);
	AvailDroplets.push_back(di);
	if(Debug3){cout<<"-----pd2 droplets pushed db: "<<db->uID<<", di: "<<di->uID<<", total size is: "<<AvailDroplets.size()<<flush<<endl;}
	//Step2

	Rational CtConc = Rational::ConctoRat(DesiredConcentrate, num_ops); //target concentration

	//Step3
	DiluteDroplet* MixCl = db; //initialize to target concentration
	DiluteDroplet* MixCh = di;


	//These values represent the two boundary Cfs, which appear as the highest node on left on and
	//highest node on right arm of digraph.
	for (unsigned i = 0; i < AvailDroplets.size(); i++) {
		if (AvailDroplets.at(i)->Concentration.numer
				< CtConc.numer) //biggest smaller than ct
		{
			MixCl = AvailDroplets.at(i);

		} else if (AvailDroplets.at(i)->Concentration.numer
				> CtConc.numer) //smaller biggest than ct
		{
			MixCh = AvailDroplets.at(i);
		} else {
			//do nothing, they will be set to desired Ct. shouldn't happen
		}
	}

	if(Debug3){cout<<"PD3.0"<<flush<<endl;}
	//Step4
	Rational MixConc = CalcMixConc(MixCl, MixCh);
	long double X = Rational::RattoFloat(MixConc); //resultant concentration of mix operation
	int x = MixConc.numer; //resulant numerator of mix operation

	//creating droplets resulting from mix operation (Splitting droplets)
	DiluteDroplet* Mix1 = new DiluteDroplet;
	id++;
	Mix1->uID = id;
	Mix1->count = 0;

	DiluteDroplet* Mix2 = new DiluteDroplet;
	id++;
	Mix2->uID = id;
	Mix2->count = 0;
	Mix1->Concentration = MixConc;
	Mix2->Concentration = MixConc;

	//Pushing this mix/split operation onto the list of mix and split operations
	MixOp* InitMix = new MixOp;
	InitMix->DropletsPreMix = make_pair(MixCh, MixCl);
	InitMix->DropletsPostMix = make_pair(Mix1, Mix2);
	num_MS++;
	Mix_Splits.push_back(InitMix);

	Mix1->Mixes = InitMix;
	Mix2->Mixes = InitMix;
	Mix1->Mix_Splits = Mix_Splits; //potentially needs to be changed to push back
	Mix2->Mix_Splits = Mix_Splits; //in order to capture all mixing operations for the new drop

	AvailDroplets.push_back(Mix1);
	AvailDroplets.push_back(Mix2);
	if(Debug3){cout<<"----- first mix droplets pushed Mix1: "<<Mix1->uID<<", Mix2: "<<Mix2->uID<<", total size is: "<<AvailDroplets.size()<<flush<<endl;}
	//AvailDroplets.push_back(Mix2);

	//Step5->Step10
	long double error = fabs(X - DesiredConcentrate);
	int NumSteps = 0;

	int Lnumer = db->Concentration.numer; //
	int Rnumer = di->Concentration.numer;

	if(Debug){cout<<"LNumer = "<<Lnumer<<", RNumer = "<<Rnumer<<flush<<endl;}

	DiluteDroplet* Left = db; //left is buffer dispense droplet
	DiluteDroplet* Right = di; //right is solution dispense droplet

	bool LFlag = false;
	bool RFlag = false;
	//return Mix_Splits;

	if(Debug3){cout<<"PD4.0"<<flush<<endl;}
	while (error >= tolerance)
	{
		//x is resulting numerator of mix action
		if (x < CtConc.numer) //CtConc represents the target concentration. x is numerator of result of the two
			//most recently mixed droplets.
			//L should be equal to a droplet with the
			//largest concentration smaller than Ct's concentration.
		{
			if(Debug3){cout<<"PD4.1"<<flush<<endl;}
			Lnumer = x;
			LFlag = true;
			RFlag = false;
			bool found = false;
			for(unsigned i = 0; i < AvailDroplets.size(); i++)
			{
				if(AvailDroplets.at(i)->Concentration.numer == x and (AvailDroplets.at(i)->uID%2 != 0))
				{
					Left = AvailDroplets.at(i);
					found = true;
				}
			}
			if(found == false)
			{
				cout<<"what? I should have found this LEFT"<<flush<<endl;
			}
		} //end if
		else if (x > CtConc.numer)
			//R should be equal to a droplet with the
			//smallest concentration larger than Ct's concentration.
		{
			if(Debug3){cout<<"PD4.2"<<flush<<endl;}
			Rnumer = x;
			RFlag = true;
			LFlag = false;
			bool found = false;

			for(unsigned i = 0; i < AvailDroplets.size(); i++)
			{
				if(AvailDroplets.at(i)->Concentration.numer == x and (AvailDroplets.at(i)->uID%2 != 0))
				{
					Right = AvailDroplets.at(i);
					found = true;
				}
			}
			if(found == false)
			{
				cout<<"what? I should have found this RIGHT"<<flush<<endl;
			}
		} //end else if

		//Step 11
		//"mixing droplets"
		if(Debug3){cout<<"PD4.3"<<flush<<endl;}
		Left->count++;
		Right->count++;

		Rational MixConc = CalcMixConc(Left, Right);
		X = Rational::RattoFloat(MixConc);
		x = MixConc.numer;

		if(Debug3){cout<<"PD4.3.1"<<flush<<endl;}

		DiluteDroplet* Mix1 = new DiluteDroplet;
		id++;
		Mix1->uID = id;
		Mix1->count = 0;

		DiluteDroplet* Mix2 = new DiluteDroplet;
		id++;
		Mix2->uID = id;
		Mix2->count = 0;
		if(Debug3)
		{
			cout<<"Mix = "<<flush<<endl;
			MixConc.display();
		}

		Mix1->Concentration = MixConc;
		Mix2->Concentration = MixConc;

		//if(Debug3){cout<<"PD4.3.2"<<flush<<endl;}
		//if(Debug3){cout<<"Mix1->Concentration is: "<<flush<<endl;}
		//if(Debug3){Mix1->Concentration.display();}
		//if(Debug3){cout<<"Mix2->Concentration is: "<<flush<<endl;}
		//if(Debug3){Mix2->Concentration.display();}
		//if(Debug3){cout<<"Left->Concentration is: "<<flush<<endl;}
		//if(Debug3){Left->Concentration.display();}
		//if(Debug3){cout<<"Right->Concentration is: "<<flush<<endl;}
		//if(Debug3){Right->Concentration.display();}

		MixOp* InitMix = new MixOp;
		InitMix->DropletsPreMix = make_pair(Right, Left);
		InitMix->DropletsPostMix = make_pair(Mix1, Mix2);\

		if(Debug3)
		{
			cout<<"drop pre Right, Drop Pre left"<<flush<<endl;
			Right->Concentration.display();
			Left->Concentration.display();
			cout<<"drop post M1, Drop Pre M2"<<flush<<endl;
			Mix1->Concentration.display();
			Mix2->Concentration.display();
		}


		Mix_Splits.push_back(InitMix);
		num_MS++;
		AvailDroplets.push_back(Mix1);
		AvailDroplets.push_back(Mix2);
		if(Debug3){cout<<"-----second Mix droplets pushed Mix1: "<<Mix1->uID<<". Mix2: "<<Mix2->uID<<", total size is: "<<AvailDroplets.size()<<flush<<endl;}
		Mix1->Mixes = InitMix;
		Mix2->Mixes = InitMix;
		Mix1->Mix_Splits = Mix_Splits;
		Mix2->Mix_Splits = Mix_Splits;
		if(Debug3){cout<<"PD4.4"<<flush<<endl;}

		NumSteps = NumSteps + 1;

		//Step 12
		error = fabs(X - DesiredConcentrate);

	} //end while

	if(Debug3){cout<<"PD5.0"<<flush<<endl;}

	if(Debug){cout << "finished while, tolerance is: " << tolerance << endl;}
	if(Debug){cout << "The final Mix Concentration id is: " << AvailDroplets.back()->uID<< ", conc is: ";}
	if(Debug){cout << Rational::RattoFloat(AvailDroplets.back()->Concentration) << endl;}
	if(Debug){cout << "The Desired mix Concentration is:" << DesiredConcentrate << endl;}
	if(Debug){cout << "Number of mix_Split steps is: " << num_MS << endl;}

	if(Debug3){cout<<"the final size of AvailDroplets is: "<<AvailDroplets.size()<<flush<<endl;}
	if(Debug2){outputMixSplits(Mix_Splits);}
	if(true){outputAvailableDroplets(AvailDroplets);}

	pair<vector<MixOp*>, vector<DiluteDroplet*> > MD = make_pair(Mix_Splits, AvailDroplets);
	DiluteRet* ret = new DiluteRet;
	ret->DilutionVals = MD;
	vector<double> endConc;
	endConc.push_back(Rational::RattoFloat(AvailDroplets.back()->Concentration));
	ret->endConcentration = endConc;
	cout<<"End concentration is: "<<ret->endConcentration[0]<<flush<<endl;
	int base = AvailDroplets.back()->Concentration.denom;
	ret->base = base;
	return ret;
} // end perform dilution

pair<VertexCounts*, DagGen*> RoyDilute::CreateDag_IDMAHelper(DagGen * dag, pair< vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals, DagGen* M)
{
	bool Debug = false;
	bool Debug2 = false;
	vector<MixOp*> Mixes = DilutionVals.first;
	vector<DiluteDroplet*> AvailableDroplets = DilutionVals.second;

	std::sort(AvailableDroplets.begin(), AvailableDroplets.end(), Dilute::CompareuID);

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
		if(Debug) { cout<<"start while"<<flush<<endl;}
		DiluteDroplet* cur = myQueue.front();
		myQueue.pop();

		//establish incoming mix droplets pre1
		Vertex2* pre1;
		Vertex2* pre2;

		if(Rational::RattoFloat(cur->Mixes->DropletsPreMix.first->Concentration) == 1) //is sample dispense
		{
			if(Debug){cout<<"pre1 if"<<flush<<endl; }
			ostringstream convert1;
			convert1 << sampleCount;
			string label1 = sample + convert1.str();

			pre1 = this->addVertex(dag, DISPENSE, label1, NULL, NULL, 1, 1);
			pre1->type = DISPENSE;
			pre1->dh = NULL; //added for dh check in expanddag
			pre1->dl = NULL; //added for dh check in expanddag
			sampleCount++;
		}
		else if(Rational::RattoFloat(cur->Mixes->DropletsPreMix.first->Concentration) == 0)
		{
			if(Debug){cout<<"pre1 else if"<<flush<<endl; }
			ostringstream convert2;
			convert2 << bufferCount;
			string label2 = buffer + convert2.str();
			pre1 = this->addVertex(dag, DISPENSE, label2, NULL, NULL, 1, 1);
			pre1->type = DISPENSE;
			pre1->dh = NULL; //added for dh check in expanddag
			pre1->dl = NULL; //added for dh check in expanddag
			bufferCount++;
		}
		else
		{
			for(unsigned v = 0; v < this->vertices.size(); v++) //check all vertices
			{
				if(vertices.at(v)->label.find(split) != std::string::npos) //check if it's a split (outgoing)
				{
					if(vertices.at(v)->dh == cur->Mixes->DropletsPreMix.first)// or vertices.at(v)->dh == cur->Mixes->DropletsPreMix.second) //if the non wastedroplet is the current one
					{
						//do not create new vertex, just connect to current mix vertex.
						if(Debug){cout<<"pre1 else"<<flush<<endl; }
						pre1 = vertices.at(v);
					}
				}
			}
		}

		//establish incoming mix droplets pre2
		if(Rational::RattoFloat(cur->Mixes->DropletsPreMix.second->Concentration) == 1) //is sample dispense
		{
			if(Debug){cout<<"pre2 if"<<flush<<endl; }
			ostringstream convert1;
			convert1 << sampleCount;
			string label1 = sample + convert1.str();
			pre2 = this->addVertex(dag, DISPENSE, label1, NULL, NULL, 1, 1);
			pre2->type = DISPENSE;
			pre2->dh = NULL; //added for dh check in expanddag
			pre2->dl = NULL; //added for dh check in expanddag
			sampleCount++;
		}
		else if(Rational::RattoFloat(cur->Mixes->DropletsPreMix.second->Concentration) == 0)
		{
			if(Debug){cout<<"pre2 else if"<<flush<<endl; }
			ostringstream convert2;
			convert2 << bufferCount;
			string label2 = buffer + convert2.str();
			pre2 = this->addVertex(dag, DISPENSE, label2, NULL, NULL, 1, 1);
			pre2->type = DISPENSE;
			pre2->dh = NULL; //added for dh check in expanddag
			pre2->dl = NULL; //added for dh check in expanddag
			bufferCount++;
		}
		else
		{
			for(unsigned v = 0; v < this->vertices.size(); v++) //check all vertices
			{
				if(vertices.at(v)->label.find(split) != std::string::npos) //check if it's a split (outgoing)
				{
					if(vertices.at(v)->dh == cur->Mixes->DropletsPreMix.second) //if the non wastedroplet is the current one
					{
						if(Debug){cout<<"pre2 else"<<flush<<endl; }
						//do not create new vertex, just connect to current mix vertex.
						pre2 = vertices.at(v);
					}
				}
			}
		}

		//create Mix vertex
		ostringstream convertMix;
		convertMix << mixCount;

		string mixLabel = mix + convertMix.str();
		Vertex2* mix = this->addVertex(dag, MIX, mixLabel, NULL, NULL, 1, 1);

		mix->type = MIX;
		mixCount++;

		dag->addEdge(pre1, mix);
		dag->addEdge(pre2, mix);
		if(Debug)
		{
			cout<<"edge added1.1: "<<pre1->uniqueID<<", "<<mix->uniqueID<<flush<<endl;
			cout<<"edge added1.2: "<<pre2->uniqueID<<", "<<mix->uniqueID<<flush<<endl;
		}

		if(Debug){ cout<<"mix added"<<flush<<endl;}

		mix->dh = cur->Mixes->DropletsPreMix.first;
		mix->dl = cur->Mixes->DropletsPreMix.second;

		//create split vertex, connect mix to split, and populate dh, dl (outgoing drops of split)
		ostringstream convertSplit;
		convertSplit << splitCount;
		string splitLabel = split + convertSplit.str();
		Vertex2* split = this->addVertex(dag, SPLIT, splitLabel, NULL, NULL, 1, 1);
		split->type = SPLIT;
		splitCount++;

		dag->addEdge(mix, split);
		if(Debug)
		{
			cout<<"edge added2.1: "<<mix->uniqueID<<", "<<split->uniqueID<<flush<<endl;
			//cout<<"edge added1.2: "<<pre2->uniqueID<<", "<<mix->uniqueID<<flush<<endl;
		}
		if(Debug){cout<<"split added"<<flush<<endl; }

		split->dh = cur->Mixes->DropletsPostMix.first;
		split->dl = cur->Mixes->DropletsPostMix.second;

		if(Debug){cout<<"the post split droplets are: ("<<split->dh->uID<<","<<split->dh->count<<"),  and ("<<split->dl->uID<<","<<split->dl->count<<")"<<flush<<endl;}
		if(Debug){cout<<"the current droplet is:  ("<<cur->uID<<","<<cur->count<<")"<<flush<<endl;}
		if(Debug){cout<<"the pre cur droplets are: ("<<cur->Mixes->DropletsPreMix.first->uID<<","<<cur->Mixes->DropletsPreMix.first->count<<") and ("<<cur->Mixes->DropletsPreMix.second->uID<<","<<cur->Mixes->DropletsPreMix.second->count<<")"<<flush<<endl;}
		if(Debug){cout<<"the post cur droplet is: ("<<cur->Mixes->DropletsPostMix.first->uID<<","<<cur->Mixes->DropletsPostMix.first->count<<") and ("<<cur->Mixes->DropletsPostMix.second->uID<<","<<cur->Mixes->DropletsPostMix.second->count<<")"<<flush<<endl;}


		//ensures that in the case of a parent being the subsequent child
		//the parent, nor the child will be added more than once.
		//each vertex is added and explored only once, avoiding circular dependencies.
		for(unsigned p = 0; p < Mixes.size(); p++)
		{

			if(Mixes.at(p)->DropletsPreMix.first == cur)
			{
				bool pushed = false;
				for(unsigned t = 0; t < prevPushed.size(); t++) //ensure pushed only once
				{
					if(Mixes.at(p)->DropletsPostMix.first == prevPushed.at(t))
					{
						pushed = true;
					}
				}
				if(pushed == false)
				{
					if(Debug){cout<<"pushed first"<<flush<<endl;}
					myQueue.push(Mixes.at(p)->DropletsPostMix.first); //do not push second, always waste
					prevPushed.push_back(Mixes.at(p)->DropletsPostMix.first);
				}
			}
			if(Mixes.at(p)->DropletsPreMix.second == cur)
			{
				bool pushed = false;
				for(unsigned t = 0; t < prevPushed.size(); t++)
				{
					if(Mixes.at(p)->DropletsPostMix.first == prevPushed.at(t))
					{
						pushed = true;
					}
				}
				if(pushed == false)
				{
					if(Debug){cout<<"pushed second"<<flush<<endl; }
					myQueue.push(Mixes.at(p)->DropletsPostMix.first); //do not push second, always waste
					prevPushed.push_back(Mixes.at(p)->DropletsPostMix.first);
				}
			}
		}
	}

	if(Debug){cout<<"add output"<<flush<<endl; }
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
	Vertex2 * out = this->addVertex(dag, OUTPUT, outLabel, NULL, NULL, 1, 1);

	//this->addEdge(fsplitIndex, out->uniqueID);
	dag->addEdge(fsplitIndex, out->uniqueID);
	if(Debug)
	{
		cout<<"edge added3.1: "<<fsplitIndex<<", "<<out->uniqueID<<flush<<endl;
		//cout<<"edge added1.2: "<<pre2->uniqueID<<", "<<mix->uniqueID<<flush<<endl;
	}
	out->dh = NULL;
	out->dl = NULL;

	if(Debug){dag->outputVertices();}
	if(Debug){dag->outputEdges();}

	VertexCounts* VC = new VertexCounts;
	VC->bufferCount = bufferCount;
	VC->mixCount = mixCount;
	VC->sampleCount = sampleCount;
	VC->splitCount = splitCount;
	VC->wasteCount = wasteCount;

	if(Debug2){cout<<"temp outputVertices = "<<flush<<endl;}

	M = (new DagGen(*dag));

	if(Debug2){M->outputVertices();}

	pair<VertexCounts*, DagGen*> ret = make_pair(VC, M);
	return ret;
}

DagGen* RoyDilute::CreateIDMADag(VertexCounts* VC, DagGen* M)
{
	bool Debug = false;
	bool Debug2 = false;

	//modifies counter values and dag output from CreateDag in order to have one sample, and one buffer to match IDMA alg. requirements
	VertexCounts VC2;
	VC2.sampleCount = VC->sampleCount;
	VC2.bufferCount = VC->bufferCount;
	VC2.wasteCount = VC->wasteCount;
	VC2.mixCount = VC->mixCount;
	VC2.splitCount = VC->splitCount;

	if(Debug){cout<<"before copy dag"<<flush<<endl;}
	//DagGen dag = VC->Intermediate;
	if(Debug2){M->outputVertices();}
	vector<Vertex*> vertices = M->Vertices();
	vector<Vertex*> samples;
	vector<Vertex*> buffers;
	vector<Edge*> parentEdges;
	vector<Edge*> edges = M->Edges();
	string s = "sample";
	string b = "buffer";
	Vertex* Sample1;
	Vertex* Buffer1;

	if(Debug){cout<<"before for loop"<<flush<<endl;}

	for(unsigned i = 0; i < vertices.size(); i++)
	{
		Vertex* vertex = vertices.at(i);
		if(vertex->type == DISPENSE)
		{
			if(Debug){cout<<"1.0"<<flush<<endl;}
			if(vertex->label.find(s) != std::string::npos) //if sample
			{
				if(Debug){cout<<"1.1"<<flush<<endl;}
				if(vertex->label == "sample1")
				{
					if(Debug){cout<<"1.2"<<flush<<endl;}
					Sample1 = vertex;
					continue; //is first sample, do nothing
				}
				else
				{
					if(Debug){cout<<"1.3"<<flush<<endl;}

					//is an additional sample, remove vertex, connect all outgoing edges' to first sample.
					parentEdges = M->findOutgoingEdges(vertex->uniqueID);

					if(Debug){cout<<"1.4"<<flush<<endl;}
					for(unsigned j = 0; j < parentEdges.size(); j++)
					{
						if(Debug){cout<<"edge being checked sample is: ("<<parentEdges.at(j)->parent<<", "<<parentEdges.at(j)->child<<")"<<flush<<endl;}
						if(parentEdges.at(j)->parent == vertex->uniqueID)
						{
							if(Debug){cout<<"edge added is: ("<<Sample1->uniqueID<<", "<<parentEdges.at(j)->child<<")"<<flush<<endl;}
							M->addEdge(Sample1->uniqueID, parentEdges.at(j)->child);
							edges = M->Edges();
						}
					}
					if(Debug){cout<<"1.5"<<flush<<endl;}
					for(unsigned j = 0; j < parentEdges.size(); j++) //find all original parent edges and remove
					{
						for(unsigned k = 0; k < edges.size(); k++)
						{
							if(parentEdges.at(j)->parent == edges.at(k)->parent and parentEdges.at(j)->child == edges.at(k)->child)
							{
								//is match, remove edge from edges
								if(Debug){cout<<"edge removed is: "<<edges.at(k)->parent<<", "<<edges.at(k)->child<<flush<<endl;}
								M->removeEdge(k);//erase(edges.begin()+k, edges.begin()+k+1);
								if(Debug){cout<<"resulting edge is: "<<edges.at(k)->parent<<", "<<edges.at(k)->child<<flush<<endl;}
								edges = M->Edges();
								//k--;
							}

						}
					}
					if(Debug){cout<<"1.6"<<flush<<endl;}
					VC2.sampleCount = VC2.sampleCount -1;//decrement sample counter
					if(Debug){cout<<"1.7"<<flush<<endl;}
					//remove additional sample node
					if(Debug){cout<<"sample erased = "<<vertex->label<<", "<<vertex->uniqueID<<flush<<endl;}
					M->removeVertex(vertex->uniqueID);//vertices.erase(vertices.begin()+vertex->uniqueID, vertices.begin()+vertex->uniqueID+1);
					vertices = M->Vertices();
					//i--;
					if(Debug){cout<<"1.8"<<flush<<endl;}
				}
				if(Debug){cout<<"1.9"<<flush<<endl;}

			}
			else if(vertex->label.find(b) != std::string::npos)
			{
				if(Debug){cout<<"2.1"<<flush<<endl;}
				if(vertex->label == "buffer1")
				{
					if(Debug){cout<<"2.2"<<flush<<endl;}
					Buffer1 = vertex;
					continue; //is first buffer, do nothing.
				}
				else
				{
					if(Debug){cout<<"2.3"<<flush<<endl;}
					//is an additional sample, remove vertex, connect all outgoing edges' to first sample.
					parentEdges = M->findOutgoingEdges(vertex->uniqueID);
					if(Debug){cout<<"2.4"<<flush<<endl;}
					for(unsigned j = 0; j < parentEdges.size(); j++)
					{
						if(Debug){cout<<"edge being check buffer is: ("<<parentEdges.at(j)->parent<<", "<<parentEdges.at(j)->child<<")"<<flush<<endl;}
						if(parentEdges.at(j)->parent == vertex->uniqueID)
						{
							if(Debug){cout<<"edge added is: ("<<Buffer1->uniqueID<<", "<<parentEdges.at(j)->child<<")"<<flush<<endl;}
							M->addEdge(Buffer1->uniqueID, parentEdges.at(j)->child);
							edges = M->Edges();
						}
					}
					if(Debug){cout<<"2.5"<<flush<<endl;}
					for(unsigned j = 0; j < parentEdges.size(); j++) //find all original parent edges and remove
					{
						for(unsigned k = 0; k < edges.size(); k++)
						{
							if(parentEdges.at(j)->parent == edges.at(k)->parent and parentEdges.at(j)->child == edges.at(k)->child)
							{
								//is match, remove edge from edges
								if(Debug){cout<<"edge removed is: "<<edges.at(k)->parent<<", "<<edges.at(k)->child<<flush<<endl;}
								M->removeEdge(k);
								if(Debug){cout<<"resulting edge is: "<<edges.at(k)->parent<<", "<<edges.at(k)->child<<flush<<endl;}
								edges = M->Edges();
							}
						}
					}
					if(Debug){cout<<"2.6"<<flush<<endl;}
					VC2.sampleCount = VC2.sampleCount -1;//decrement sample counter
					//remove additional sample node
					if(Debug){cout<<"2.7"<<flush<<endl;}
					if(Debug){cout<<"buffer erased = "<<vertex->label<<", "<<vertex->uniqueID<<flush<<endl;}
					if(Debug){cout<<"buffer erased = "<<vertex->label<<", "<<vertex->uniqueID<<flush<<endl;}
					M->removeVertex(vertex->uniqueID);
					vertices = M->Vertices();
					if(Debug){cout<<"2.8"<<flush<<endl;}
				}
				if(Debug){cout<<"2.9"<<flush<<endl;}
			}
			else
			{
				if(Debug){cout<<"3.0"<<flush<<endl;}
				//did not find buffer or sample, but type was dispense. This was an error
				continue;
			}
		}
		else
		{
			continue;
			//not a DISPENSE, don't care
		}
		if(Debug){cout<<"4.0"<<flush<<endl;}
	}

	if(Debug){cout<<"5.0"<<flush<<endl;}
	if(Debug){cout<<"intermediate post mod"<<flush<<endl;}
	if(Debug2){M->outputVertices();}
	if(Debug2){M->outputEdges();}
	if(Debug){cout<<"5.1"<<flush<<endl;}

	return M;
}

pair< int, int > RoyDilute::RoyDilute_Process(DiluteDroplet* db, DiluteDroplet* di, int num_ops, double DesiredConcentrate, double tolerance, DagGen* dag)
{
	bool Debug = false;
	RoyDilute RD;

	int numMS = 0;
	int numW = 0;

	pair<vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals;
	DiluteRet* valid;

	//cout<<"The Desired concentrate is: "<<DesiredConcentrate<<flush<<endl;

	if(Debug){cout<<" /////////Before PerformDilution\\\\\\\\\\ "<<flush<<endl;}
	valid = RD.PerformDilution(di,db, tolerance, DesiredConcentrate, num_ops);
	DilutionVals = valid->DilutionVals;
	if(Debug){cout<<" /////////after performdilution\\\\\\\\\\ "<<flush<<endl;}

	if(Debug){cout<<" /////////before create dag\\\\\\\\\\"<<flush<<endl;}
	VertexCounts* VC = RD.CreateDagRoy(dag, DilutionVals);
	if(Debug){cout<<"/////////after create dag\\\\\\\\\\"<<flush<<endl;}


	if(Debug){cout<<" /////////before generate DropletDag\\\\\\\\\\ "<<flush<<endl;}
	if(Debug){dag->generateDropletDag("DMRWafterCreateT.cpp");}
	if(Debug){cout<<" /////////after generate DropletDag\\\\\\\\\\ "<<flush<<endl;}

	if(Debug){cout<<" /////////before generate DotyGraph\\\\\\\\\\ "<<flush<<endl;}
	if(Debug){dag->generateDotyGraph("DMRWafterCreate.dot");}
	if(Debug){cout<<" /////////after generate DotyGrpah\\\\\\\\\\ "<<flush<<endl;}

	if(Debug){cout<<"/////////before dag expander\\\\\\\\\\"<<flush<<endl;}
	RD.expander(dag, VC);
	if(Debug){cout<<"/////////after dag expander\\\\\\\\\\"<<flush<<endl;}

	if(Debug){cout<<" /////////before generate DropletDag\\\\\\\\\\ "<<flush<<endl;}
//	dag->generateDropletDag("DMRWafterExpandT.cpp");
	if(Debug){cout<<" /////////after generate DropletDag\\\\\\\\\\ "<<flush<<endl;}

	if(Debug){cout<<" /////////before generate DotyGraph\\\\\\\\\\ "<<flush<<endl;}
	//dag->generateDotyGraph("DMRWafterExpand.dot");
	if(Debug){cout<<" /////////after generate DotyGrpah\\\\\\\\\\ "<<flush<<endl;}

	numW = dag->calcNumWaste();
	numMS = dag->calcNumMixSplits();

	pair<int, int> ret = make_pair(numW, numMS);

if(Debug){
	if(Debug){cout<<" /////////before validation testing\\\\\\\\\ "<<flush<<endl;}

	int base = valid->base;
	vector<double> endConcentration = valid->endConcentration;
	bool value = dag->isValidSingleReactantDilution(endConcentration, base);
	if(Debug){cout<<" //////////end validation testing\\\\\\\\\ "<<flush<<endl;}

	if(value)
	{
		cout<<"Validity test passed"<<flush<<endl;
	}
	else
	{
		cout<<"validity test not passed"<<flush<<endl;
	}
}
	//VC->H->~DagGen();
	return ret;
}

DiluteRet* RoyDilute::RoyDilute_IDMA(DiluteDroplet * L, DiluteDroplet* R, double tol, double Ct, int n)
{

	bool Debug = true;
	RoyDilute RD;
	DagGen dag;

	DiluteDroplet* db = L;
	DiluteDroplet* di = R;
	int num_ops = n;


	double tolerance = tol; //1/(2^11) using 1/(2^10) terminates in 3 steps due to lack of precision
	double DesiredConcentrate = Ct; //(Roy Figure 7(a): 127/1024)

	//GenerateDigraph(Cl, Ch, precision, DesiredConcentrate);
	pair<vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals;
	DiluteRet* valid = new DiluteRet;

	if(Debug){cout<<" /////////RoyDilute_IDMA::Before PerformDilution\\\\\\\\\\ "<<flush<<endl;}
	valid = RD.PerformDilution(di,db, tolerance, DesiredConcentrate, num_ops);
	//DilutionVals = valid->DilutionVals;
	if(Debug){cout<<" /////////after performdilution\\\\\\\\\\ "<<flush<<endl;}

	return valid;
}

DiluteRet* RoyDilute::populateIDMA_M(double tol, double DesiredConc, DagGen* M, int num_ops)
{
	RoyDilute RD;
	DagGen dag;

	bool Debug1 = false;
	bool Debug2 = false;
	bool Debug3 = false;

	DiluteDroplet* db = new DiluteDroplet;
	db->Concentration = Rational(0,pow(2,num_ops));
	DiluteDroplet* di = new DiluteDroplet;
	di->Concentration = Rational(pow(2,num_ops), pow(2,num_ops));

	double tolerance = tol;
	double DesiredConcentrate = DesiredConc;
	if(Debug1){cout<<"values passed are; tol:"<<tolerance<<", DC: "<<DesiredConcentrate<<flush<<endl;}
	//GenerateDigraph(Cl, Ch, precision, DesiredConcentrate);
	pair<vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals;
	DiluteRet* valid = new DiluteRet;

	if(Debug3){cout<<" /////////populateIDMA_M::Before PerformDilution\\\\\\\\\\ "<<flush<<endl;}
	valid = RD.PerformDilution(di,db, tolerance, DesiredConcentrate, num_ops);
	DilutionVals = valid->DilutionVals;
	if(Debug3){cout<<" /////////populateIDMA_M::after performdilution\\\\\\\\\\ "<<flush<<endl;}

	if(Debug3){cout<<" /////////populateIDMA_M::before create dag\\\\\\\\\\"<<flush<<endl;}

	return valid;
}


void RoyDilute:: RunDMRW(int argc, char** argv, DagGen* dag)
{
	/*<NumOps>, <tolerance> <DesiredConcentration> */
	if(argc<4){
		cerr<<"In correct Input: <NumOps>, <tolerance> <DesiredConcentration>" << endl;
		std::exit(-1);
	}

	int num_ops = atoi(argv[1]);
	DiluteDroplet* db = new DiluteDroplet;
	db->Concentration = Rational(0,pow(2,num_ops));
	DiluteDroplet* di = new DiluteDroplet;
	di->Concentration = Rational(pow(2,num_ops),pow(2,num_ops));

	double tolerance = atof(argv[2]);
	double DesiredConcentrate = atof(argv[3]);
	RoyDilute::RoyDilute_Process(db, di, num_ops, DesiredConcentrate, tolerance, dag);
}

