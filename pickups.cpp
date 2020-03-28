#include "pickups.hpp"

/*****************************************************************************/
//pickup
//Any sort of item that can be collected by the player once
/*****************************************************************************/

    pickup::pickup(  float newX, float newY,  Color newTint,
                    float newSizeFactor,  int newDisplayChar, int newLifetime, int newID, bool newTouch) :
        entity(newX, newY, newTint, newSizeFactor),
        displayChar(newDisplayChar),
        lifetime(newLifetime),
        ID(newID),
        touch(newTouch) {
            toPrint = TextToUtf8(&displayChar, 1);
        }

    pickup::~pickup() {
        free(toPrint);
    }

    bool pickup::doesCollide(float otherX, float otherY, int otherType) {
        if (!collected && (IsKeyPressed(KEY_S) || touch) && otherX > x - 1 && otherX < x + 1 && otherY > y - 1 && otherY < y + 1 && otherType == PLAYERTYPE) {
            collected = true;
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
    }

    void pickup::tickGet() {
        if (collected && !exploded) {
            exploded = true;
            explode(16, x, y, tint, sizeFactor, 0.3, '*', 100, 0.5);
        }
    }

    bool pickup::finalize() {
        return (collected || lifetime < 0);
    }

    void pickup::print() {
        if (!collected) {
            theCanvas -> draw(x, y, tint, sizeFactor, toPrint);
        }
    }

/*****************************************************************************/
//gunPickup
//Unlocks a new gun
/*****************************************************************************/

    gunPickup::gunPickup(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newGunID) :
                        entity(newX, newY, newTint, newSizeFactor),
                        pickup(newX, newY, newTint, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        gunID(newGunID) {}

    unsigned int gunPickup::type() {
        return GUNPICKUPTYPE;
    }

    collision gunPickup::getCollision(float otherX, float otherY, int otherType) {
        return collision(GUNPICKUPTYPE, gunID, 0.0, ID);
    }

/*****************************************************************************/
//ammoPickup
//Adds ammo to a given gun
/*****************************************************************************/

    ammoPickup::ammoPickup(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newGunID, int newAmmoCount) :
                        entity(newX, newY, newTint, newSizeFactor),
                        pickup(newX, newY, newTint, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        gunID(newGunID),
                        ammoCount(newAmmoCount) {}

    unsigned int ammoPickup::type() {
        return AMMOPICKUPTYPE;
    }

    collision ammoPickup::getCollision(float otherX, float otherY, int otherType) {
        return collision(AMMOPICKUPTYPE, gunID, ammoCount, ID);      //ammoCount in xVal
    }

/*****************************************************************************/
//healthPickup
//Adds health back
/*****************************************************************************/

    healthPickup::healthPickup(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newHealthCount) :
                        entity(newX, newY, newTint, newSizeFactor),
                        pickup(newX, newY, newTint, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        healthCount(newHealthCount) {}

    unsigned int healthPickup::type() {
        return HEALTHPICKUPTYPE;
    }

    collision healthPickup::getCollision(float otherX, float otherY, int otherType) {
        return collision(HEALTHPICKUPTYPE, healthCount, 0.0, ID);
    }

/*****************************************************************************/
//maxHealthPickup
//Adds to max health
/*****************************************************************************/

    maxHealthPickup::maxHealthPickup(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newHealthCount) :
                        entity(newX, newY, newTint, newSizeFactor),
                        pickup(newX, newY, newTint, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        healthCount(newHealthCount) {}

    unsigned int maxHealthPickup::type() {
        return MAXHEALTHPICKUPTYPE;
    }

    collision maxHealthPickup::getCollision(float otherX, float otherY, int otherType) {
        return collision(MAXHEALTHPICKUPTYPE, healthCount, 0.0, ID);
    }

/*****************************************************************************/
//airPickup
//Adds air back
/*****************************************************************************/

    airPickup::airPickup(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newAirCount) :
                        entity(newX, newY, newTint, newSizeFactor),
                        pickup(newX, newY, newTint, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        airCount(newAirCount) {}

    unsigned int airPickup::type() {
        return AIRPICKUPTYPE;
    }

    collision airPickup::getCollision(float otherX, float otherY, int otherType) {
        return collision(AIRPICKUPTYPE, airCount, 0.0, ID);
    }

/*****************************************************************************/
//maxairPickup
//Adds to max air
/*****************************************************************************/

    maxAirPickup::maxAirPickup(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newAirCount) :
                        entity(newX, newY, newTint, newSizeFactor),
                        pickup(newX, newY, newTint, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        airCount(newAirCount) {}

    unsigned int maxAirPickup::type() {
        return MAXAIRPICKUPTYPE;
    }

    collision maxAirPickup::getCollision(float otherX, float otherY, int otherType) {
        return collision(MAXAIRPICKUPTYPE, airCount, 0.0, ID);
    }


/*****************************************************************************/
//Op pickup
//Gives the player another bitwise op to play with
/*****************************************************************************/

    opPickup::opPickup(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newOpID) :
                        entity(newX, newY, newTint, newSizeFactor),
                        pickup(newX, newY, newTint, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        opID(newOpID) {}

    unsigned int opPickup::type() {
        return OPPICKUPTYPE;
    }

    collision opPickup::getCollision(float otherX, float otherY, int otherType) {
        return collision(OPPICKUPTYPE, opID, 0.0, ID);
    }
