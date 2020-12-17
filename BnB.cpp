#include <iostream>
#include <vector>
#include <limits>
#include <stack>
#include <random>
#include "Graph.h"
#include "BnB.h"

using namespace std;

float UB, cost;
stack<int> path, best_path;
clock_t stime;
clock_t cur_duration;

void printPath(stack<int> s)
{
    while (!s.empty())
    {
        if (s.size() == 1)
            cout << s.top() << "\n";
        else
            cout << s.top() << " <- ";
        s.pop();
    }
}

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
    float min = numeric_limits<float>::max();
    for (int i = 0; i < graph.get_city_num(); i++)
    {
        if (i != node && graph.get_dis(node, i) < min)
            min = graph.get_dis(node, i);
    }
    return min;
}

void BnBHelper(int start, int cur, vector<bool> &visited, Graph &graph)
{
    if (cur == start)
    {
        if (allVisited(visited) && cost < UB)
        {
            UB = cost;
            best_path = path;
        }
        return;
    }

    int index = rand() % graph.get_city_num();
    while (visited[index] || index == cur)
        index = rand() % graph.get_city_num();

    if (cost + h(cur, graph) < UB)
    {
        path.push(index);
        visited[index] = true;
        cost += graph.get_dis(cur, index);
        BnBHelper(start, index, visited, graph);
        cost -= graph.get_dis(cur, index);
        visited[index] = false;
        path.pop();
    }
}

float BnBDFS(Graph &graph)
{
    UB = numeric_limits<float>::max();
    cost = 0;
    stime = clock();
    vector<bool> visited(graph.get_city_num(), false);
    path.push(0);
    int index;
    while (true)
    {
        cur_duration = clock() - stime;
        if ((float)cur_duration / CLOCKS_PER_SEC > 900)
            break;
        index = rand() % graph.get_city_num();
        visited[index] = true;
        cost += graph.get_dis(0, index);
        path.push(index);
        BnBHelper(0, index, visited, graph);
        cost -= graph.get_dis(0, index);
        path.pop();
        visited[index] = false;
    }
    cout << "Final UB = " << UB << endl;
    cout << "Fianl best_path = ";
    printPath(best_path);
    return UB;
}