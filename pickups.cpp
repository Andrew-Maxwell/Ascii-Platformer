#include "pickups.hpp"

/*****************************************************************************/
//pickUp
//Any sort of item that can be collected by the player once
/*****************************************************************************/

    pickUp::pickUp(  float newX, float newY,  Color newTint,
                    float newSizeFactor,  int newDisplayChar, int newLifetime, int newID, bool newTouch) :
        entity(newX, newY, newTint, newSizeFactor),
        displayChar(newDisplayChar),
        lifetime(newLifetime),
        ID(newID),
        touch(newTouch) {
            toPrint = TextToUtf8(&displayChar, 1);
        }

    pickUp::~pickUp() {
        free(toPrint);
    }

    bool pickUp::doesCollide(float otherX, float otherY, int otherType) {
        if (!collected && (IsKeyPressed(KEY_S) || touch) && otherX > x - 1 && otherX < x + 1 && otherY > y - 1 && otherY < y + 1 && otherType == PLAYERTYPE) {
            collected = true;
            return true;
        }
        return false;
    }

    collision pickUp::getCollision(float otherX, float otherY, int otherType) {
        return collision(type());
    }

    bool pickUp::stopColliding() {
        return collected || lifetime < 0;
    }

    void pickUp::tickSet() {
	    lifetime--;
    }

    void pickUp::tickGet() {
        if (collected && !exploded) {
            exploded = true;
            explode(16, x, y, tint, sizeFactor, 0.3, '*', 100, 0.5);
        }
    }

    bool pickUp::finalize() {
        return (collected || lifetime < 0);
    }

    void pickUp::print() {
        if (!collected) {
            theCanvas -> draw(x, y, tint, sizeFactor, toPrint);
        }
    }

/*****************************************************************************/
//gunPickUp
//Unlocks a new gun
/*****************************************************************************/

    gunPickUp::gunPickUp(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newGunID) :
                        entity(newX, newY, newTint, newSizeFactor),
                        pickUp(newX, newY, newTint, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        gunID(newGunID) {}

    unsigned int gunPickUp::type() {
        return GUNPICKUPTYPE;
    }

    collision gunPickUp::getCollision(float otherX, float otherY, int otherType) {
        return collision(GUNPICKUPTYPE, gunID, 0.0, ID);
    }

/*****************************************************************************/
//ammoPickUp
//Adds ammo to a given gun
/*****************************************************************************/

    ammoPickUp::ammoPickUp(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newGunID, int newAmmoCount) :
                        entity(newX, newY, newTint, newSizeFactor),
                        pickUp(newX, newY, newTint, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        gunID(newGunID),
                        ammoCount(newAmmoCount) {}

    unsigned int ammoPickUp::type() {
        return AMMOPICKUPTYPE;
    }

    collision ammoPickUp::getCollision(float otherX, float otherY, int otherType) {
        return collision(AMMOPICKUPTYPE, gunID, ammoCount, ID);      //ammoCount in xVal
    }

/*****************************************************************************/
//healthPickUp
//Adds health back
/*****************************************************************************/

    healthPickUp::healthPickUp(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newHealthCount) :
                        entity(newX, newY, newTint, newSizeFactor),
                        pickUp(newX, newY, newTint, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        healthCount(newHealthCount) {}

    unsigned int healthPickUp::type() {
        return HEALTHPICKUPTYPE;
    }

    collision healthPickUp::getCollision(float otherX, float otherY, int otherType) {
        return collision(HEALTHPICKUPTYPE, healthCount, 0.0, ID);
    }

/*****************************************************************************/
//maxHealthPickUp
//Adds to max health
/*****************************************************************************/

    maxHealthPickUp::maxHealthPickUp(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newHealthCount) :
                        entity(newX, newY, newTint, newSizeFactor),
                        pickUp(newX, newY, newTint, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        healthCount(newHealthCount) {}

    unsigned int maxHealthPickUp::type() {
        return MAXHEALTHPICKUPTYPE;
    }

    collision maxHealthPickUp::getCollision(float otherX, float otherY, int otherType) {
        return collision(MAXHEALTHPICKUPTYPE, healthCount, 0.0, ID);
    }

/*****************************************************************************/
//airPickUp
//Adds air back
/*****************************************************************************/

    airPickUp::airPickUp(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newAirCount) :
                        entity(newX, newY, newTint, newSizeFactor),
                        pickUp(newX, newY, newTint, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        airCount(newAirCount) {}

    unsigned int airPickUp::type() {
        return AIRPICKUPTYPE;
    }

    collision airPickUp::getCollision(float otherX, float otherY, int otherType) {
        return collision(AIRPICKUPTYPE, airCount, 0.0, ID);
    }

/*****************************************************************************/
//maxairPickUp
//Adds to max air
/*****************************************************************************/

    maxAirPickUp::maxAirPickUp(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newAirCount) :
                        entity(newX, newY, newTint, newSizeFactor),
                        pickUp(newX, newY, newTint, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        airCount(newAirCount) {}

    unsigned int maxAirPickUp::type() {
        return MAXAIRPICKUPTYPE;
    }

    collision maxAirPickUp::getCollision(float otherX, float otherY, int otherType) {
        return collision(MAXAIRPICKUPTYPE, airCount, 0.0, ID);
    }


/*****************************************************************************/
//Op pickup
//Gives the player another bitwise op to play with
/*****************************************************************************/

    opPickUp::opPickUp(float newX, float newY, Color newTint, float newSizeFactor,  int newDisplayChar, int newLifeTime, int newID, bool newTouch, string newMessage) :
                        entity(newX, newY, newTint, newSizeFactor),
                        pickUp(newX, newY, newTint, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        message(newMessage) {}

    unsigned int opPickUp::type() {
        return OPPICKUPTYPE;
    }

    collision opPickUp::getCollision(float otherX, float otherY, int otherType) {
        return collision(OPPICKUPTYPE, 0, 0.0, ID, message);
    }
