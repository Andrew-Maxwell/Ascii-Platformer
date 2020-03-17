#include "meta.hpp"

/******************************************************************************/
//getLevelIFStream() and getLevelFileP()
//Checks multiple locations for filename and sets string correctly so that
//the file can be written back to later.
/******************************************************************************/

ifstream getLevelIFStream(string& fileName) {
    ifstream levelFile;
    levelFile.open(fileName);
    if (!levelFile) {
        string attempt2 = string("levels/").append(fileName);
        levelFile.open(attempt2);
        if (!levelFile) {
            string attempt3 = string("levels\\").append(fileName);
            levelFile.open(attempt3);
            if (!levelFile) {
                cerr << "Could not open " << fileName << endl;
                exit(EXIT_FAILURE);
            }
            else {
                fileName = attempt3;
            }
        }
        else {
            fileName = attempt2;
        }
    }
    return levelFile;
}

FILE* getLevelFileP(string& fileName) {
    FILE* levelFile = NULL;
    levelFile = fopen(fileName.c_str(), "rb");
    if (!levelFile) {
        string attempt2 = string("levels/").append(fileName);
        levelFile = fopen(attempt2.c_str(), "rb");
        if (!levelFile) {
            string attempt3 = string("levels\\").append(fileName);
            levelFile = fopen(attempt3.c_str(), "rb");
            if (!levelFile) {
                cerr << "Could not open " << fileName << endl;
                exit(EXIT_FAILURE);
            }
            else {
                fileName = attempt3;
            }
        }
        else {
            fileName = attempt2;
        }
    }
    return levelFile;
}

