#include "forcefield.hpp"

/*****************************************************************************/
//forceField
//Attracts or repels physical entities within its influence
/*****************************************************************************/

    forceField::forceField(float newX, float newY, Color newTint, float newSizeFactor, entityList* newEList, int newChannel, float newPower, float newRange) :
        entity(newX, newY, newTint, newSizeFactor),
        channel(newChannel),
        power(newPower),
        range(newRange),
        isOn(false) {
            eList = newEList;
            nextCollision.type = FORCEFIELDTYPE;
        }

    bool forceField::doesCollide(float otherX, float otherY, int otherType) {
        if (!isOn) {
            return false;
        }
        if (pow(pow(x - otherX, 2) + pow(y - otherY, 2), 0.5) > range) {
            return false;
        }
        return true;
    }

    collision forceField::getCollision(float otherX, float otherY, int otherType) {
        nextCollision.xVal = copysign(pow(pow(power, 2) / (1 + pow((y - otherY) / (x - otherX), 2)), 0.5), x - otherX) * copysign(1, power);
        nextCollision.yVal = copysign(pow(pow(power, 2) / (1 + pow((x - otherX) / (y - otherY), 2)), 0.5), y - otherY) * copysign(1, power);
        return nextCollision;
    }

    bool forceField::stopColliding() {
        return false;
    }

    void forceField::tickSet(collider& col) {
        if (col.getChannel(channel)) {
            isOn = true;
            if (++tickCount % (int)(1 / power) == 0) {
                if (power > 0) { //Attractor force field
                    for (float angle = 0; angle < 2 * M_PI; angle += (2 * M_PI / 20)) {
                        eList -> addEntity(new particle(x + cos(angle) * range, y + sin(angle) * range, tint, sizeFactor, cos(angle) * power * -40, sin(angle) * power * -40, 0, range / power / 40));
                    }
                }
                else {  //repeller force field
                    for (float angle = 0; angle < 2 * M_PI; angle += (2 * M_PI / 20)) {
                        eList -> addEntity(new particle(x, y, tint, sizeFactor, cos(angle) * power * 40, sin(angle) * power * 40, 0, range / abs(power) / 40));
                    }
                }
            }
        }
        else {
            isOn = false;
            tickCount = -1;
        }
    }

    void forceField::tickGet(collider& col) {
    }

    bool forceField::finalize() {
        return false;
    }

    void forceField::print(float cameraX, float cameraY, Font displayFont) {
        myDrawText(displayFont, "F", (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
    }
