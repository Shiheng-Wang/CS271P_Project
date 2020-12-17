#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<vector>
#include<algorithm>
#include<string>
#include<time.h>
using namespace std;

const int ant = 100; //蚂蚁的数量 后期需要调参 
const int n = 99; // 城市的数量
const int Iter_max = 1000; // 迭代次数
const double Alpha = 1; // 表征信息素重要参数, 后期需要进行调参
const double beta = 5; //表征启发式因子重要程度参数, 后期需要进行调参
const double rho = 0.1; //蒸发系数 后期需要调参 
const double Q = 100; //信息素增加强度系数 后期需要调参

double C[n][n];
double D[n][n]; //图的临接矩阵
double ethea[n][n]; // 启发式因子, 这里用的1/dxy 
double deltatau[n][n]; //启发式因子的变化量
double Tau[n][n]; //路径上信息素的浓度
int Tabu[ant][n]; //禁忌表 存储走过的路径 

double L_best[Iter_max]; //存储每次迭代的最短路径长度
double L_avg[Iter_max]; //存储每次迭代的平均路径长度
int R_best[Iter_max][n]; // 存储每次迭代的最佳路径 

inline void
input_file(const string &textfile) {
	ifstream file;
	string line;
	size_t pos = 0;
	string delimiter = " ";
	vector<string> token;
	file.open(textfile);
	if (!file) cout << "Unable to open file." << endl;
	int line_number = 0;
	while (getline(file, line)) {
		while ((pos = line.find(delimiter)) != string::npos) {
			token.push_back(line.substr(0, pos));
			line.erase(0, pos + delimiter.length());
		}
		for (int i = 0; i < token.size(); i++) C[line_number][i] = stod(token[i]);
		line_number++;
		token.clear();
	}
}

inline void
initialize() {
	input_file("Text.txt");
	for (int i = 0; i < n; i++)			//初始化 D[n][n]
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

	//初始化 ethea[n][n] 
	for (int i = 0; i < n; i++)		
		for (int j = 0; j < n; j++) {
			ethea[i][j] = 1.0 / D[i][j];
		}

	//初始化禁忌表 Tabu
	for (int i=0;i<ant;i++)
		for (int j=0;j<n;j++)
			Tabu[i][j] = 0; //所有禁忌表为0
}

//随即函数 生成lower和uper之间的一个double类型随机数
inline double
rnd(double lower, double upper) { return (rand() / (double)RAND_MAX * (upper - lower) + lower); }

inline void
separate_visited_not_visited(int i, int j, vector<int> &visited, vector<int> &I, vector<double> &P) {
	for (int k = 0; k < j; k++) visited.push_back(Tabu[i][k]); // 把已经去过的城市推入visited中
	for (int k = 0; k < n; k++) {
		if (find(visited.begin(), visited.end(), k) == visited.end()) { // 查找k是否在visited里出现
			I.push_back(k); //把未找到的k存入待访问城市中
			P.push_back(0.0); // 初始化概率 
		}
	}
}

inline double
calculate_pk(vector<double>& P, vector<int>& I, vector<int> &visited) {
	double Psum = 0.0;
	for (int k = 0; k < P.size(); k++) { //计算去下一个城市的概率 
		P[k] = pow(Tau[visited.back()][I[k]], Alpha) * pow(ethea[visited.back()][I[k]], beta); // p_xy^k上半部分
		Psum += P[k];
	}
	return Psum;
}

inline int
get_next_city(double const Psum,vector<double>&P, vector<int> &I) {
	double rate = rnd(0.0, Psum),choose =0.0; // 随即一个概率 
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
			Tau[i][j] = (1 - rho) * Tau[i][j] + deltatau[i][j];	//考虑信息素挥发，更新后的信息素
		}
	}
}

inline void
find_min_path_and_draw(int iter) {
	double min_L = L_best[0];			//所有迭代中最短距离
	int min_L_index = 0;				//所有迭代中最优路径的下标
	int Shortest_Route[n];				//所有迭代中的最优路径
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
//蚁群算法
inline void
ant_colony_optimization() {
	//第一步进行初始化 
	initialize();
	clock_t start, end, total;
	int iter = 0;
	total = 0;
	while (iter < Iter_max && total<=900) {
		cout << "total time is: " << total << endl;
		start = clock();
		for (int i = 0; i < ant; i++) Tabu[i][0] = 0; // 假设0为初始城市
		for (int j = 1; j < n; j++) {
			for (int i = 0; i < ant; i++) {
				vector<int> visited, I;  //第i只蚂蚁待访问的城市
				vector<double> P; //第i只蚂蚁代访问城市的概率 
				double Psum = 0.0; //概率值合
				int to_visit; // 下一个要去的城市 
				separate_visited_not_visited(i, j, visited, I, P);
				to_visit = rand() % I.size();
				Psum = calculate_pk(P, I, visited);
				int nc = get_next_city(Psum, P, I);
				Tabu[i][j] = (nc == -1) ? to_visit : nc;
			}
		}

		//第四步 记录路线
		vector<double> L;
		calculate_path(L);
		find_min_path(iter, L);
		cout << iter << ": L_best is " << L_best[iter] << ' ' << "L_ave is " << L_avg[iter] << endl;
		iter++;
		update_tau(L);
		for (int i = 0; i < ant; i++)			//禁忌表清零
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