#include "savepoint.hpp"

/*****************************************************************************/
//SavePoint
//Saves the game when interacted with.
/*****************************************************************************/

    savePoint::savePoint(float newX, float newY, Color newTint, float newScale) :
        entity(newX, newY, newTint, newScale) {}

    unsigned int savePoint::type() {
        return SAVEPOINTTYPE;
    }

    bool savePoint::doesCollide(float otherX, float otherY, int otherType, unsigned int otherID) {
        return gotCollision && otherType == PLAYERTYPE && (otherX >= x - 1 && otherX <= x + 1 && otherY >= y - 1 && otherY <= y + 1);
    }

    collision savePoint::getCollision(float otherX, float otherY, int otherType, unsigned int otherID) {
        return collision(type(), id);
    }

    bool savePoint::stopColliding() {
        return false;
    }

    void savePoint::tickSet() {}

    void savePoint::tickGet() {
        gotCollision = false;
        for (auto colIter = collisions.begin(); colIter != collisions.end(); colIter++) {
            if (colIter -> type == PLAYERTYPE && colIter -> damage == 2) {  //Player is interacting
                gotCollision = true;
                explode(16, x, y, tint, scale, 0.3, '*', 100, 0.5, zPosition);
            }
        }
        collisions.clear();
    }

    bool savePoint::finalize() {
        return false;
    }

    void savePoint::print() {
        theScreen -> draw(x, y, tint, scale, "S", doLighting, doHighlight);
    }
