#include <vector>
#include <limits>
#include "Graph.h"

using namespace std;

void BnBHelper(int start, int cur, int UB, vector<bool> visited) {

}

int BnBDFS(Graph graph) {
    int UB = numeric_limits<float>::max();
    vector<bool> visited(graph.get_city_num(), false);
    BnBHelper(0, 0, UB, visited);
    return UB;
}