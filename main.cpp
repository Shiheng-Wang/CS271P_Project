#include <iostream>
#include <fstream>
#include <string>
#include "Graph.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Type the test file name as the command argument\n";
        exit(0);
    }

    // Build a Graph object
    string filename = argv[1];
    ifstream myFile;
    myFile.open(filename);
    string line;
    getline(myFile, line);
    int size = stoi(line);
    Graph graph(size);
    int i = 0;
    string dis;
    while (getline(myFile, line)) {
        for (int j = 0; j < size; j++) {
            int index = line.find(' ');
            dis = line.substr(0, index);
            graph.set_dis(i, j, stoi(dis));
            line = line.substr(index + 1);
        }
        i++;
    }
    myFile.close();

    // Test BnB DFS

    // Test SLS

    return 0;
}