#ifndef PICKUPS_HPP
#define PICKUPS_HPP

#include "world.hpp"
#include "effects.hpp"
#include "entity.hpp"
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
    
    char* toPrint;

    public:

    int displayChar;

    explicit pickUp(  float newX, float newY,  Color newTint,
                    float newSizeFactor,  int newDisplayChar, int newLifetime, int newID, bool newTouch);

    ~pickUp();

    bool doesCollide(float otherX, float otherY, int otherType);

    virtual collision getCollision(float otherX, float otherY, int otherType);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

/*****************************************************************************/
//Gun pickup
//Unlocks a gun
/*****************************************************************************/

class gunPickUp : public pickUp {

    int gunID;

    public:

    explicit gunPickUp(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType);

};

/*****************************************************************************/
//Ammo pickup
//Adds ammo to a gun
/*****************************************************************************/

class ammoPickUp : public pickUp {

    int gunID;
    int ammoCount;

    public:

    explicit ammoPickUp(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID, int newAmmoCount);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType);

};


/*****************************************************************************/
//healthPickUp
//Adds health back
/*****************************************************************************/

class healthPickUp : public pickUp {

    int healthCount;

    public:

    explicit healthPickUp(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType);

};


/*****************************************************************************/
//max health pickup
//Increases max health
/*****************************************************************************/

class maxHealthPickUp : public pickUp {

    int healthCount;

    public:

    explicit maxHealthPickUp(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType);

};

/*****************************************************************************/
//airPickUp
//Adds air back
/*****************************************************************************/

class airPickUp : public pickUp {

    int airCount;

    public:

    explicit airPickUp(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType);

};


/*****************************************************************************/
//max air pickup
//Increases max air
/*****************************************************************************/

class maxAirPickUp : public pickUp {

    int airCount;

    public:

    explicit maxAirPickUp(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType);

};


/*****************************************************************************/
//Op pickup
//Gives the player another bitwise op to play with
/*****************************************************************************/

class opPickUp : public pickUp {

    string message;

    public:

    explicit opPickUp(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifeTime, int newID, bool newTouch, string newMessage);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType);

};

#endif //PICKUPS_HPP
