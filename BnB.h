#ifndef BNB_H
#define BNB_H

#include <vector>
#include "Graph.h"
using namespace std;

bool allVisited(vector<bool> &visited);

float h(int node, Graph &graph);

void BnBHelper(int start, int cur, vector<bool> &visited, Graph &graph);

float BnBDFS(Graph &graph);

#endif