#ifndef META_HPP
#define META_HPP

#include <thread>
#include <math.h>
#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <chrono>
#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <vector>
#include <sstream>
#include <string.h>
#include <bitset>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>

#define DEBUG true
#define STATS true

//Convert to be read from a config file

#define DRAWFPS true
#define SCREENHEIGHT 900
#define SCREENWIDTH 1600

#define AIRRECOVERYRATE 15
#define PLAYERSPEED 0.015
#define PLAYERMAXSPEED 10
#define JUMPSPEED 0.25
#define SWIMSPEED 0.015
#define PARTICLELIFESPAN 240
#define COLLIDERTILESIZE 10
#define GRAVITY 0.01
#define FRICTION 0.95
#define WATERRESISTANCE 0.9

#define UIBACKGROUND {15, 15, 30, 255}
#define UIFOREGROUND {255, 240, 200, 255}
#define HEALTHCOLOR {50, 255, 150, 255}
#define HURTCOLOR {255, 0, 0, 255}
#define AIRCOLOR {255, 255, 255, 255}

#define ERRORTYPE -1
#define LAYERTYPE 0
#define PLAYERTYPE 1
#define ENDINGGATETYPE 2
#define DOORTYPE 3
#define SAVEPOINTTYPE 4
#define FORCEFIELDTYPE 5
#define BULLETTYPE 6
#define WATERTYPE 7
#define PARTICLETYPE  8
#define PHYSICALENTITYTYPE 9
#define PHYSICALPARTICLETYPE 10
#define RAINTYPE 11
#define DROPTYPE 12
#define DUMMYENTITYTYPE 13
#define GAMELAYERTYPE 14
#define ENEMYTYPE 15

#define GUNPICKUPTYPE 21
#define AMMOPICKUPTYPE 22
#define HEALTHPICKUPTYPE 23
#define MAXHEALTHPICKUPTYPE 24
#define AIRPICKUPTYPE 25
#define MAXAIRPICKUPTYPE 26
#define OPPICKUPTYPE 27

//Globally accessible stuff

class collider;
extern collider* world;
class canvas;
extern canvas* theCanvas;
extern int entityCounter;
extern int tickCounter;
extern bool debugWatch;

extern "C" {
    extern int FONTCHARS[];
}

using namespace std;

struct intVector2 {
    int x;
    int y;

    intVector2(Vector2 V) {
        x = V.x;
        y = V.y;
    }
};

float roundTo8th(float x);

/******************************************************************************/
//getLevelIFStream() and getLevelFileP()
//Checks multiple locations for filename and sets string correctly so that
//the file can be written back to later.
/******************************************************************************/

string findFile(string fileName);

ifstream getLevelIFStream(string& fileName);

FILE* getLevelFileP(string& fileName);

#endif //META_HPP
