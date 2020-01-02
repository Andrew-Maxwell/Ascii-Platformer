#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    if (argc != 5) {
        cout << "Usage: [levelname] [layers] [rows] [columns]";
        exit(EXIT_FAILURE);
    }
    string levelName = argv[1];
    int layers = atoi(argv[2]);
    int rows = atoi(argv[3]);
    int cols = atoi(argv[4]);

    ofstream mainFileOut;
    mainFileOut.open(levelName + ".txt");
    mainFileOut << levelName << "_collider.txt 255 255 255 255\n";
    for (int i = 0; i < layers; i++) {
        mainFileOut << "L\t0\t0\t0\t0\t0\t255\t1\t\t" << levelName << "_layer" << (int)i << ".txt\n";
    }
    mainFileOut.close();

    for (int i = 0; i < layers; i++) {
        ofstream layerFileOut;
        layerFileOut.open(levelName + "_layer" + to_string(i) + ".txt");
        for (int rowNo = 0; rowNo < rows; rowNo++) {
            for (int colNo = 0; colNo < cols; colNo++) {
                layerFileOut << ' ';
            }
            layerFileOut << endl;
        }
        layerFileOut.close();
    }

    ofstream colliderFileOut;
    colliderFileOut.open(levelName + "_collider.txt");
    for (int rowNo = 0; rowNo < rows; rowNo++) {
        for (int colNo = 0; colNo < cols; colNo++) {
            colliderFileOut << '.';
        }
        colliderFileOut << endl;
    }
    colliderFileOut.close();
}
