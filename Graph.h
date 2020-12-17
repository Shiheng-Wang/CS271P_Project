#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>

using namespace std;

class Graph
{
private:
    int n;
    vector<vector<float> > distance;
public:
    Graph(int n);
    void set_dis(int i, int j, float dis); // Set the distance between i and j
    float get_dis(int i, int j);
    int get_city_num();
};

#endif