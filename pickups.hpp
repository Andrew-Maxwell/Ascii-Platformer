#ifndef PICKUPS_HPP
#define PICKUPS_HPP

#include "world.hpp"
#include "effects.hpp"
#include "entity.hpp"
#include "meta.hpp"
#include "particles.hpp"
#include <set>

using namespace rapidjson;

/*****************************************************************************/
//pickup parent class
//Any sort of collectable bonus
/*****************************************************************************/

class pickup : public collideable, public entityParent {

    protected:

    bool gotCollision = false;
    bool collected = false;
    bool exploded = false;
    bool touch;
    int lifetime, pickupID;
    set<int>* collectedPickups;
    string toPrint;
    int playerID = 0;

    public:

    int displayChar;

    explicit pickup(float newX, float newY, Color newTint,
                    float newScale, set<int>* newCollectedPickups, int newDisplayChar, int newLifetime, int newID, bool newTouch);


    bool doesCollide(float otherX, float otherY, int otherType, unsigned int otherID);

    virtual collision getCollision(float otherX, float otherY, int otherType, unsigned int otherID);

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

    explicit outfitPickup(float newX, float newY, Color newTint, float newScale, set<int>* newCollectedPickups, int newDisplayChar, int newLifetime, int newID, bool newTouch, string newKey, double newValue, bool add);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType, unsigned int otherID);

};

/*****************************************************************************/
//Gun pickup
//Unlocks a gun
/*****************************************************************************/

class gunPickup : public pickup {

    int gunID;

    public:

    explicit gunPickup(float newX, float newY, Color newTint, float newScale, set<int>* newCollectedPickups,  int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType, unsigned int otherID);

};

/*****************************************************************************/
//Ammo pickup
//Adds ammo to a gun
/*****************************************************************************/

class ammoPickup : public pickup {

    int gunID;
    int ammoCount;

    public:

    explicit ammoPickup(float newX, float newY, Color newTint, float newScale, set<int>* newCollectedPickups,  int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID, int newAmmoCount);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType, unsigned int otherID);

};

/*****************************************************************************/
//Op pickup
//Gives the player another bitwise op to play with
/*****************************************************************************/

class opPickup : public pickup {

    int opID;

    public:

    explicit opPickup(float newX, float newY, Color newTint, float newScale, set<int>* newCollectedPickups, int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newOpID);

    unsigned int type();

    collision getCollision(float otherX, float otherY, int otherType, unsigned int otherID);

};

#endif //PICKUPS_HPP
