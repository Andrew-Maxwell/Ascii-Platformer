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
    float acceleration, speed, friction;
    float waterAcceleration, waterSpeed, waterFriction;
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

class player : virtual public collideable, public entityParent, public hudEntity {

    //Formerly part of the physicalEntity inheritance
    bool hit = false;
    bool isUnderWater = false, lastTickUnderWater = false;
    float elasticity = 0, maxSpeed = 100, gravity, friction;
    float width = 0.8;

    float xMovement, yMovement;
    float xMoveWater, yMoveWater;
    float xInertia, yInertia;

    string outfitName;
    int display;
    string displayStr;

    int health, maxHealth;
    int hurtTimer = -1000;
    int air, maxAir;

    float acceleration, speed, playerFriction;
    float waterAcceleration, waterSpeed, waterFriction;
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
    Color channelColors[10];
    int lastChannel = 0;

    set<int> collectedPickups;

    public:

    string nextRoom;
    bool breakDoor = false, breakSave = false, breakDead = false;

    int won = 0;

    //Constructor

    explicit player (float newX, float newY, Color newTint, float newSizeFactor);

    unsigned int type ();

    //In accessor file: Outfit handling + save/load functions

        outfit getCurrentOutfit();

        void setOutfit(outfit newOutfit);

        void moveTo(Vector2 position);

        Vector2 getPosition();

        set<int> getCollectedPickups();

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

    //In collision file: tickGet() helper functions

        void handleCollision(collision& col);

        void handlePickup(collision& col);

    bool finalize();

    void print();

    //In HUD file

        void printHud();

        void drawTabScreen();

};

#endif //PLAYER_HPP
