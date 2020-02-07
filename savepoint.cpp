#include "savepoint.hpp"

/*****************************************************************************/
//SavePoint
//Saves the game when interacted with.
/*****************************************************************************/

    savePoint::savePoint(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor) : entity(newX, newY, R, G, B, A, newSizeFactor) {type = 4;}

    bool savePoint::doesCollide(float otherX, float otherY, int otherType) {
        bool collided = (IsKeyPressed(KEY_S) && otherX > x - 1 && otherX < x + 1 && otherY > y - 1 && otherY < y + 1 && otherType == 1);
        savedGame |= collided;
        return collided;
    }

    collision savePoint::getCollision() {
        return collision(4);
    }

    bool savePoint::stopColliding() {
        return false;
    }

    void savePoint::tickSet(collider& col) {
        myParticles.tickSet(col);;
    }

    void savePoint::tickGet(collider& col) {
        if (savedGame) {;
            savedGame = false;
            explosion(col, myParticles, 16, x, y, tint, sizeFactor, 0.3, '*', 100, 0.5);
        }
        myParticles.tickGet(col);
    }

    bool savePoint::finalize() {
        myParticles.finalize();
        return false;
    }

    void savePoint::print(float cameraX, float cameraY, Font displayFont) {
        myDrawText(displayFont, "S", (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
        myParticles.print(cameraX, cameraY, displayFont);
    }
