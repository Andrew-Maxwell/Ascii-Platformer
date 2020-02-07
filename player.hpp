#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "bullet.hpp"
#include "col.hpp"
#include "effects.hpp"
#include "entitymeta.hpp"
#include "meta.hpp"
#include "particles.hpp"

using namespace rapidjson;

/******************************************************************************/
//PlayerEntity
//what it sounds like, I guess.
/******************************************************************************/

class player : public realPhysicalEntity, virtual public collideable {

    int health, maxHealth, hurtTimer = 0;

    bool gunUnlocked[16] = {false};
    int gunAmmos[16] {0};
    int gunMaxAmmos[16] = {0};
    int gunCoolDowns[16] = {0};
    int gunDisplayChars[16] = {'E'};
    Color gunColors[16] = {{255, 0, 255, 255}};
    Color gunColorsFaded[16] = {{127, 0, 127, 127}};

    int gunSelect = 0;

    //Used in bit manipulation

    int ops[16][4] = {{4}, {0}};
    int args[16][4] = {{0}, {0}};
    int opCount = 1;
    bitset<8> channels[10];

    bool pickUpsCollected[512] = { false };

    entityList localEntities;
    int lastMovedX, lastMovedY;
    Vector2 positionOnScreen = {0, 0};

    public:

    string nextRoom;
    bool shouldChangeRooms;

    int won = 0;

    //Constructor + save and load functions

    explicit player(  float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, string newNextRoom);

    bool save(string fileName);

    bool load(string fileName);
    
    //Access whether pickup is collected. Saved here so that it can go in the same savefile.
    
    bool isCollected(int pickUpID);

    //Special accessors because playerEntity must read data from save file as well

    void setColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A);

    void setSizeFactor(float newSizeFactor);

    float getSizeFactor();

    //Collision functions

    bool doesCollide(float otherX, float otherY, int type);

    collision getCollision();

    bool stopColliding();

    //Tick functions

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);

    //Draw the HUD

    void drawHUD(Font displayFont);

    //Draw a GUI inventory/byte editor screen

    void drawTabScreen(Font displayFont);

};

#endif //PLAYER_HPP
