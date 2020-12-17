#include <vector>
#include "Graph.h"

using namespace std;

Graph::Graph(int n)
{
    this->n = n;
    for (int i = 0; i < n; i++) {
        vector<float> cur(n, 0);
        distance.push_back(cur);
    }
}

void Graph::set_dis(int i, int j, float dis) {
    if (i == j) return;
        this->distance[i][j] = dis;
}

float Graph::get_dis(int i, int j) {
    if (i == j) return 0.0;
    else
        return this->distance[i][j];
}

int Graph::get_city_num() {
    return this->n;
}