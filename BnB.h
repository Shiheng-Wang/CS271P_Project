#ifndef BNB_H
#define BNB_H

#include <vector>
#include <stack>
#include "Graph.h"
using namespace std;

void printPath(stack<int> s);

bool allVisited(vector<bool> &visited);

float h(int node, Graph &graph);

void BnBHelper(int start, int cur, vector<bool> &visited, Graph &graph);

float BnBDFS(Graph &graph);

#endif
