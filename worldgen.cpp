#include <iostream>
#include <fstream>

using namespace std;

int main() {
    fstream worldFile;
    worldFile.open("world.txt");
    int size = 1000;
    worldFile << "0 0\n";
    for(int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            worldFile << "#";
        }
        worldFile << endl;
    }
    worldFile.close();
}
