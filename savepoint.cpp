#include "savepoint.hpp"

/*****************************************************************************/
//SavePoint
//Saves the game when interacted with.
/*****************************************************************************/

    savePoint::savePoint(float newX, float newY, Color newTint, float newSizeFactor) :
        entity(newX, newY, newTint, newSizeFactor) {
            type = SAVEPOINTTYPE;
            
        }

    bool savePoint::doesCollide(float otherX, float otherY, int otherType) {
        bool collided = (IsKeyPressed(KEY_S) && otherX > x - 1 && otherX < x + 1 && otherY > y - 1 && otherY < y + 1 && otherType == PLAYERTYPE);
        savedGame |= collided;
        return collided;
    }

    collision savePoint::getCollision(float otherX, float otherY, int otherType) {
        return collision(type);
    }

    bool savePoint::stopColliding() {
        return false;
    }

    void savePoint::tickSet() {}

    void savePoint::tickGet() {
        if (savedGame) {;
            savedGame = false;
            explode(16, x, y, tint, sizeFactor, 0.3, '*', 100, 0.5);
        }
    }

    bool savePoint::finalize() {
        return false;
    }

    void savePoint::print() {
        theCanvas -> draw(x, y, tint, sizeFactor, "S");
    }
