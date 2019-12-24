#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define HASHSIZE 501

int main(int argc, char** argv) {
    ifstream in(argv[1]);
    int cp;
    vector<vector<int>> table(HASHSIZE, vector<int>());
    while (in >> cp) {
        table[cp % HASHSIZE].push_back(cp);
        in.ignore(100,'\n');
    }
    cout << "{\n";
    for (vector<int> row : table) {
        cout << "{";
        for (int val : row) {
            cout << val << ", ";
        }
        cout << "}\n";
    }
    cout << "}";
}
