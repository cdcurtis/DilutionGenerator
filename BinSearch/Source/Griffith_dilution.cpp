/*
 * Griffith_dilution.cc
 *
 *  Created on: Apr 19, 2013
 *      Author: Kenneth O'Neal
 */
#include "../Headers/Griffith_dilution.h"
#include <cstdlib>

Vertex2* GriffDilute::addVertex(DagGen & dag, VertexType vertexType, std:: string label, DiluteDroplet* dh, DiluteDroplet* dl, int dropsAvail, int dropsNeeded)
{
	int IDs = dag.getID();
	Vertex2* v =  new Vertex2(vertexType,label,IDs++, dl, dh, dropsAvail, dropsNeeded);
	dag.addVertex(vertexType, label);
	vertices.push_back(v);
	return v;
}

void GriffDilute::outputVertices()
{
	for(unsigned i = 0; i < vertices.size(); i++)
	{
		cout<<"V"<<i<<": "<<vertices.at(i)->label<<", "<<vertices.at(i)->uniqueID<<flush<<endl;
	}
	return;
}

void GriffDilute::outputEdges(DagGen & dag)
{
	vector<Edge*> edges = dag.Edges();
	for(unsigned i = 0; i < edges.size(); i++)
	{
		cout<<"edge "<<i<<": parent="<<edges.at(i)->parent<<", child = "<<edges.at(i)->child<<endl;
	}
	return;
}

///////////Begin GriffDilute functions
Rational GriffDilute::FindRange(DiluteDroplet* input, DiluteDroplet* buffer) {
	return (input->Concentration.subtract(buffer->Concentration));
}

//returns the concentrate of a Dilute Droplet
Rational GriffDilute::GetConcentrate(DiluteDroplet* node)
{
	return node->Concentration;
}

//sets the concentrate of a dilute droplet (to be used after mixing)
void GriffDilute::SetConcentrate(DiluteDroplet* node, double ConcNum, double ConcDenom)
{
	node->Concentration.numer = ConcNum;
	node->Concentration.denom = ConcDenom;
}

//Gets the desire concentrate of a droplet
Rational GriffDilute::GetDesiredConcentrate(DiluteDroplet* node) //returns c from variable list
{
	return node->DesiredConcentrate;
}

//Gets the tolerance allowed for a droplet
Rational GriffDilute::GetTolerance(DiluteDroplet* node) {
	return node->Tolerance;
}

void GriffDilute::DeleteM(vector<MixOp*> M) {
	for (unsigned i = 0; i < M.size(); i++) {
		delete M.at(i)->DropletsPostMix.first;
		delete M.at(i)->DropletsPostMix.second;
		delete M.at(i)->DropletsPreMix.first;
		delete M.at(i)->DropletsPreMix.second;
		delete M.at(i);
	}
}

void GriffDilute::DeleteD(vector<pair<DiluteDroplet*, DiluteDroplet*> > D) {
	for (unsigned i = 0; i < D.size(); i++) {
		delete D.at(i).first;
		delete D.at(i).second;
	}
	D.clear();
}

Rational GriffDilute::FloatToRat(double value, int num_ops) {
	double tolerance = 0.000001;
	double check_val = 0.00;
	double check_denom = 0.00;
	int top = 0;
	int bot;
	while ((fabs(check_val - value)) > tolerance) {
		bot = 0;
		check_denom = value + 1;
		top++;
		while (check_denom > value) {
			bot++;
			check_denom = (static_cast<double>(top) / static_cast<double>(bot));
		}

		check_val = check_denom;
	}
	Rational temp = Rational(top, bot);
	return temp;
}
Rational GriffDilute::ConctoRat(double Concentration, int num_ops) {
	int denom = pow(2, num_ops);
	int numer = 0;
	long double value = (static_cast<long double>(numer)
			/ static_cast<long double>(denom));

	while (value < Concentration) {
		numer = numer + 1;
		value = (static_cast<long double>(numer)
				/ static_cast<long double>(denom));
	}
	Rational RatConc = Rational(numer, denom);
	return RatConc;
}

Rational GriffDilute::CalcMixConc(DiluteDroplet* MixCl, DiluteDroplet* MixCh) {
	int numer = (MixCl->Concentration.numer + MixCh->Concentration.numer) / 2;
	Rational PostMix = Rational(numer, MixCl->Concentration.denom);
	return PostMix;
}

long double GriffDilute::RattoFloat(Rational Concentration) {
	long double Conc = (static_cast<long double>(Concentration.numer)
			/ static_cast<long double>(Concentration.denom));
	return Conc;
}

DiluteDroplet* GriffDilute::CreateDroplet(Rational Concentration) {
	DiluteDroplet* temp = new DiluteDroplet;
	temp->Concentration = Concentration;
	return temp;
}


DiluteRet GriffDilute::PerformDilution(DiluteDroplet* di, DiluteDroplet* db, double tolerance, double DesiredConcentrate, int num_ops)
{
	bool Debug = false;
	bool Debug2 = false;
	int id = 0;
	vector< MixOp*> Mixes;

	vector<DiluteDroplet*> AvailDropletsHigher;
	AvailDropletsHigher.push_back(di);
	vector<DiluteDroplet*> AvailDropletsLower;
	AvailDropletsLower.push_back(db);
	Rational Range = FindRange(di, db);

	DiluteDroplet* dH, * dh;
	dH = di;
	id++;
	di->uID = id;
	di->count = 1L;
	DiluteDroplet* dL, * dl;// = new DiluteDroplet;
	dL = db;
	id++;
	db->uID = id;
	db->count = 1;

	double C = DesiredConcentrate;
	double tol = tolerance;

	DiluteDroplet* FinalMix = new DiluteDroplet;

	while(RattoFloat(Range.divide(Rational(2,1))) > tol)
	{
		for(unsigned i = 0; i < AvailDropletsHigher.size();i++)
		{
			dh = AvailDropletsHigher.at(i);

			for(unsigned j = 0; j < AvailDropletsLower.size(); j++)
			{
				dl = AvailDropletsLower.at(j);


			if(RattoFloat(GetConcentrate(dh).subtract(GetConcentrate(dl))) < RattoFloat(Range))
			{
				Range = (GetConcentrate(dh).subtract(GetConcentrate(dl)));

				dH = dh;
				dL = dl;
				dL->count = dL->count+1;
				dH->count = dH->count+1;
			}
			}
		}
		if(Debug){cout<<"range = "<<RattoFloat(Range)<<flush<<endl;}
		if(Debug){cout<<"droplet concentration dl is: "<<RattoFloat(dL->Concentration)<<", dh is: "<<RattoFloat(dH->Concentration)<<flush<<endl;}
		DiluteDroplet* DropletMix = new DiluteDroplet;
		DiluteDroplet* DropletWaste = new DiluteDroplet;

		DropletMix->Concentration = FloatToRat(((RattoFloat(GetConcentrate(dH).add(GetConcentrate(dL))))/2), num_ops);
		if(Debug){cout<<"resulting concentration is: "<<RattoFloat(DropletMix->Concentration)<<flush<<endl;}
		id++;
		DropletMix->uID = id;
		DropletMix->count = 0;

		DropletWaste->Concentration = FloatToRat(((RattoFloat(GetConcentrate(dH).add(GetConcentrate(dL))))/2), num_ops);
		id++;
		DropletWaste->uID = id;
		DropletWaste->count = 0;

		pair<DiluteDroplet*, DiluteDroplet*> PreMix = make_pair(dH, dL);
		pair<DiluteDroplet*, DiluteDroplet*> PostMix = make_pair(DropletMix, DropletWaste);
		if(RattoFloat(GetConcentrate(DropletMix)) > C)
		{
			AvailDropletsHigher.push_back(DropletMix);
		}
		else
		{
			AvailDropletsLower.push_back(DropletMix);
		}
		if(RattoFloat(GetConcentrate(DropletWaste)) > C)
		{
			AvailDropletsHigher.push_back(DropletWaste);
		}
		else
		{
			AvailDropletsLower.push_back(DropletWaste);
		}
		FinalMix->Concentration = DropletMix->Concentration;
		MixOp* Operation = new MixOp;
		Operation->DropletsPreMix = PreMix;
		Operation->DropletsPostMix = PostMix;

		DropletMix->Mixes = Operation;
		DropletWaste->Mixes = Operation;

		Mixes.push_back(Operation); //all mixes performed during dilution.
		DropletWaste->Mix_Splits = Mixes;
		DropletMix->Mix_Splits = Mixes;
	}
	if(Debug){cout<<"The final Mix Concentration is: "<<RattoFloat(FinalMix->Concentration)<<endl;}
	if(Debug){cout<<"The desired Mix Concentration is: "<<C<<endl;}

	if(Debug2)
	{
		for(unsigned i = 0; i < Mixes.size(); i++)
		{
			cout<<endl;
			cout<<"Mix "<<i<<" is droplet: "<<Mixes.at(i)->DropletsPreMix.first->uID<<" and droplet: "<<Mixes.at(i)->DropletsPreMix.second->uID<<" with counts "<<Mixes.at(i)->DropletsPreMix.first->count<<","<<Mixes.at(i)->DropletsPreMix.second->count<<endl;
			cout<<"resulting in Mixdroplet: "<<Mixes.at(i)->DropletsPostMix.first->uID<<" and wastedroplet: "<<Mixes.at(i)->DropletsPostMix.second->uID<<" with counts "<<Mixes.at(i)->DropletsPostMix.first->count<<","<<Mixes.at(i)->DropletsPostMix.second->count<<endl;
			cout<<"with concentration: "<<RattoFloat(Mixes.at(i)->DropletsPostMix.first->Concentration)<<flush<<endl;
		}
	}

	vector<DiluteDroplet*> AllAvailDroplets;
	for(unsigned i = 0; i < AvailDropletsHigher.size(); i++)
	{
		AllAvailDroplets.push_back(AvailDropletsHigher.at(i));
	}
	for(unsigned i = 0; i < AvailDropletsLower.size(); i++)
	{
		AllAvailDroplets.push_back(AvailDropletsLower.at(i));

	}

	if(Debug2){outputAvailableDroplets(AllAvailDroplets);}
	if(Debug2){outputMixSplits(Mixes);}

	pair< vector<MixOp*>, vector<DiluteDroplet*> > MD = make_pair(Mixes, AllAvailDroplets);
	DiluteRet ret;
	ret.DilutionVals = MD;
	vector<double> endConc;
	endConc.push_back(RattoFloat(AllAvailDroplets.back()->Concentration));
	ret.endConcentration = endConc;
	if(Debug){cout<<"endConcentration is: "<<RattoFloat(AllAvailDroplets.back()->Concentration)<<flush<<endl;}
	int base = FloatToRat(RattoFloat(AllAvailDroplets.back()->Concentration), num_ops).denom;
	ret.base = base;
	return ret;
}

bool CompareByuID(DiluteDroplet* A, DiluteDroplet* B)
{
	return A->uID < B->uID;
}

void GriffDilute::outputAvailableDroplets(vector<DiluteDroplet*> AvailableDroplets)
{
	for(unsigned i = 0; i < AvailableDroplets.size(); i++)
	{
		cout<<"ID: "<<AvailableDroplets.at(i)->uID<<", count = "<<AvailableDroplets.at(i)->count<<endl;
	}
	return;
}

void GriffDilute::outputMixSplits(vector<MixOp*> MixSplits)
{
	for(unsigned i = 0; i < MixSplits.size(); i++)
	{
		cout<<endl;
		cout<<"Mix "<<i<<" is droplet: "<<MixSplits.at(i)->DropletsPreMix.first->uID<<" and droplet: "<<MixSplits.at(i)->DropletsPreMix.second->uID<<" with counts "<<MixSplits.at(i)->DropletsPreMix.first->count<<","<<MixSplits.at(i)->DropletsPreMix.second->count<<endl;
		cout<<"resulting in Mixdroplet: "<<MixSplits.at(i)->DropletsPostMix.first->uID<<" and wastedroplet: "<<MixSplits.at(i)->DropletsPostMix.second->uID<<" with counts "<<MixSplits.at(i)->DropletsPostMix.first->count<<","<<MixSplits.at(i)->DropletsPostMix.second->count<<endl;
		cout<<"with concentration: "<<RattoFloat(MixSplits.at(i)->DropletsPostMix.first->Concentration)<<flush<<endl;
	}
	cout<<endl;
	return;
}

void GriffDilute::clearDropletCounts(vector<DiluteDroplet*> AvailableDroplets)
{
	for(unsigned i = 0; i < AvailableDroplets.size(); i++) //remove after removing counting processs from above
	{
		AvailableDroplets.at(i)->count = 0;
	}
	return;
}

void GriffDilute::calcNumDropsFin(pair <vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals)
{
	DagGen dag;
	vector<MixOp*> Mixes = DilutionVals.first;
	vector<DiluteDroplet*> AvailableDroplets = DilutionVals.second;

	std::sort(AvailableDroplets.begin(), AvailableDroplets.end(), CompareByuID);

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

		if(RattoFloat(cur->Mixes->DropletsPreMix.first->Concentration) != 1 and RattoFloat(cur->Mixes->DropletsPreMix.first->Concentration) != 0)
		{
			myQueue.push(cur->Mixes->DropletsPreMix.first);
			cur->Mixes->DropletsPreMix.first->count++;
		}
		else
		{
			cur->Mixes->DropletsPreMix.first->count++;
		}
		if(RattoFloat(cur->Mixes->DropletsPreMix.second->Concentration) != 1 and RattoFloat(cur->Mixes->DropletsPreMix.second->Concentration) != 0)
		{
			myQueue.push(cur->Mixes->DropletsPreMix.second);
			cur->Mixes->DropletsPreMix.second->count++;
		}
		else
		{
			cur->Mixes->DropletsPreMix.second->count++;
		}
	}
	return;
}

int GriffDilute::findCounter(VertexCounts* VC, Vertex2* vertex)
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

string GriffDilute::findLabel(Vertex2* vertex)
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

VertexCounts* GriffDilute::CreateDag(DagGen & dag, pair< vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals)
{
	bool debug = false;
	bool debug2 = false;
	vector<MixOp*> Mixes = DilutionVals.first;
	vector<DiluteDroplet*> AvailableDroplets = DilutionVals.second;

	std::sort(AvailableDroplets.begin(), AvailableDroplets.end(), CompareByuID);

	if(debug){cout<<"Before BFS dag generation"<<flush<<endl;}
	if(debug2){outputAvailableDroplets(AvailableDroplets);}

	//establish new counts
	queue<DiluteDroplet*> myQueue;
	int iter = 0;
	bool flag = false;
	for(unsigned i = 0; i < AvailableDroplets.size(); i++)
	{
		if(RattoFloat(AvailableDroplets.at(iter)->Concentration) != 0)
		{
			if(RattoFloat(AvailableDroplets.at(iter)->Concentration) != 1)
			{
				flag = true;
				break;
			}
			if(flag == true) {break;}
		}
		if(flag == true) {break;}
		iter++;
	}

	if(debug) { cout<<"iter is: "<<iter<<flush<<endl;}

	myQueue.push(AvailableDroplets.at(iter)); //push last

	//count decrement TODO:: remove this?? ANSWER: YES
	AvailableDroplets.at(iter)->Mixes->DropletsPreMix.first->count --;
	AvailableDroplets.at(iter)->Mixes->DropletsPreMix.second->count --;

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
		if(debug) { cout<<"start while"<<flush<<endl;}
		DiluteDroplet* cur = myQueue.front();
		myQueue.pop();

		//establish incoming mix droplets pre1
		Vertex2* pre1;
		Vertex2* pre2;

		if(RattoFloat(cur->Mixes->DropletsPreMix.first->Concentration) == 1) //is sample dispense
		{
			if(debug){cout<<"pre1 if"<<flush<<endl; }
			ostringstream convert1;
			convert1 << sampleCount;
			string label1 = sample + convert1.str();
			pre1 = this->addVertex(dag, DISPENSE, label1, NULL, NULL, 1, 1);
			pre1->type = DISPENSE;
			sampleCount++;
		}
		else if(RattoFloat(cur->Mixes->DropletsPreMix.first->Concentration) == 0)
		{
			if(debug){cout<<"pre1 else if"<<flush<<endl; }
			ostringstream convert2;
			convert2 << bufferCount;
			string label2 = buffer + convert2.str();
			pre1 = this->addVertex(dag, DISPENSE, label2, NULL, NULL, 1, 1);
			pre1->type = DISPENSE;
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
						if(debug){cout<<"pre1 else"<<flush<<endl; }
						pre1 = vertices.at(v);
					}
				}
			}
		}

		//establish incoming mix droplets pre2
		if(RattoFloat(cur->Mixes->DropletsPreMix.second->Concentration) == 1) //is sample dispense
		{
			if(debug){cout<<"pre2 if"<<flush<<endl; }
			ostringstream convert1;
			convert1 << sampleCount;
			string label1 = sample + convert1.str();
			pre2 = this->addVertex(dag, DISPENSE, label1, NULL, NULL, 1, 1);
			pre2->type = DISPENSE;
			sampleCount++;
		}
		else if(RattoFloat(cur->Mixes->DropletsPreMix.second->Concentration) == 0)
		{
			if(debug){cout<<"pre2 else if"<<flush<<endl; }
			ostringstream convert2;
			convert2 << bufferCount;
			string label2 = buffer + convert2.str();
			pre2 = this->addVertex(dag, DISPENSE, label2, NULL, NULL, 1, 1);
			pre2->type = DISPENSE;
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
						//do not create new vertex, just connect to current mix vertex.
						if(debug){cout<<"pre2 else"<<flush<<endl; }
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

		//connect incoming mix droplets to mix node, and populate dh, dl (incoming drops of mix)
		dag.addEdge(pre1, mix);
		dag.addEdge(pre2, mix);

		if(debug){ cout<<"mix added"<<flush<<endl;}

		mix->dh = cur->Mixes->DropletsPreMix.first;
		mix->dl = cur->Mixes->DropletsPreMix.second;

		//create split vertex, connect mix to split, and populate dh, dl (outgoing drops of split)
		ostringstream convertSplit;
		convertSplit << splitCount;
		string splitLabel = split + convertSplit.str();
		Vertex2* split = this->addVertex(dag, SPLIT, splitLabel, NULL, NULL, 1, 1);
		split->type = SPLIT;
		splitCount++;

		dag.addEdge(mix, split);

		if(debug){ cout<<"split added"<<flush<<endl;}

		split->dh = cur->Mixes->DropletsPostMix.first;
		split->dl = cur->Mixes->DropletsPostMix.second;

		//in naive griffith model, the second droplet is always a waste droplet and is not reused
		//Create output vertex, connect split dl to waste vertex
		ostringstream convertWaste;
		convertWaste << wasteCount;
		string wasteLabel = waste + convertWaste.str();
		Vertex2* waste= this->addVertex(dag, OUTPUT, wasteLabel, NULL, NULL, 1, 1);
		waste->type = OUTPUT;
		wasteCount++;
		//add waste to graph via edge
		dag.addEdge(split, waste);

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
					if(debug){cout<<"pushed first"<<flush<<endl; }
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
					if(debug){cout<<"pushed first"<<flush<<endl; }
					myQueue.push(Mixes.at(p)->DropletsPostMix.first); //do not push second, always waste
					prevPushed.push_back(Mixes.at(p)->DropletsPostMix.first);
				}
			}
		}
	}

	//add output droplet and edge to final split node.
	if(debug){cout<<"add output"<<flush<<endl; }
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
	string outLable = "Output"+convert.str();
	Vertex2* out = this->addVertex(dag, OUTPUT, outLable, NULL, NULL, 1, 1);

	dag.addEdge(fsplitIndex, out->uniqueID);

	if(debug2){outputVertices();}
	if(debug2){dag.outputEdges();}

	VertexCounts* ret = new VertexCounts;
	ret->bufferCount = bufferCount;
	ret->mixCount = mixCount;
	ret->sampleCount = sampleCount;
	ret->splitCount = splitCount;
	ret->wasteCount = wasteCount;

	return ret;
}

void GriffDilute::expandDag(DagGen & dag, VertexCounts* VC)
{
	bool Debug = true;
	bool Debug2 = true;
	vector<Edge*> edges = dag.Edges();
	int sampleCount = VC->sampleCount;
	int bufferCount = VC->bufferCount;
	int mixCount = VC->mixCount;
	int splitCount = VC->splitCount;

	unsigned i = 0;
	int count = vertices.size();
	for(i; i < vertices.size(); i++) //check all vertices
	{
		if(vertices.at(i)->type == SPLIT) //if its a split
		{
			if(vertices.at(i)->dh != NULL)
			{
				if(vertices.at(i)->dh->count > 1) //if its outgoing droplet has count > 1 (used more than once)1
				{
					int lastID = vertices.at(i)->uniqueID;
					vector<int> lastVertices;

					for(unsigned j = 0; j < vertices.at(i)->dh->count-1; j++) //create subtree count # times
					{
						//add outgoing edges
						unsigned k = 0;
						while(edges.at(k)->parent != vertices.at(i)->uniqueID) //find start of this outgoing edge
						{
							k++;
						}
						vector< pair<int, int> > added; //first == newID for copy vertex //second == oldID for orig vertex
						for(unsigned l = 0; l < k; l++) //for all edges previous to current node
						{
							int pID = edges.at(l)->parent;
							int cID = edges.at(l)->child;
							bool pfound = false;
							int ploc;
							bool cfound = false;
							int cloc;

							Vertex2* p;
							Vertex2* c;

							//check if the parent was already added this round
							for(unsigned val = 0; val < added.size(); val++) //comparison NEW
							{
								if(added.at(val).second == pID)
								{

									pfound = true;
									ploc = added.at(val).first;
								}
								if(added.at(val).second == cID)
								{
									cfound = true;
									cloc = added.at(val).first;
								}
							}

							if(pfound == false) //NEW based on comparison
							{
								int countVal = findCounter(VC, vertices.at(pID));
								ostringstream convertP;
								convertP << countVal;
								VertexType T1 = vertices.at(pID)->type;
								//string pLabel = vertices.at(pID)->label + "." + convertP.str();
								string pLabel = findLabel(vertices.at(pID)) +  convertP.str();
								p = this->addVertex(dag, T1, pLabel, NULL, NULL, 1, 1);

								count++;

								int newpID =  p->uniqueID;//old id
								int oldpID = pID;
								pair<int, int> pusher = make_pair(newpID, oldpID);
								added.push_back(pusher);//NEW

								if(pusher.second == lastID) //this is the vertex that has more than one outgoing
								{
									lastVertices.push_back(pusher.first);
								}
							}
							else //parent was found, so just establish location
							{
								p = vertices.at(ploc);
							}

							//create the copy child node
							if(cfound == false)
							{
								int countVal = findCounter(VC, vertices.at(cID));
								ostringstream convertC;
								convertC << countVal;
								VertexType T2 = vertices.at(cID)->type;
								string cLabel = findLabel(vertices.at(cID)) + convertC.str();
								c = this->addVertex(dag, T2, cLabel, NULL, NULL, 1, 1);

								count++;

								int newcID = c->uniqueID;//old id
								int oldcID = cID;
								pair<int, int> pusher = make_pair(newcID, oldcID);
								added.push_back(pusher);//NEW

								if(pusher.second == lastID) //this is the vertex that has more than one outgoing.
								{
									lastVertices.push_back(pusher.first);
								}
							}
							else //child was found, so just estblish location
							{
								c = vertices.at(cloc);
							}

							//create the copy edge
							dag.addEdge(p,c);
						}

						if(j == vertices.at(i)->dh->count - 2) //if this is the last iteration of the copy
						{
							vector<int> lastOut;

							bool first = true;
							for(unsigned t = 0; t < edges.size(); t++)
							{

								if(edges.at(t)->parent == lastID)
								{
									if(vertices.at(edges.at(t)->child)->type == OUTPUT)
									{
										continue;
									}
									else
									{
										lastOut.push_back(edges.at(t)->child);
										if(first == false)
										{
											edges.erase(edges.begin()+t, edges.begin()+t+1);
											dag.removeEdge(t);
										}
										first = false;
									}
								}
							}
							lastOut.erase(lastOut.begin(), lastOut.begin()+1);

							for(unsigned u = 0; u < lastVertices.size(); u++)
							{
								//connect copy vertices to associated children
								dag.addEdge(lastVertices.at(u), lastOut.at(u));

								//because it's a split make the new waste droplets.
								if(vertices.at(lastID)->type == SPLIT)
								{
									VC->wasteCount = VC->wasteCount+1;
									int countVal = VC->wasteCount;
									ostringstream convertWaste;
									convertWaste << countVal;
									string wasteLabel = "waste" + convertWaste.str();
									Vertex2* waste = this->addVertex(dag, OUTPUT, wasteLabel, NULL, NULL, 1, 1);

									waste->type = OUTPUT;
									count++;
									dag.addEdge(lastVertices.at(u), waste->uniqueID);
								}
							}
						}
					}
				}
			}
		}
	}

	return;
}

DagGen GriffDilute::RunGriffith(int argc, char ** argv)
{
	int num_ops = atoi(argv[0]);
	double DesiredConcentrate = atof(argv[1]);
	double tolerance = atof(argv[2]);

	DiluteDroplet* db = new DiluteDroplet;
	db->Concentration = Rational(0,pow(2,num_ops));
	DiluteDroplet* di = new DiluteDroplet;
	di->Concentration = Rational(pow(2,num_ops),pow(2,num_ops));

	DagGen dag;
	GriffDilute GD;

	pair< vector<MixOp*>, vector<DiluteDroplet*> > DilutionVals;
	DiluteRet valid;

	cout<<" /////////before PerformDilution\\\\\\\\\ "<<flush<<endl;
	valid = GD.PerformDilution(di, db, tolerance, DesiredConcentrate, num_ops);
	DilutionVals = valid.DilutionVals;
	cout<<" /////////after PerformDilution \\\\\\\\\\ "<<flush<<endl;

	cout<<" //////////before CreateDag \\\\\\\\\ "<<flush<<endl;
	VertexCounts* VC = GD.CreateDag(dag, DilutionVals);
	cout<<" //////////after CreateDag\\\\\\\\\ "<<flush<<endl;

	cout<<" //////////before expand Dag\\\\\\\\\ "<<flush<<endl;
	GD.expandDag(dag, VC);
	cout<<" //////////after expand Dag\\\\\\\\\ "<<flush<<endl;

	cout<<" //////////before generate DropletDag\\\\\\\\\ "<<flush<<endl;
	dag.generateDropletDag("DropletDagT.cpp");
	cout<<" //////////after generate DropletDag\\\\\\\\\ "<<flush<<endl;

	cout<<" //////////before generate DotyGraph\\\\\\\\\ "<<flush<<endl;
	dag.generateDotyGraph("Test.dot");
	cout<<" //////////after generate DotyGrpah\\\\\\\\\ "<<flush<<endl;

	cout<<" /////////before validation testing\\\\\\\\\ "<<flush<<endl;
	int base = valid.base;
	vector<double> endConcentration = valid.endConcentration;
	bool value = dag.isValidSingleReactantDilution(endConcentration, base);
	cout<<" //////////end validation testing\\\\\\\\\ "<<flush<<endl;

	if(value)
	{
		cout<<"Validity test passed"<<flush<<endl;
	}
	else
	{
		cout<<"validity test not passed"<<flush<<endl;
	}

	return dag;
}
