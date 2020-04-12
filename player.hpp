#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "bullet.hpp"
#include "world.hpp"
#include "effects.hpp"
#include "entity.hpp"
#include "meta.hpp"
#include "particles.hpp"
#include "physicalentities.hpp"
#include <set>

using namespace rapidjson;

struct weapon {
    bool unlocked = false;
    bool automatic;
    bool hitWall, hitWater, hitEntity;
    bool sticky;
    float explosionRange, explosionPower;
    int lifetime;
    float speed;
    int damage;
    float gravity;
    float elasticity;
    int gunID;
    int gunType;
    int ammo = 0;
    int maxAmmo = 0;
    int lastFired = 0;
    int cooldown = 60;
    int bulletDisplay;
    int particleCount;
    string display = "";
    Color tint, tintFaded;

    bool operator== (const weapon& other) {
        return gunID == other.gunID;
    }
};

struct puzzleOp {
    bool unlocked = false;
    int opID;
    string display = "";
    vector<uint8_t> operations;
    vector<uint8_t> operands;

    bool operator== (const puzzleOp& other) {
        return opID == other.opID;
    }
};

struct outfit {
    string name;
    int display;
    int health = 8;
    int maxHealth;
    int air;
    int maxAir;

    float elasticity, gravity, jumpSpeed;
    float groundSpeed, airSpeed, waterSpeed, groundFriction, airFriction, waterFriction;
    int jumpCount;
    bool autoRejump; //Creates a jetpack-like effect
    bool walljump;

    vector<weapon> guns;

    vector<puzzleOp> ops;
    bitset<8> channels[10];

    set<int> collectedPickups;

    void merge(outfit& otherOutfit) {
        for (int i = 0; i < otherOutfit.guns.size(); i++) {
            if (find(guns.begin(), guns.end(), otherOutfit.guns[i]) == guns.end()) {
                guns.push_back(otherOutfit.guns[i]);
            }
        }
        for (int i = 0; i < otherOutfit.ops.size(); i++) {
            if (find(ops.begin(), ops.end(), otherOutfit.ops[i]) == ops.end()) {
                ops.push_back(otherOutfit.ops[i]);
            }
        }
    }
};

/******************************************************************************/
//PlayerEntity
//what it sounds like, I guess.
/******************************************************************************/

class player : protected physicalEntity, virtual public collideable {

    string outfitName;
    int display;
    string displayStr;

    int health, maxHealth;
    int hurtTimer = -1000;
    int air, maxAir;

    float groundSpeed, airSpeed, waterSpeed, groundFriction, airFriction, waterFriction;
    float jumpSpeed;
    int jumpCount;
    int jumpsUsed;
    bool autoRejump; //Creates a jetpack-like effect
    bool walljump;
    bool justJumped = false, jumpControl;

    vector<weapon> guns;
    int gunSelect = 0;

    //Used in bit manipulation

    vector<puzzleOp> ops;
    bitset<8> channels[10];

    set<int> collectedPickups;

    public:

    string nextRoom;
    bool breakDoor = false, breakSave = false, breakDead = false;

    int won = 0;

    //Constructor

    explicit player (float newX, float newY, Color newTint, float newSizeFactor);

    unsigned int type ();

    //Outfit handling functions

    outfit getCurrentOutfit();

    void setOutfit(outfit newOutfit);

    //Accessors used for save/load system

    void moveTo(Vector2 position);

    Vector2 getPosition();

    set<int> getCollectedPickups();

    //Special accessors because playerEntity must read data from save file as well

    void setColor(Color newTint);

    void setSizeFactor(float newSizeFactor);

    float getSizeFactor();

    Vector2 getPos();

    //Collision functions

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision(float otherX, float otherY, int otherType);

    bool stopColliding();

    //Fire the gun

    void fire(weapon& gun);

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
