#include "endinggate.hpp"



/*****************************************************************************/
//endingGate
//an imposing gate that spawns particles that float slowly upwards.
/*****************************************************************************/

    endingGate::endingGate(float newX, float newY, Color newTint,
                        float newSizeFactor, entityList* newEList, int newWidth = 0, int newHeight = 0) :
                        entity(newX, newY, newTint, newSizeFactor) {
        toPrint = {"O----O", "|    |", "|    |", "|    |", "O----O"};
        width = 5;
        height = 5;
        type = ENDINGGATETYPE;
        eList = newEList;
    }

    bool endingGate::doesCollide(float otherX, float otherY, int otherType) {
        return (otherX > x && otherX < x + width && otherY > y && otherY < y + height && otherType == PLAYERTYPE);
    }

    collision endingGate::getCollision(float otherX, float otherY, int otherType) {
        return collision(type, 0, 0, 0, "");
    }

    bool endingGate::stopColliding() {
        return false;
    }

    void endingGate::tickSet(collider& col) {
        if (GetRandomValue(0, 10) == 0) {
            eList -> addEntity(new particle( x + GetRandomValue(0, 10 * width) / 10.0, y + height - 1.5, tint, sizeFactor, 0, -0.01, '*', (height - 1.5) * 100));
        }
    }

    void endingGate::tickGet(collider& col) {
    }

    bool endingGate::finalize() {
        return false;
    }

    void endingGate::print(float cameraX, float cameraY, Font displayFont) {
        for (int i = 0; i < height; i++) {
            myDrawText(displayFont, toPrint[i].c_str(), (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y + i) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 0, tint);
        }
    }

