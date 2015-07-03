#include "../Headers/GDA.h"

//Using the function findSubset to find the subset from Q such that sum == T +/- 0.5
void GDA :: findSubset(long double *p, int *Qindex, int index, int low, int high, long double sum)
{
	if( sum <= -0.5 || low > high )
		return;

	if( fabsl(sum) < 0.5 )
	{
		vld tmp;
		vector < int > tmp_index;
    		for(int i=0;i<index;i++)
		{
			tmp.push_back(p[i]);
			tmp_index.push_back(Qindex[i]);
		}
		R.push_back(tmp);
		Rindex.push_back(tmp_index);
    		return;
	}

	p[index]=Q[low];
	Qindex[index] = low;

	findSubset(p,Qindex,index+1,low+1,high,sum-Q[low]);
	findSubset(p,Qindex,index,low+1,high,sum);
}

void GDA :: RunGDA(int argc, char* argv[], DagGen *dag)
{
	//(<b1, b2, b3, ..., bn>, Ca, d)
	//where <b1, b2, b3, ..., bn> are the CF's of the related inputs <A1, A2, A3, ..., An> & n>=2 & each b is a positive real number such that 0<b<=1
	//Ca is the desired CF of the biofluid & Ca is a positive real number such that 0<=Ca<=1
	//d is an integer that detrmines the accuracy level of Ca.
	int i,j;
	char *stopstring;
	if(dag == NULL)
		dag = new DagGen();

	//DagGen* dag = new DagGen();
	GDA gda;
//cout << argc<<endl;
	//Getting the input parameters <b1, b2, b3, ..., bn>, Ca, & d
	for(i=1; i<argc-2; i++) {
//cout<< i<< endl;
		cout<<gda.b.size()<<endl;
		long double t = strtold(argv[i], &stopstring);
		gda.b.push_back(t);
	}

	gda.Ca = strtold(argv[i++], &stopstring);
	gda.d = atoi(argv[i]);

	//**********************Step 2 of Algorithm 1 begins here**********************
	//Converting <b1, b2, b3, ..., bn> to <B1/2^d, B2/2^d, B3/2^d, ..., Bn/2^d> & storing <B1, B2, B3, ..., Bn> in vector B.
	for(i=0; i<gda.b.size(); i++)
		gda.B.push_back(gda.b[i]*pow(2,gda.d));
	//T = Ca * 2^d
	gda.T = gda.Ca * pow(2,gda.d);
	//**********************Step 2 of Algorithm 1 ends here**********************

	//**********************Step 3 & 4 of Algorithm 1 begins here**********************
	for(i=0; i<gda.B.size(); i++)
	{
		vld tmp;
		tmp.push_back(gda.B[i]);
		gda.W.push_back(tmp);
	}
	//Following Step 1 here. Directly pushing the buffer solution to W
	vld tmp;
	tmp.push_back(0.0);
	gda.W.push_back(tmp);
	//**********************Step 3 & 4 of Algorithm 1 ends here**********************

	//**********************Step 5, 6 & 7 of Algorithm 1 begins here**********************
	for(i=0; i<gda.W.size(); i++)
		for(j=0; j<gda.d; j++)
			gda.W[i].push_back(gda.W[i][0]/pow(2,j+1));

	for(i=0; i<gda.W.size()-1; i++)
		for(j=0; j<gda.W[i].size(); j++)
		{
			gda.Q.push_back(gda.W[i][j]);
			gda.w.push_back(1/pow(2,j));
		}
	//**********************Step 5, 6 & 7 of Algorithm 1 ends here**********************

	//**********************Step 8 of Algorithm 1 begins here**********************
	long double *p = new long double[gda.Q.size()];
	int *Qindex = new int[gda.Q.size()];
	gda.findSubset(p, Qindex, 0, 0, gda.Q.size()-1, gda.T);
	//**********************Step 8 of Algorithm 1 ends here**********************

	//**********************Step 9 to 12 of Algorithm 1 begins here**********************
	long double V;
    	for(i=0;i<gda.Rindex.size();i++)
    	{
		V = 0.0;
		for(j=0; j<gda.Rindex[i].size(); j++)
			V += gda.w[gda.Rindex[i][j]];
		if(V <= 1)
		{
			gda.validR.push_back(gda.R[i]);
			gda.validRindex.push_back(gda.Rindex[i]);
		}
    	}
	//**********************Step 9 to 12 of Algorithm 1 ends here**********************

	//############################ STEP 13 TO 17 BEGINS HERE ############################

	//**********************Step 14 of Algorithm 1 begins here**********************
	//Computing the value of d' for every valid solution
	int d1;
	for(i=0;i<gda.validRindex.size();i++)
	{
		V = 1.0;
		d1 = 0;
		for(j=0;j<gda.validRindex[i].size();j++)
		{
			if(V > gda.w[gda.validRindex[i][j]])
				V = gda.w[gda.validRindex[i][j]];
		}
		while(V != 1.0)
		{
			d1++;
			V *= 2;
		}
		gda.d1_validR.push_back(d1);
	}
	//Finding the smallest d' in the entire set
	d1 = gda.d + 2;
	for(i=0;i<gda.d1_validR.size();i++)
		if(d1 > gda.d1_validR[i])
			d1 = gda.d1_validR[i];

	//Keeping the valid solution(s) with smallest d' in R_Optimal_d1
	for(i=0;i<gda.d1_validR.size();i++)
		if(d1 ==  gda.d1_validR[i])
			gda.R_Optimal_d1.push_back(gda.validR[i]);

	/*cout<<"Smallest d' = "<<d1<<endl;
	cout<<"Sets are - \n";
    	for(i=0;i<R_Optimal_d1.size();i++)
	{
		for(j=0; j<R_Optimal_d1[i].size(); j++)
			cout<<R_Optimal_d1[i][j]<<"\t";
		cout<<"\n";
	}*/
	//**********************Step 14 of Algorithm 1 ends here**********************

	//**********************Step 15 of Algorithm 1 begins here**********************
	int m;
	//Computing the value of 'm' (step 15) for all the valid solutions in the valid set
    	for(i=0;i<gda.validRindex.size();i++)
    	{
		//First find the cumulative weight of the related inputs of each valid solutions one by one and store in V
		V = 0.0;
		for(j=0; j<gda.validRindex[i].size(); j++)
			V += gda.w[gda.validRindex[i][j]];

		//Since in Step 15, m = |R|+ {Beta}d'(D), we store the |R| for the current solution in 'm'.
		m = gda.validR[i].size();

		//Only if V < 1, the buffer droplets will be needed else not. Ofcourse the cases with V > 1 have all been ruled out in the previous steps
		if(V < 1.0)
		{
			int beta_d = 0;	//holds the value {Beta}d'(D)
			//Doing V = 1 - V means computing what is the contribution of buffer to get the final solution & then as the paper says we just count the number of 1's coming in the binary representation of V now.
			V = 1.0 - V;
			while(V != 0.0)
			{
				V = V*2;
				if(V >= 1.0)
				{
					beta_d++;
					V = V - 1.0;
				}
			}
			//Again from Step 15, m = |R|+ {Beta}d'(D), we add the count of 1's to m
			m += beta_d;
		}
		//Pushing the value of 'm' to the vector. Once we have the m value for each valid solution in the set R, we can find the optimal solution.
		gda.m_validR.push_back(m);
    	}
	m = gda.Q.size() + 1;	//Setting the size of 'm' size of the vector Q + 1. Since no solution can have this number of related inputs & buffers combined.
	//Finding the smallest 'm' in the entire set
	for(i=0;i<gda.m_validR.size();i++)
		if(m > gda.m_validR[i])
			m = gda.m_validR[i];

	//Keeping the valid solution(s) with smallest 'm' in R_Optimal_m
	for(i=0;i<gda.m_validR.size();i++)
		if(m == gda.m_validR[i])
			gda.R_Optimal_m.push_back(gda.validR[i]);

	/*cout<<"Smallest m = "<<m<<endl;
	cout<<"Sets are - \n";
    	for(i=0;i<R_Optimal_m.size();i++)
	{
		for(j=0; j<R_Optimal_m[i].size(); j++)
			cout<<R_Optimal_m[i][j]<<"\t";
		cout<<"\n";
	}*/

	//**********************Step 15 of Algorithm 1 ends here**********************

	//############################ STEP 13 TO 17 ENDS HERE ############################

	cout<<"****************************************\nW :-\n";
	for(i=0; i<gda.W.size()-1; i++)
	{
		for(j=0; j<gda.W[i].size(); j++)
			cout<<gda.W[i][j]<<"\t";
		cout<<"\n";
	}

	cout<<"# Valid R's are = "<<gda.validR.size()<<"\n";
	for(i=0;i<gda.validR.size();i++)
	{
		V = 0.0;
		for(j=0;j<gda.validR[i].size();j++)
		{
			V += gda.validR[i][j];
			cout<<gda.validR[i][j]<<"\t";
		}
		cout<<" = "<<V<<"\n";
	}
	cout<<"***********************************************\n# Valid Indexes are = "<<gda.validRindex.size()<<"\n";
    	for(i=0;i<gda.validRindex.size();i++)
	{
		for(j=0; j<gda.validRindex[i].size(); j++)
			cout<<gda.w[gda.validRindex[i][j]]<<"\t";
		cout<<"\n";
	}

	cout<<"Your Dag files are in ../output folder\n";

  //  	dag.generateJSON("../output/Example.json");
	//dag.generateDropletDag("../output/DropletDag.cpp");
	dag->generateDotyGraph("Example.dot");

	return;

}
