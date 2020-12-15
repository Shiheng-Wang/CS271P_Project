#include <vector>
#include "BnB.h"

using namespace std;

Graph::Graph(int n)
{
    this->n = n;
    for (int i = 0; i < n; i++) {
        vector<float> cur(n, 0);
        distance.push_back(cur);
    }
}

void Graph::set_dis(int i, int j, int dis) {
    if (i == j) return;
    else if (i < j)
        distance[i][j] = dis;
    else
        distance[j][i] = dis;
}

float Graph::get_dis(int i, int j) {
    if (i == j) return 0.0;
    else if (i < j)
        return distance[i][j];
    else
        return distance[j][i];
}

int Graph::get_city_num() {
    return this->n;
}