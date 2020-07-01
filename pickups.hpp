#ifndef PICKUPS_HPP
#define PICKUPS_HPP

#include "world.hpp"
#include "effects.hpp"
#include "entity.hpp"
#include "meta.hpp"
#include "particles.hpp"

using namespace rapidjson;

/*****************************************************************************/
//pickup parent class
//Any sort of collectable bonus
/*****************************************************************************/

class pickup : public collideable, public entityParent {

    protected:

    bool collected = false;
    bool exploded = false;
    bool touch;
    int lifetime, pickupID;
    
    char* toPrint;

    public:

    int displayChar;

    explicit pickup(float newX, float newY, Color newTint,
                    float newSizeFactor,  int newDisplayChar, int newLifetime, int newID, bool newTouch);

    ~pickup();

    bool doesCollide(float otherX, float otherY, int otherType);

    virtual collision getCollision(float otherX, float otherY, int otherType);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

/*****************************************************************************/
//Outfit pickup
//Modifies any variable in the outfit, by name.
/*****************************************************************************/

class outfitPickup : public pickup {

    string key;
    double value;
    bool add;

    public:

    explicit outfitPickup(float newX, float newY, Color newTint, float newSizeFactor, int newDisplayChar, int newLifetime, int newID, bool newTouch, string newKey, double newValue, bool add);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType);

};

/*****************************************************************************/
//Gun pickup
//Unlocks a gun
/*****************************************************************************/

class gunPickup : public pickup {

    int gunID;

    public:

    explicit gunPickup(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType);

};

/*****************************************************************************/
//Ammo pickup
//Adds ammo to a gun
/*****************************************************************************/

class ammoPickup : public pickup {

    int gunID;
    int ammoCount;

    public:

    explicit ammoPickup(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID, int newAmmoCount);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType);

};


/*****************************************************************************/
//healthPickup
//Adds health back
/*****************************************************************************/

class healthPickup : public pickup {

    int healthCount;

    public:

    explicit healthPickup(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType);

};


/*****************************************************************************/
//max health pickup
//Increases max health
/*****************************************************************************/

class maxHealthPickup : public pickup {

    int healthCount;

    public:

    explicit maxHealthPickup(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType);

};

/*****************************************************************************/
//airPickup
//Adds air back
/*****************************************************************************/

class airPickup : public pickup {

    int airCount;

    public:

    explicit airPickup(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType);

};


/*****************************************************************************/
//max air pickup
//Increases max air
/*****************************************************************************/

class maxAirPickup : public pickup {

    int airCount;

    public:

    explicit maxAirPickup(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType);

};


/*****************************************************************************/
//Op pickup
//Gives the player another bitwise op to play with
/*****************************************************************************/

class opPickup : public pickup {

    int opID;

    public:

    explicit opPickup(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newOpID);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType);

};

#endif //PICKUPS_HPP
