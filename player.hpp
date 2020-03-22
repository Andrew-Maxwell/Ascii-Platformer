#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "bullet.hpp"
#include "world.hpp"
#include "effects.hpp"
#include "entity.hpp"
#include "meta.hpp"
#include "particles.hpp"
#include "physicalentities.hpp"

using namespace rapidjson;

/******************************************************************************/
//PlayerEntity
//what it sounds like, I guess.
/******************************************************************************/

class player : protected physicalEntity, virtual public collideable {

    int health, maxHealth, hurtTimer = 0;
    int air, maxAir;

    bool gunUnlocked[16] = {false};
    int gunAmmos[16] {0};
    int gunMaxAmmos[16] = {0};
    int gunCoolDowns[16] = {0};
    int gunDisplays[16] = {'E'};
    string gunDisplayChars[16] = {""};
    Color gunColors[16] = {{255, 0, 255, 255}};
    Color gunColorsFaded[16] = {{127, 0, 127, 127}};
    int lastCollisionType;

    int gunSelect = 0;

    //Used in bit manipulation

    int ops[16][4] = {{4}, {0}};
    int args[16][4] = {{0}, {0}};
    int opCount = 1;
    bitset<8> channels[10];

    bool pickUpsCollected[512] = { false };
    bool spawned = false;

    Vector2 positionOnScreen = {0, 0};

    public:

    string nextRoom;
    bool shouldChangeRooms;

    int won = 0;

    //Constructor + save and load functions

    explicit player(  float newX, float newY, Color newTint, float newSizeFactor);

    unsigned int type();

    void spawn(float spawnX, float spawnY);

    bool save(string fileName);

    bool load(string fileName);
    
    //Access whether pickup is collected. Saved here so that it can go in the same savefile.
    
    bool isCollected(int pickUpID);

    //Special accessors because playerEntity must read data from save file as well

    void setColor(Color newTint);

    void setSizeFactor(float newSizeFactor);

    float getSizeFactor();

    Vector2 getPos();

    //Collision functions

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision(float otherX, float otherY, int otherType);

    bool stopColliding();

    //Tick functions

    void tickSet();

    void tickGet();

    bool finalize();

    void print();

    //Draw the HUD

    void drawHud();

    //Draw a GUI inventory/byte editor screen

    void drawTabScreen();

};

#endif //PLAYER_HPP
