#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include "Graph.h"
#include "BnB.h"
#include "SLS_ACO.h"

using namespace std;

int main()
{
    // Build a Graph object
    ifstream myFile;
    myFile.open("./tests/tsp-problem-25-125-100-25-1.txt");
    if (!myFile) cerr << "Could not open the file\n";
    string line;
    getline(myFile, line);
    int size = stoi(line);
    Graph graph(size);
    int i = 0;
    string dis;
    int index;
    while (getline(myFile, line))
    {
        for (int j = 0; j < size; j++)
        {
            index = line.find(' ');
            if (index == string::npos) {
                graph.set_dis(i, j, stof(dis));
            }
            else {
                dis = line.substr(0, index);
                graph.set_dis(i, j, stof(dis));
                line = line.substr(index + 1);
            }
        }
        i++;
    }
    myFile.close();

    // Test BnB DFS
    BnBDFS(graph);

    // Test SLS
    SLS_ACO(graph);

    return 0;
}