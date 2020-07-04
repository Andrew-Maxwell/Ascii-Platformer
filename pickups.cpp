#include "pickups.hpp"

/*****************************************************************************/
//pickup
//Any sort of item that can be collected by the player once
/*****************************************************************************/

    pickup::pickup(  float newX, float newY, Color newTint,
                    float newSizeFactor, set<int>* newCollectedPickups, int newDisplayChar, int newLifetime, int newID, bool newTouch) :
        entity(newX, newY, newTint, newSizeFactor),
        collectedPickups(newCollectedPickups),
        displayChar(newDisplayChar),
        lifetime(newLifetime),
        pickupID(newID),
        touch(newTouch) {
            toPrint = TextToUtf8(&displayChar, 1);
        }

    pickup::~pickup() {
        free(toPrint);
    }

    bool pickup::doesCollide(float otherX, float otherY, int otherType) {
        if (!collected && otherX > x - 1 && otherX < x + 1 && otherY > y - 1 && otherY < y + 1 && otherType == PLAYERTYPE) {
            collected = true;
            collectedPickups -> insert(pickupID);
            return true;
        }
        return false;
    }

    collision pickup::getCollision(float otherX, float otherY, int otherType) {
        return collision(type());
    }

    bool pickup::stopColliding() {
        return collected || lifetime < 0;
    }

    void pickup::tickSet() {
	    lifetime--;
        if (!collected && collectedPickups -> count(pickupID)) {    //If another pickup with same ID has been collected
            collected = exploded = true;
        }
    }

    void pickup::tickGet() {
        if (collected && !exploded) {
            exploded = true;
            explode(16, x, y, tint, sizeFactor, 0.3, '*', 100, 0.5, zPosition);
        }
    }

    bool pickup::finalize() {
        return (collected || lifetime < 0);
    }

    void pickup::print() {
        if (!collected) {
            theScreen -> draw(x, y, tint, sizeFactor, toPrint, doLighting);
        }
    }


/*****************************************************************************/
//outfitPickup
//Modifies any value according to the name used in the save JSON
/*****************************************************************************/

    outfitPickup::outfitPickup(float newX, float newY, Color newTint, float newSizeFactor, set<int>* newCollectedPickups, int newDisplayChar, int newLifetime, int newID, bool newTouch, string newKey, double newValue, bool newAdd) :
                        entity(newX, newY, newTint, newSizeFactor),
                        pickup(newX, newY, newTint, newSizeFactor, newCollectedPickups, newDisplayChar, newLifetime, newID, newTouch),
                        key(newKey),
                        value(newValue),
                        add (newAdd) {}

    unsigned int outfitPickup::type() {
        return OUTFITPICKUPTYPE;
    }

    collision outfitPickup::getCollision(float otherX, float otherY, int otherType) {
        return collision(OUTFITPICKUPTYPE, add ? 1 : 0, 0.0, pickupID, key, value);
    }

/*****************************************************************************/
//gunPickup
//Unlocks a new gun
/*****************************************************************************/

    gunPickup::gunPickup(float newX, float newY, Color newTint, float newSizeFactor, set<int>* newCollectedPickups, int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newGunID) :
                        entity(newX, newY, newTint, newSizeFactor),
                        pickup(newX, newY, newTint, newSizeFactor, newCollectedPickups, newDisplayChar, newLifeTime, newID, newTouch),
                        gunID(newGunID) {}

    unsigned int gunPickup::type() {
        return GUNPICKUPTYPE;
    }

    collision gunPickup::getCollision(float otherX, float otherY, int otherType) {
        return collision(GUNPICKUPTYPE, gunID, 0.0, pickupID);
    }

/*****************************************************************************/
//ammoPickup
//Adds ammo to a given gun
/*****************************************************************************/

    ammoPickup::ammoPickup(float newX, float newY, Color newTint, float newSizeFactor, set<int>* newCollectedPickups, int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newGunID, int newAmmoCount) :
                        entity(newX, newY, newTint, newSizeFactor),
                        pickup(newX, newY, newTint, newSizeFactor, newCollectedPickups, newDisplayChar, newLifeTime, newID, newTouch),
                        gunID(newGunID),
                        ammoCount(newAmmoCount) {}

    unsigned int ammoPickup::type() {
        return AMMOPICKUPTYPE;
    }

    collision ammoPickup::getCollision(float otherX, float otherY, int otherType) {
        return collision(AMMOPICKUPTYPE, gunID, ammoCount, pickupID);      //ammoCount in xVal
    }

/*****************************************************************************/
//Op pickup
//Gives the player another bitwise op to play with
/*****************************************************************************/

    opPickup::opPickup(float newX, float newY, Color newTint, float newSizeFactor, set<int>* newCollectedPickups, int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newOpID) :
                        entity(newX, newY, newTint, newSizeFactor),
                        pickup(newX, newY, newTint, newSizeFactor, newCollectedPickups, newDisplayChar, newLifeTime, newID, newTouch),
                        opID(newOpID) {}

    unsigned int opPickup::type() {
        return OPPICKUPTYPE;
    }

    collision opPickup::getCollision(float otherX, float otherY, int otherType) {
        return collision(OPPICKUPTYPE, opID, 0.0, pickupID);
    }
