#include <vector>
#include <limits>
#include <stack>
#include "Graph.h"
#include "BnB.h"

using namespace std;

float UB, cost;
stack<int> path, best_path;

bool allVisited(vector<bool> &visited)
{
    for (int i = 0; i < visited.size(); i++)
    {
        if (!visited[i])
            return false;
    }
    return true;
}

float h(int node, Graph &graph)
{
    float max = numeric_limits<int>::max();
    for (int i = 0; i < graph.get_city_num(); i++)
    {
        if (i != node)
        {
            float dis = graph.get_dis(node, i);
            if (dis < max)
                max = dis;
        }
    }
    return max;
}

void BnBHelper(int start, int cur, vector<bool> &visited, Graph &graph)
{
    if (cur == start && allVisited(visited))
    {
        if (cost < UB)
        {
            UB = cost;
            best_path = path;
        }
        return;
    }
    for (int i = 0; i < graph.get_city_num(); i++)
    {
        if (visited[i])
            continue;
        if (cost + h(cur, graph) < UB)
        {
            path.push(i);
            visited[i] = true;
            cost += graph.get_dis(cur, i);
            BnBHelper(start, i, visited, graph);
            cost -= graph.get_dis(cur, i);
            visited[i] = false;
            path.pop();
        }
    }
}

float BnBDFS(Graph &graph)
{
    UB = numeric_limits<float>::max();
    cost = 0;
    vector<bool> visited(graph.get_city_num(), false);
    BnBHelper(0, 0, visited, graph);
    return UB;
}