#include "savepoint.hpp"

/*****************************************************************************/
//SavePoint
//Saves the game when interacted with.
/*****************************************************************************/

    savePoint::savePoint(float newX, float newY, Color newTint, float newSizeFactor, entityList* newEList) :
        entity(newX, newY, newTint, newSizeFactor) {
            type = SAVEPOINTTYPE;
            eList = newEList;
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

    void savePoint::tickSet(collider& col) {}

    void savePoint::tickGet(collider& col) {
        if (savedGame) {;
            savedGame = false;
            explode(col, eList, 16, x, y, tint, sizeFactor, 0.3, '*', 100, 0.5);
        }
    }

    bool savePoint::finalize() {
        return false;
    }

    void savePoint::print(float cameraX, float cameraY, Font displayFont) {
        myDrawText(displayFont, "S", (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
    }
