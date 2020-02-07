#ifndef PICKUPS_HPP
#define PICKUPS_HPP

#include "col.hpp"
#include "effects.hpp"
#include "entitymeta.hpp"
#include "meta.hpp"
#include "particles.hpp"

using namespace rapidjson;

/*****************************************************************************/
//pickUp
//Any sort of collectable bonus
/*****************************************************************************/

class pickUp : public collideable {

    protected:

    bool collected = false;
    bool exploded = false;
    bool touch;
    int lifetime, ID;
    entityList myParticles;

    public:

    int displayChar;

    explicit pickUp(  float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                    float newSizeFactor, int newDisplayChar, int newLifetime, int newID, bool newTouch);

    bool doesCollide(float otherX, float otherY, int otherType);

    virtual collision getCollision();

    bool stopColliding();

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};

/*****************************************************************************/
//Gun pickup
//Unlocks a gun
/*****************************************************************************/

class gunPickUp : public pickUp {

    int gunID;

    public:

    explicit gunPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID);

    collision getCollision();

};

/*****************************************************************************/
//Ammo pickup
//Adds ammo to a gun
/*****************************************************************************/

class ammoPickUp : public pickUp {

    int gunID;
    int ammoCount;

    public:

    explicit ammoPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID, int newAmmoCount);

    collision getCollision();

};


/*****************************************************************************/
//healthPickUp
//Adds health back
/*****************************************************************************/

class healthPickUp : public pickUp {

    int healthCount;

    public:

    explicit healthPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID);

    collision getCollision();

};


/*****************************************************************************/
//max health pickup
//Increases max health
/*****************************************************************************/

class maxHealthPickUp : public pickUp {

    int healthCount;

    public:

    explicit maxHealthPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID);

    collision getCollision();

};


/*****************************************************************************/
//Op pickup
//Gives the player another bitwise op to play with
/*****************************************************************************/

class opPickUp : public pickUp {

    string message;

    public:

    explicit opPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifeTime, int newID, bool newTouch, string newMessage);

    collision getCollision();

};

#endif //PICKUPS_HPP
