#include "forcefield.hpp"

/*****************************************************************************/
//forceField
//Attracts or repels physical entities within its influence
/*****************************************************************************/

    forceField::forceField(float newX, float newY, Color newTint, float newSizeFactor,  int newChannel, float newPower, float newRange) :
        entity(newX, newY, newTint, newSizeFactor),
        channel(newChannel),
        power(newPower),
        isOn(false),
        range(newRange) {
            nextCollision.type = FORCEFIELDTYPE;
        }

    unsigned int forceField::type() {
        return FORCEFIELDTYPE;
    }

    bool forceField::doesCollide(float otherX, float otherY, int otherType) {
        if (!isOn) {
            return false;
        }
        if (otherType == WATERTYPE) {
            return true;
        }
        if (pow(pow(x - otherX, 2) + pow(y - otherY, 2), 0.5) > range) {
            return false;
        }
        return true;
    }

    collision forceField::getCollision(float otherX, float otherY, int otherType) {
        if (otherType == WATERTYPE) {
            return collision(FORCEFIELDTYPE, range, x, y, "", power);
        }
        else {
            nextCollision.xVal = copysign(pow(pow(power, 2) / (1 + pow((y - otherY) / (x - otherX), 2)), 0.5), x - otherX) * copysign(1, power);
            nextCollision.yVal = copysign(pow(pow(power, 2) / (1 + pow((x - otherX) / (y - otherY), 2)), 0.5), y - otherY) * copysign(1, power);
            return nextCollision;
        }
    }

    bool forceField::stopColliding() {
        return false;
    }

    void forceField::tickSet() {
    }

    void forceField::tickGet() {
        if (world -> getChannel(channel)) {
            isOn = true;
            if (tickCount++ % (int)(1 / power) == 0) {
                if (power > 0) { //Attractor force field
                    for (float angle = 0; angle < 2 * M_PI; angle += (2 * M_PI / 50)) {
                        world -> addEntity(new particle(x + cos(angle) * range, y + sin(angle) * range, tint, sizeFactor, cos(angle) * power * -40, sin(angle) * power * -40, 0, range / power / 40));
                    }
                }
                else {  //repeller force field
                    for (float angle = 0; angle < 2 * M_PI; angle += (2 * M_PI / 50)) {
                        world -> addEntity(new particle(x, y, tint, sizeFactor, cos(angle) * power * 40, sin(angle) * power * 40, 0, range / abs(power) / 40));
                    }
                }
            }
        }
        else {
            isOn = false;
            tickCount = -1;
        }
    }

    bool forceField::finalize() {
        return false;
    }

    void forceField::print() {
        theCanvas -> draw(x, y, tint, sizeFactor, "F");
    }

/*****************************************************************************/
//explosion 
//Attracts or repels physical entities for one tick, then disappears.
/*****************************************************************************/

    explosion::explosion(float newX, float newY, Color newTint, float newSizeFactor,  int newChannel, float newPower, float newRange) :
        entity(newX, newY, newTint, newSizeFactor),
        forceField(newX, newY, newTint, newSizeFactor, newChannel, newPower, newRange)
        {
            isOn = true;
        }

    bool explosion::doesCollide(float otherX, float otherY, int otherType) {
        return exploding && forceField::doesCollide(otherX, otherY, otherType);
    }

    bool explosion::stopColliding() {
        return exploding;
    }

    void explosion::tickSet() {
        exploding = true;    //Delete the explosion after its first full tick.
    }

    void explosion::tickGet() {}

    bool explosion::finalize() {
        return exploding;
    }

    void explosion::print() {}


