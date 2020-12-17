#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<vector>
#include<algorithm>
#include<string>
#include<time.h>
using namespace std;

int n;
const int ant = 100;
const int Iter_max = 1000;
const double Alpha = 1;
const double beta = 5;
const double rho = 0.1;
const double Q = 100;

vector<vector<double> > C, ethea, deltatau, Tau;
vector<vector<int> > Tabu, R_best;
double L_best[Iter_max]; 
double L_avg[Iter_max];

inline void
initialize() {
	n = graph.get_city_num();
	C.resize(n, vector<double>(n));
	ethea.resize(n, vector<double>(n));
	deltatau.resize(n, vector<double>(n));
	Tau.resize(n, vector<double>(n));
	Tabu.resize(ant, vector<int>(n));
	R_best.resize(Iter_max, vector<int>(n));
	for (int i = 0; i < n; i++)			//初始化 D[n][n]
	{
		vector<double> temp(n, 0);
		vector<double> temp2(n, 1);
		deltatau.push_back(temp);
		Tau.push_back(temp2);
	}

	//初始化 ethea[n][n] 
	for (int i = 0; i < n; i++) {
		vector<double> temp;
		for (int j = 0; j < n; j++) {
			temp.push_back (1.0 / graph.get_dis(i, j));
		}
		ethea.push_back(temp);
	}
	//初始化禁忌表 Tabu
	for (int i = 0; i < ant; i++) {
		vector<int> temp(n, 0);
		Tabu.push_back(temp);
	}
}


inline double
rnd(double lower, double upper) { return (rand() / (double)RAND_MAX * (upper - lower) + lower); }

inline void
separate_visited_not_visited(int i, int j, vector<int> &visited, vector<int> &I, vector<double> &P) {
	for (int k = 0; k < j; k++) visited.push_back(Tabu[i][k]); 
	for (int k = 0; k < n; k++) {
		if (find(visited.begin(), visited.end(), k) == visited.end()) {
			I.push_back(k); 
			P.push_back(0.0);
		}
	}
}

inline double
calculate_pk(vector<double>& P, vector<int>& I, vector<int> &visited) {
	double Psum = 0.0;
	for (int k = 0; k < P.size(); k++) {  
		P[k] = pow(Tau[visited.back()][I[k]], Alpha) * pow(ethea[visited.back()][I[k]], beta); // p_xy^k上半部分
		Psum += P[k];
	}
	return Psum;
}

inline int
get_next_city(double const Psum,vector<double>&P, vector<int> &I) {
	double rate = rnd(0.0, Psum),choose =0.0; 
	for (int k = 0; k < P.size(); k++) {
		choose += P[k];
		if (choose >= rate) {
			return(I[k]);
		}
	}
	return -1;
}

inline void
calculate_path(vector<double> &L) {
	for (int i = 0; i < ant; i++) L.push_back(0.0);
	for (int i = 0; i < ant; i++)
	{
		for (int j = 0; j < n - 1; j++)
		{
			L[i] += graph.get_dis(Tabu[i][j], Tabu[i][j + 1]);
		}
		L[i] += graph.get_dis(Tabu[i][0], Tabu[i][n - 1]);
	}
}

inline void
find_min_path(int iter,const vector<double> L) {
	double min_value = L[0];
	double sum_value = L[0];
	int min_index = 0;
	for (int i = 1; i < ant; i++) {
		sum_value += L[i];
		if (L[i] < min_value)
		{
			min_value = L[i];
			min_index = i;
		}
	}
	L_best[iter] = min_value;
	L_avg[iter] = sum_value / ant;
	for (int i = 0; i < n; i++) R_best[iter][i] = Tabu[min_index][i];
}

inline void
update_tau(vector<double> &L) {
	for (int i = 0; i < ant; i++) {
		for (int j = 0; j < n - 1; j++) {
			deltatau[Tabu[i][j]][Tabu[i][j + 1]] += Q / L[i];
		}
		deltatau[Tabu[i][n - 1]][Tabu[i][0]] += Q / L[i];
	}
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			Tau[i][j] = (1 - rho) * Tau[i][j] + deltatau[i][j];	
		}
	}
}

inline void
find_min_path_and_draw(int iter) {
	double min_L = L_best[0];			
	int min_L_index = 0;				
	int Shortest_Route[n];				
	for (int i = 0; i < iter; i++)
	{
		if (L_best[i] < min_L)
		{
			min_L = L_best[i];
			min_L_index = i;
		}
	}

	cout << "The length of the shortest route is " << min_L << endl;
	cout << "The number of iteration is " << min_L_index << endl;
	cout << "The Shortest route is： " << endl << "start";

	for (int i = 0; i < n; i++)		//所有迭代中的最优路径
	{
		Shortest_Route[i] = R_best[min_L_index][i];
		cout << " -> " << Shortest_Route[i];
	}
}

inline void
ant_colony_optimization() {

	initialize();
	clock_t start, end, total;
	int iter = 0;
	total = 0;
	while (iter < Iter_max && total<=900) {
		cout << "total time is: " << total << endl;
		start = clock();
		for (int i = 0; i < ant; i++) Tabu[i][0] = 0; 
		for (int j = 1; j < n; j++) {
			for (int i = 0; i < ant; i++) {
				vector<int> visited, I; 
				vector<double> P; 
				double Psum = 0.0; 
				int to_visit; 
				separate_visited_not_visited(i, j, visited, I, P);
				to_visit = rand() % I.size();
				Psum = calculate_pk(P, I, visited);
				int nc = get_next_city(Psum, P, I);
				Tabu[i][j] = (nc == -1) ? to_visit : nc;
			}
		}
		vector<double> L;
		calculate_path(L);
		find_min_path(iter, L);
		cout << iter << ": L_best is " << L_best[iter] << ' ' << "L_ave is " << L_avg[iter] << endl;
		iter++;
		update_tau(L);
		for (int i = 0; i < ant; i++)			
			for (int j = 0; j < n; j++)
				Tabu[i][j] = 0;
		end = clock();
		total += (end-start)/CLOCKS_PER_SEC;
	}
		find_min_path_and_draw(iter);
}

int main() {
	initialize();
	ant_colony_optimization();
	return 0;
}