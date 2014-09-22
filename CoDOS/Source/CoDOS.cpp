#include "../Headers/CoDOS.h"

using namespace std;
CoDOS :: CoDOS()
{
	d = -1;
	NR = 0;
}

void CoDOS :: binary(int number, std::vector < int >& r)
{
	int remainder;

	if(number <= 1) 
	{
		r.push_back(number);
		return;
	}

	remainder = number%2;
	binary(number >> 1, r);    
	r.push_back(remainder);
}

Matrix CoDOS :: ConstructRecipeMatrix(Entry T)
{
	Matrix M;
	int val;

	//Constructing the recipe matrix
	for(int i = 0; i<T.size(); i++)
	{
		Entry row;
		Entry r;
		Entry in;

		val = T[i];

		binary(val, r);

		val = d - r.size();

		while(val != 0)
		{
			row.push_back(0);
			val--;
		}
		for(int j = 0; j<r.size(); j++)
			row.push_back(r[j]);

		M.push_back(row);
		in.push_back(i);
		index.push_back(in);
	}
	return M;
}

std::vector < Rect_Cood > CoDOS :: getRectangle(Matrix M)
{
	Rect_Cood K;
	std::vector < Rect_Cood > rectangles;
	int i, j;
	int N;
	int x1, y1, x2, y2, x3, y3, x4, y4;
	for(i = 0; i<M.size(); i++)
		for(int j = 0; j<M[i].size(); j++)
			if(M[i][j] == 1)
				K.push_back(make_pair(i,j));

	N = M.size();

	for(Rect_Cood::iterator it = K.begin(); it != K.end(); ++it)
	{
		Rect_Cood tmp;

		x1 = it->first;
		y1 = it->second;
		
		if(x1 < N-1 && y1 < d-1)
		{
			x2 = x1;
			y2 = y1 + 1;

			while(y2 < d)
			{
				x3 = x1 + 1;
				y3 = y1;

				x4 = x2 + 1;
				y4 = y2;

				while(x3 < N)
				{
					if(M[x1][y1] == 1 && M[x2][y2] == 1 && M[x3][y3] == 1 && M[x4][y4] == 1)
					{
						tmp.push_back(make_pair(x1,y1));
						tmp.push_back(make_pair(x4,y4));
						rectangles.push_back(tmp);
					}
					x3++;
					x4++;
				}
				y2++;
			}
		}
	}

	return rectangles;
}

Rect_Cood CoDOS :: getHighestPrecedenceRectangle(std::vector < Rect_Cood > rectangles, Entry W)
{
	int r1, r2;
	std::vector < int > gain;
	std::vector < float > volume;
	Rect_Cood k;

	int max_gain = 0;
	float min_volume = std::numeric_limits<float>::max();

	int g;
	float v;

	for(int i = 0; i<rectangles.size(); i++)
	{
		g = 0;
		v = 0.0;

		k = rectangles[i];

		Rect_Cood::iterator it = k.begin();
		r1 = it->first;
		it++;
		r2 = it->first;

		if(r1 < W.size())
			g += W[r1];

		if(r2 < W.size())
			g += W[r2];

		gain.push_back(g);

		it = k.begin();
		r1 = it->second;
		it++;
		r2 = it->second;

		v += 1.0/pow(2, r1+1);
		v += 1.0/pow(2, r2+1);

		volume.push_back(v);
	}

	for(int i = 0; i<rectangles.size(); i++)
	{
		if(max_gain < gain[i])
		{
			max_gain = gain[i];
			min_volume = volume[i];
			k = rectangles[i];
		}
		else if(max_gain == gain[i])// && max_gain != 0)
		{
			if(min_volume > volume[i])
			{
				max_gain = gain[i];
				min_volume = volume[i];
				k = rectangles[i];
			}
		}
	}
	return k;
}

Matrix CoDOS :: CoDOS_Main(Entry T, Entry W)
{
	Matrix M;
	
	std::vector < Rect_Cood > rectangles;
	Rect_Cood k;
	int x1,x4,y1,y4;

	NR = T.size();
	M = ConstructRecipeMatrix(T);

	while(1)
	{
		rectangles = getRectangle(M);

		if(rectangles.size() == 0)
			break;
		else if(rectangles.size() == 1)
			k = rectangles[0];
		else
			k = getHighestPrecedenceRectangle(rectangles, W);

		Rect_Cood::iterator it = k.begin();
		x1 = it->first;
		y1 = it->second;
		it++;
		x4 = it->first;
		y4 = it->second;

		M[x1][y1] = 0;
		M[x1][y4] = 0;
		M[x4][y1] = 0;
		M[x4][y4] = 0;

		Entry row;
		Entry in;
		for(int i = 0; i<d; i++)
		{
			if(i == y1-1 || i == y4-1)
				row.push_back(1);
			else
				row.push_back(0);
		}
		M.push_back(row);

		in.push_back(x1);
		in.push_back(x4);
		index.push_back(in);
	}

	return M;
}

Vertex* CoDOS :: createVertex(int count, std::string str,DagGen &dag, VertexType type)
{
	std::ostringstream oss;
	Vertex* v;

	oss << count;
	str += oss.str();

	v = dag.addVertex(type, str);
	return (v);
}

void CoDOS :: Construct_Graph(Matrix M, DagGen &dag)
{
	int m = 1, s = 1, w = 1, o = 1;
	Matrix ver_mat;
	std::stack< Vertex *> vertices;
	Matrix level;
	bool flag;

	for(int i = 0; i<M.size(); i++)
	{
		Entry ver;
		flag = true;
		for(int j = 0; j<M[i].size(); j++)
		{
			if(M[i][j] == 1)
			{
				Vertex* vs; 

				if(index[i].size() == 1)
				{
					Vertex* vr = createVertex(i+1, "Reactant", dag, DISPENSE);
					ver.push_back(vr->uniqueID);
					vertices.push(vr);
				}
				else if(flag == true)
				{
					Vertex* vm = createVertex(m++, "Mix", dag, MIX);
					vs = createVertex(s++, "Split", dag, SPLIT);
					vertices.push(vs);
					ver.push_back(vs->uniqueID);
					dag.addEdge(vm, vs);
					for(int j = 0; j<index[i].size(); j++)
					{
						if(index[i][j] < NR)
						{
							Vertex* vr = createVertex(index[i][j]+1, "Reactant", dag, DISPENSE);
							dag.addEdge(vr, vm);
						}
						else
						{
							Vertex* vm1 = createVertex(m++, "Mix", dag, MIX);
							Vertex* vs1 = createVertex(s++, "Split", dag, SPLIT);
							Vertex* vw = createVertex(w++, "Waste", dag, WASTE);

							dag.addEdge(vm1, vs1);
							dag.addEdge(vs1, vw);
							dag.addEdge(vs1, vm);
							for(int k = 0; k<index[index[i][j]].size(); k++)
							{
								Vertex* vr = createVertex(index[index[i][j]][k]+1, "Reactant", dag, DISPENSE);

								dag.addEdge(vr, vm1);
							}
						}
					}
					flag = false;
				}
				else if(flag == false)
					ver.push_back(vs->uniqueID);
			}
			else
				ver.push_back(-1);
		}
		ver_mat.push_back(ver);
	}

	for(int j = 0; j < d; j++)
	{
		Entry l;
		for(int i = 0; i < ver_mat.size(); i++)
			if(ver_mat[i][j] != -1)
				l.push_back(ver_mat[i][j]);
		level.push_back(l);
	}

	for(int i = level.size()-1; i>=0; i--)
	{
		if(level[i].size() != 0)
			for(int j = 0; j<level[i].size()-1; j = j+2)
			{
				Vertex* v1;
				Vertex* v2;
				Vertex* vm = createVertex(m++, "Mix", dag, MIX);
				Vertex* vs = createVertex(s++, "Split", dag, SPLIT);
				Vertex* vw = createVertex(w++, "Waste", dag, WASTE);
				dag.addEdge(vm, vs);
				dag.addEdge(vs, vw);
				for(std::stack< Vertex *> dummy = vertices; !dummy.empty(); dummy.pop())
				{
					if(dummy.top()->uniqueID == level[i][j])
						v1 = dummy.top();
					if(dummy.top()->uniqueID == level[i][j+1])
						v2 = dummy.top();
				}
				dag.addEdge(v1, vm);
				dag.addEdge(v2, vm);
				if(i != 0)
					level[i-1].push_back(vs->uniqueID);
				else
				{
					Vertex* vo = createVertex(o++, "Output", dag, OUTPUT);
					dag.addEdge(vs, vo);
				}
				vertices.push(vs);
			}
	}
}

DagGen CoDOS :: RunCoDOS(int argc, char* argv[])
{
	//(option)1 - CV_R1, Wt_R1, CV_R2, Wt_R2, CV_R3, Wt_R3, ... -> Each reactant has different weight
	//(option)2 - CV_R1, CV_R2, CV_R3, CV_R4, ...	-> Each Reactant has same weight i.e., 1

	DagGen dag;
	CoDOS codos;
	Matrix M;
	Entry T;	//T holds the target concentration which is concentration of each reactant in the target droplet, like <6,7,3>
	Entry W;	//W holds the cost weights of reactants.
	int val_cv = 0, tmp_val_cv, i;

	if(atoi(argv[1]) == 1)
		for(i = 2; i<argc; i = i+2)
		{
			T.push_back(atoi(argv[i]));
			val_cv += atoi(argv[i]);
			W.push_back(atoi(argv[i+1]));
		}
	else
		for(i = 2; i<argc; i++)
		{
			T.push_back(atoi(argv[i]));
			val_cv += atoi(argv[i]);
			W.push_back(1);
		}

	tmp_val_cv = val_cv;

	while(val_cv != 0)
	{
		codos.d++;
		val_cv /= 2;
	}

	if(tmp_val_cv > pow(2,codos.d))
		codos.d++;

	val_cv = pow(2,codos.d);
	i = pow(2, codos.d-1);

	if(val_cv - tmp_val_cv <= tmp_val_cv - i)
	{
		i = 0;
		while(tmp_val_cv != val_cv)
		{
			T[i] = T[i] + 1;
			tmp_val_cv++;
			i++;
			if(i == T.size())
				i = 0;
		}
	}
	else
	{
		val_cv = i;
		i = 0;
		while(tmp_val_cv != val_cv)
		{
			T[i] = T[i] - 1;
			tmp_val_cv--;
			i++;
			if(i == T.size())
				i = 0;
		}
	}


	std::cout<<"achievable CV = <";
	for(i=0; i<argc-2; i++)
		std::cout<<T[i]<<",";
	std::cout<<">\n";

	M = codos.CoDOS_Main(T,W);

	codos.Construct_Graph(M, dag);

	return dag;
}
