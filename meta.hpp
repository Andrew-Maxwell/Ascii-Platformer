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

#define SCREENWIDTH 1600
#define SCREENHEIGHT 900
#define FONTSIZE 16
#define SCREENROWS SCREENHEIGHT / FONTSIZE
#define SCREENCOLS SCREENWIDTH / FONTSIZE

#define NUMCHARS 875
#define FONTNAME "PxPlus_IBM_BIOS2.ttf"

#define CAMERALAGX SCREENCOLS * 3 / 16
#define CAMERALAGY SCREENROWS * 3 / 16

#define PLAYERSPEED 0.015
#define PLAYERMAXSPEED 0.7
#define JUMPSPEED 0.35
#define JPMAX 6
#define PARTICLELIFESPAN 240
#define COLLIDERTILESIZE 10
#define GRAVITY 0.01
#define FRICTION 0.95

#define UIBACKGROUND {15, 15, 30, 255}
#define UIFOREGROUND {255, 240, 200, 255}
#define HEALTHCOLOR {255, 0, 0, 255}

extern "C" {
    extern int FONTCHARS[];// = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,318,319,320,321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,402,417,439,506,507,508,509,510,511,536,537,538,539,593,632,710,711,713,728,729,730,731,732,733,894,900,901,902,903,904,905,906,908,910,911,912,913,914,915,916,917,918,919,920,921,922,923,924,925,926,927,928,929,931,932,933,934,935,936,937,938,939,940,941,942,943,944,945,946,947,948,949,950,951,952,953,954,955,956,957,958,959,960,961,962,963,964,965,966,967,968,969,970,971,972,973,974,976,1012,1024,1025,1026,1027,1028,1029,1030,1031,1032,1033,1034,1035,1036,1037,1038,1039,1040,1041,1042,1043,1044,1045,1046,1047,1048,1049,1050,1051,1052,1053,1054,1055,1056,1057,1058,1059,1060,1061,1062,1063,1064,1065,1066,1067,1068,1069,1070,1071,1072,1073,1074,1075,1076,1077,1078,1079,1080,1081,1082,1083,1084,1085,1086,1087,1088,1089,1090,1091,1092,1093,1094,1095,1096,1097,1098,1099,1100,1101,1102,1103,1104,1105,1106,1107,1108,1109,1110,1111,1112,1113,1114,1115,1116,1117,1118,1119,1168,1169,1470,1488,1489,1490,1491,1492,1493,1494,1495,1496,1497,1498,1499,1500,1501,1502,1503,1504,1505,1506,1507,1508,1509,1510,1511,1512,1513,1514,1520,1521,1522,1523,1524,7451,7462,7464,7808,7809,7810,7811,7812,7813,7839,7922,7923,8208,8210,8211,8212,8213,8215,8216,8217,8218,8219,8220,8221,8222,8223,8224,8225,8226,8230,8231,8240,8242,8243,8245,8249,8250,8252,8254,8255,8256,8260,8276,8308,8309,8310,8311,8312,8313,8314,8315,8319,8321,8322,8323,8324,8325,8326,8327,8328,8329,8330,8331,8355,8356,8359,8362,8364,8453,8467,8470,8482,8486,8494,8528,8529,8531,8532,8533,8534,8535,8536,8537,8538,8539,8540,8541,8542,8592,8593,8594,8595,8596,8597,8616,8706,8709,8710,8712,8719,8721,8722,8725,8729,8730,8734,8735,8745,8747,8776,8800,8801,8804,8805,8857,8960,8962,8976,8992,8993,9472,9474,9484,9488,9492,9496,9500,9508,9516,9524,9532,9552,9553,9554,9555,9556,9557,9558,9559,9560,9561,9562,9563,9564,9565,9566,9567,9568,9569,9570,9571,9572,9573,9574,9575,9576,9577,9578,9579,9580,9600,9601,9602,9603,9604,9605,9606,9607,9608,9612,9616,9617,9618,9619,9632,9633,9642,9643,9644,9650,9658,9660,9668,9670,9674,9675,9679,9688,9689,9698,9699,9700,9701,9702,9786,9787,9788,9792,9794,9824,9827,9829,9830,9834,9835,10003,64257,64258,65533};
}

using namespace std;

//Forward declarations, not sure why this fixes things but it does.

class playerEntity;
class collider;


/******************************************************************************/
//Raylib functions I optimized to run faster in my use case
/******************************************************************************/

int myGetGlyphIndex(int codepoint);

void myDrawTexture(Texture2D texture, Rectangle sourceRec, Rectangle destRec, Vector2 origin, float rotation, Color tint);

void myDrawText(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint);

/******************************************************************************/
//Virtual entity parent class.
/******************************************************************************/

class entity {

    protected:

    Color tint;
    float sizeFactor = 1;

    public:

    int type;
    int id;
    float x;
    float y;

    //Constructor and destructor

    entity(float newx, float newy, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor);

    entity() {}

    virtual ~entity() = default;

    //Tick functions
    //Move entities. Update collider

    virtual void tickSet(collider& col) {cerr << "Entity tickset called illegally. This function should be overridden!";}

    //Update entities based on new collider values

    virtual void tickGet(collider& col) {cerr << "Entity tickGet() called illegally. This function should be overridden!";}

    //perform additional cleanup. If this function returns true, then the entity is deleted by the entityList.

    virtual bool finalize() {cerr << "Entity finalize() called illegally. This function should be overridden!";}

    //Display the entity.

    virtual void print(float cameraX, float cameraY, Font displayFont) {cerr << "Entity print() called illegally. This function should be overridden!";}
};

/******************************************************************************/
//A list for containing entities. Every tick, every entity in the list has its four functions called
//once. Also, entities can contain entityLists (e.g. an entity containing the bullets it's fired) as
//long as the four functions are called by that entity's four functions.
/******************************************************************************/

class entityList {

    list<entity*> entities;

    public:

    ~entityList();

    void clear(); //Has the potential to cause big issues with collideables. should ONLY be called in one place - by player when changing rooms.

    int size();

    void tickSet(collider& col);

    void tickGet(collider& col);

    void finalize();

    void print(float cameraX, float cameraY, Font displayFont);

    void addEntity(entity* toAdd);
};

/******************************************************************************/
//layer
//Static multi-character entities read in from file.
//Designed to be the visual part of the level.
/******************************************************************************/

class layer : virtual public entity {

    protected:

    vector<string> canvas;
    string fileName;

    public:

    explicit layer( float newx, float newy, uint8_t R, uint8_t G,
                    uint8_t B, uint8_t A, float newSizeFactor,
                    string newFileName);

    void readLayer();

    int getRows();

    int getCols();

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};

#endif //META_HPP
