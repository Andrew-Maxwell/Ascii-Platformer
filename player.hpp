#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "bullet.hpp"
#include "world.hpp"
#include "effects.hpp"
#include "entity.hpp"
#include "meta.hpp"
#include "particles.hpp"
#include "physicalentities.hpp"
#include "input.hpp"
#include <set>

using namespace rapidjson;

struct playerConfig {
    Color tint;
    inputMap in;
    int playerNo = -1;
    int configNo = -1;
};

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

    void merge(weapon& other) {
        assert(gunID == other.gunID);
        unlocked |= other.unlocked;
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

    void merge(puzzleOp& otherOp) {
        assert(opID == otherOp.opID);
        unlocked |= otherOp.unlocked;
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

    set<uint8_t> interceptedCodes;
    vector<puzzleOp> ops;
    bitset<8> channels[10];
    Color channelColors[10];

    void merge(outfit& otherOutfit) {
        if (name != otherOutfit.name) {
            cout << "WARNING: Merging outfits with different names\n";
        }
        for (int i = 0; i < otherOutfit.guns.size(); i++) {
            vector<weapon>::iterator foundGun = find(guns.begin(), guns.end(), otherOutfit.guns[i]);
            if (foundGun == guns.end()) {
                guns.push_back(otherOutfit.guns[i]);
            }
            else {
                foundGun -> merge(otherOutfit.guns[i]);
            }
        }
        for (int i = 0; i < otherOutfit.ops.size(); i++) {
            vector<puzzleOp>::iterator foundOp = find(ops.begin(), ops.end(), otherOutfit.ops[i]);
            if (foundOp == ops.end()) {
                ops.push_back(otherOutfit.ops[i]);
            }
            else {
                foundOp -> merge(otherOutfit.ops[i]);
            }
        }
    }
};

/******************************************************************************/
//PlayerEntity
//what it sounds like, I guess.
/******************************************************************************/

class player : virtual public collideable, public entityParent, public hudEntity {

    inputMap in;

    //Formerly part of the physicalEntity inheritance
    bool onGround, onWall;
    bool hit = false;
    bool isUnderWater = false, lastTickUnderWater = false;
    float elasticity = 0, maxSpeed = 100, gravity, friction, maxFallSpeed = 0.3f;
    float width = 1, height = 1;

    float xMovement = 0, yMovement = 0;
    float pushedX = 0, pushedY = 0;
    float xInertia = 0, yInertia = 0;

    float nextX = 0, nextY = 0;

    string outfitName = "";
    int display = '?';
    string displayStr = "?";

    int health = 1000000, maxHealth = 100000;
    int hurtTimer = -1000;
    int air = 1, maxAir = 1;

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

    set<uint8_t> interceptedCodes;
    vector<puzzleOp> ops;
    bitset<8> channels[10];
    Color channelColors[10];
    int lastChannel = 0;

    public:

    int playerNo = 0, outfitNo = 0, configNo = 0;
    string nextRoom;
    bool breakDoor = false, breakSave = false, breakDead = false, breakInventory = false, focusCamera;

    int won = 0;

    //Constructor

    explicit player (float newX, float newY, Color newTint, float newScale);

    unsigned int type ();

    //In accessor file: Outfit handling + save/load functions

        void setInputMap(inputMap newMap);

        inputMap& getInputMap();

        outfit getCurrentOutfit();

        void setOutfit(outfit newOutfit);

        void moveTo(Vector2 position);

        Vector2 getPosition();

        void setColor(Color newTint);

        void setSizeFactor(float newScale);

        float getSizeFactor();

        void goToDoor();

        string getDoorDestination();

        bool isMoving();

    //Collision functions

    bool doesCollide(float otherX, float otherY, int otherType, unsigned int otherID);

    collision getCollision(float otherX, float otherY, int otherType, unsigned int otherID);

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
