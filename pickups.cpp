#include "pickups.hpp"

/*****************************************************************************/
//pickUp
//Any sort of item that can be collected by the player once
/*****************************************************************************/

    pickUp::pickUp(  float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                    float newSizeFactor, int newDisplayChar, int newLifetime, int newID, bool newTouch) :
        entity(newX, newY, R, G, B, A, newSizeFactor),
        displayChar(newDisplayChar),
        lifetime(newLifetime),
        ID(newID),
        touch(newTouch) {}

    bool pickUp::doesCollide(float otherX, float otherY, int otherType) {
        if (!collected && (IsKeyPressed(KEY_S) || touch) && otherX > x - 1 && otherX < x + 1 && otherY > y - 1 && otherY < y + 1 && otherType == 1) {
            collected = true;
            return true;
        }
        return false;
    }

    collision pickUp::getCollision() {
        return collision(type);
    }

    bool pickUp::stopColliding() {
        return collected || lifetime < 0;
    }

    void pickUp::tickSet(collider& col) {
	lifetime--;
        myParticles.tickSet(col);
    }

    void pickUp::tickGet(collider& col) {
        if (collected && !exploded) {
            exploded = true;
            explosion(col, myParticles, 16, x, y, tint, sizeFactor, 0.3, '*', 100, 0.5);
        }

        myParticles.tickGet(col);
    }

    bool pickUp::finalize() {
        myParticles.finalize();
        return (collected && myParticles.size() == 0) || (!(collected) && lifetime < 0);
    }

    void pickUp::print(float cameraX, float cameraY, Font displayFont) {
        myParticles.print(cameraX, cameraY, displayFont);
        if (!collected) {
            myDrawText(displayFont, TextToUtf8(&displayChar, 1), (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
        }
    }

/*****************************************************************************/
//gunPickUp
//Unlocks a new gun
/*****************************************************************************/

    gunPickUp::gunPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newGunID) :
                        entity(newX, newY, R, G, B, A, newSizeFactor),
                        pickUp(newX, newY, R, G, B, A, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        gunID(newGunID) {}

    collision gunPickUp::getCollision() {
        return collision(7, gunID, 0.0, ID);
    }

/*****************************************************************************/
//ammoPickUp
//Adds ammo to a given gun
/*****************************************************************************/

    ammoPickUp::ammoPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newGunID, int newAmmoCount) :
                        entity(newX, newY, R, G, B, A, newSizeFactor),
                        pickUp(newX, newY, R, G, B, A, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        gunID(newGunID),
                        ammoCount(newAmmoCount) {}

    collision ammoPickUp::getCollision() {
        return collision(8, gunID, ammoCount, ID);      //ammoCount in xVal
    }

/*****************************************************************************/
//healthPickUp
//Adds health back
/*****************************************************************************/

    healthPickUp::healthPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newHealthCount) :
                        entity(newX, newY, R, G, B, A, newSizeFactor),
                        pickUp(newX, newY, R, G, B, A, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        healthCount(newHealthCount) {}

    collision healthPickUp::getCollision() {
        return collision(9, healthCount, 0.0, ID);
    }

/*****************************************************************************/
//maxHealthPickUp
//Adds to max health
/*****************************************************************************/

    maxHealthPickUp::maxHealthPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifeTime, int newID, bool newTouch, int newHealthCount) :
                        entity(newX, newY, R, G, B, A, newSizeFactor),
                        pickUp(newX, newY, R, G, B, A, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        healthCount(newHealthCount) {}

    collision maxHealthPickUp::getCollision() {
        return collision(10, healthCount, 0.0, ID);
    }

/*****************************************************************************/
//Op pickup
//Gives the player another bitwise op to play with
/*****************************************************************************/

    opPickUp::opPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifeTime, int newID, bool newTouch, string newMessage) :
                        entity(newX, newY, R, G, B, A, newSizeFactor),
                        pickUp(newX, newY, R, G, B, A, newSizeFactor, newDisplayChar, newLifeTime, newID, newTouch),
                        message(newMessage) {}

    collision opPickUp::getCollision() {
        return collision(11, 0, 0.0, ID, message);
    }
