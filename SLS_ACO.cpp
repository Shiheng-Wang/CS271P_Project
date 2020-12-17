#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <string>
#include <time.h>
#include "Graph.h"

using namespace std;

const int ant = 100; //���ϵ����� ������Ҫ���� 
int n;
// const int n = 99; // ���е�����
const int Iter_max = 1000; // ��������
const double Alpha = 1; // ������Ϣ����Ҫ����, ������Ҫ���е���
const double beta = 5; //��������ʽ������Ҫ�̶Ȳ���, ������Ҫ���е���
const double rho = 0.1; //����ϵ�� ������Ҫ���� 
const double Q = 100; //��Ϣ������ǿ��ϵ�� ������Ҫ����

vector<vector<double> > C, ethea, deltatau, Tau;
vector<vector<int> > Tabu, R_best;

// double C[n][n];
// double D[n][n]; //ͼ���ٽӾ���
// double ethea[n][n]; // ����ʽ����, �����õ�1/dxy 
// double deltatau[n][n]; //����ʽ���ӵı仯��
// double Tau[n][n]; //·������Ϣ�ص�Ũ��
// int Tabu[ant][n]; //���ɱ� �洢�߹���·�� 

double L_best[Iter_max]; //�洢ÿ�ε��������·������
double L_avg[Iter_max]; //�洢ÿ�ε�����ƽ��·������
// int R_best[Iter_max][n]; // �洢ÿ�ε��������·�� 

// inline void
// input_file(const string &textfile) {
// 	ifstream file;
// 	string line;
// 	size_t pos = 0;
// 	string delimiter = " ";
// 	vector<string> token;
// 	file.open(textfile);
// 	if (!file) cout << "Unable to open file." << endl;
// 	int line_number = 0;
// 	while (getline(file, line)) {
// 		while ((pos = line.find(delimiter)) != string::npos) {
// 			token.push_back(line.substr(0, pos));
// 			line.erase(0, pos + delimiter.length());
// 		}
// 		for (int i = 0; i < token.size(); i++) C[line_number][i] = stod(token[i]);
// 		line_number++;
// 		token.clear();
// 	}
// }

inline void
initialize(Graph &graph) {
	// input_file("Text.txt");
	n = graph.get_city_num();
	C.resize(n, vector<double>(n));
	ethea.resize(n, vector<double>(n));
	deltatau.resize(n, vector<double>(n));
	Tau.resize(n, vector<double>(n));
	Tabu.resize(ant, vector<int>(n));
	R_best.resize(Iter_max, vector<int>(n));
	for (int i = 0; i < n; i++)			//��ʼ�� D[n][n]
	{
		for (int j = 0; j < n; j++)
		{
			deltatau[i][j] = 0;
			Tau[i][j] = 1;
			if (i != j)
				D[i][j] = pow(pow((C[i][0] - C[j][0]), 2) + pow((C[i][1] - C[j][1]), 2), 0.5);
			else
				D[i][j] = DBL_EPSILON;
		}
	}

	//��ʼ�� ethea[n][n] 
	for (int i = 0; i < n; i++)		
		for (int j = 0; j < n; j++) {
			ethea[i][j] = 1.0 / D[i][j];
		}

	//��ʼ�����ɱ� Tabu
	for (int i=0;i<ant;i++)
		for (int j=0;j<n;j++)
			Tabu[i][j] = 0; //���н��ɱ�Ϊ0
}

//�漴���� ����lower��uper֮���һ��double���������
inline double
rnd(double lower, double upper) { return (rand() / (double)RAND_MAX * (upper - lower) + lower); }

inline void
separate_visited_not_visited(int i, int j, vector<int> &visited, vector<int> &I, vector<double> &P) {
	for (int k = 0; k < j; k++) visited.push_back(Tabu[i][k]); // ���Ѿ�ȥ���ĳ�������visited��
	for (int k = 0; k < n; k++) {
		if (find(visited.begin(), visited.end(), k) == visited.end()) { // ����k�Ƿ���visited�����
			I.push_back(k); //��δ�ҵ���k��������ʳ�����
			P.push_back(0.0); // ��ʼ������ 
		}
	}
}

inline double
calculate_pk(vector<double>& P, vector<int>& I, vector<int> &visited) {
	double Psum = 0.0;
	for (int k = 0; k < P.size(); k++) { //����ȥ��һ�����еĸ��� 
		P[k] = pow(Tau[visited.back()][I[k]], Alpha) * pow(ethea[visited.back()][I[k]], beta); // p_xy^k�ϰ벿��
		Psum += P[k];
	}
	return Psum;
}

inline int
get_next_city(double const Psum,vector<double>&P, vector<int> &I) {
	double rate = rnd(0.0, Psum),choose =0.0; // �漴һ������ 
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
			L[i] += D[Tabu[i][j]][Tabu[i][j + 1]];
		}
		L[i] += D[Tabu[i][0]][Tabu[i][n - 1]];
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
			Tau[i][j] = (1 - rho) * Tau[i][j] + deltatau[i][j];	//������Ϣ�ػӷ������º����Ϣ��
		}
	}
}

inline void
find_min_path_and_draw(int iter) {
	double min_L = L_best[0];			//���е�������̾���
	int min_L_index = 0;				//���е���������·�����±�
	int Shortest_Route[n];				//���е����е�����·��
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
	cout << "The Shortest route is�� " << endl << "start";

	for (int i = 0; i < n; i++)		//���е����е�����·��
	{
		Shortest_Route[i] = R_best[min_L_index][i];
		cout << " -> " << Shortest_Route[i];
	}
}
//��Ⱥ�㷨
inline void
ant_colony_optimization(Graph& graph) {
	//��һ�����г�ʼ�� 
	initialize(graph);
	clock_t start, end, total;
	int iter = 0;
	total = 0;
	while (iter < Iter_max && total<=900) {
		cout << "total time is: " << total << endl;
		start = clock();
		for (int i = 0; i < ant; i++) Tabu[i][0] = 0; // ����0Ϊ��ʼ����
		for (int j = 1; j < n; j++) {
			for (int i = 0; i < ant; i++) {
				vector<int> visited, I;  //��iֻ���ϴ����ʵĳ���
				vector<double> P; //��iֻ���ϴ����ʳ��еĸ��� 
				double Psum = 0.0; //����ֵ��
				int to_visit; // ��һ��Ҫȥ�ĳ��� 
				separate_visited_not_visited(i, j, visited, I, P);
				to_visit = rand() % I.size();
				Psum = calculate_pk(P, I, visited);
				int nc = get_next_city(Psum, P, I);
				Tabu[i][j] = (nc == -1) ? to_visit : nc;
			}
		}

		//���Ĳ� ��¼·��
		vector<double> L;
		calculate_path(L);
		find_min_path(iter, L);
		cout << iter << ": L_best is " << L_best[iter] << ' ' << "L_ave is " << L_avg[iter] << endl;
		iter++;
		update_tau(L);
		for (int i = 0; i < ant; i++)			//���ɱ�����
			for (int j = 0; j < n; j++)
				Tabu[i][j] = 0;
		end = clock();
		total += (end-start)/CLOCKS_PER_SEC;
	}
		find_min_path_and_draw(iter);
}

int SLS_ACO(Graph& graph) {
	initialize(graph);
	ant_colony_optimization(graph);
	return 0;
}