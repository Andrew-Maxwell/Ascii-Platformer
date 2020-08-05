#include "endinggate.hpp"



/*****************************************************************************/
//endingGate
//an imposing gate that spawns particles that float slowly upwards.
/*****************************************************************************/

    endingGate::endingGate(float newX, float newY, Color newTint,
                        float newScale,  int newWidth = 0, int newHeight = 0) :
                        entity(newX, newY, newTint, newScale) {
        toPrint = {"O----O", "|    |", "|    |", "|    |", "O----O"};
        width = 5;
        height = 5;
    }

    unsigned int endingGate::type() {
        return ENDINGGATETYPE;
    }

    bool endingGate::doesCollide(float otherX, float otherY, int otherType, unsigned int otherID) {
        return (otherX > x && otherX < x + width && otherY > y && otherY < y + height && otherType == PLAYERTYPE);
    }

    collision endingGate::getCollision(float otherX, float otherY, int otherType, unsigned int otherID) {
        return collision(type(), id, 0, 0, 0, "");
    }

    bool endingGate::stopColliding() {
        return false;
    }

    void endingGate::tickSet() {
        if (GetRandomValue(0, 10) == 0) {
            world -> addEntity(new particle( x + GetRandomValue(0, 10 * width) / 10.0, y + height - 1.5, tint, scale, 0, -0.01, '*', (height - 1.5) * 100), zPosition);
        }
    }

    void endingGate::tickGet() {
    }

    bool endingGate::finalize() {
        return false;
    }

    void endingGate::print() {
        for (int i = 0; i < height; i++) {
            theScreen -> draw(x, y + i, tint, scale, toPrint[i], doLighting);
        }
    }

