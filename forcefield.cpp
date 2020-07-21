#include "forcefield.hpp"


/*****************************************************************************/
//linearField
//propels physical entities in a given direction
/*****************************************************************************/

    linearField::linearField(float newX, float newY, Color newTint, Color newOnTint, float newSizeFactor,  int newChannel, float newXPower, float newYPower, int newWidth, int newHeight) :
        entity(newX, newY, newTint, newSizeFactor),
        channel(newChannel),
        xPower(newXPower),
        yPower(newYPower),
        width(newWidth),
        height(newHeight),
        onTint(newOnTint)
    {
        int arrowChar;
        if (xPower > 0) {
            if (yPower > 0) {
                arrowChar = 0x2198;
            }
            else if (yPower < 0) {
                arrowChar = 0x2197;
            }
            else {
                arrowChar = 0x2192;
            }
        }
        else if (xPower < 0) {
            if (yPower > 0) {
                arrowChar = 0x2199;
            }
            else if (yPower < 0) {
                arrowChar = 0x2196;
            }
            else {
                arrowChar = 0x2190;
            }
        }
        else { //xPower == 0
            if (yPower > 0) {
                arrowChar = 0x2193;
            }
            else if (yPower < 0) {
                arrowChar = 0x2191;
            }
            else {
                arrowChar = '?';
            }
        }
        char* arrowUnicode = TextToUtf8(&arrowChar, 1);
        tex = LoadRenderTexture(width * 8, height * 8);
        BeginTextureMode(tex);
        ClearBackground((Color){0, 0, 0, 0});
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                theScreen -> myDrawText(arrowUnicode, (Vector2){j * 8, i * 8}, 8, 0, (Color){255, 255, 255, 255});
            }
        }
        EndTextureMode();
        free(arrowUnicode);
    }

    linearField::~linearField() {
        UnloadRenderTexture(tex);
    }

    unsigned int linearField::type() {
        return FORCEFIELDTYPE;
    }

    bool linearField::doesCollide(float otherX, float otherY, int otherType, unsigned int otherID) {
        return (isOn && otherX >= x && otherX <= x + width && otherY >= y && otherY <= y + height && otherType != WATERTYPE);
    }

    collision linearField::getCollision(float otherX, float otherY, int otherType, unsigned int otherID) {
        return collision(type(), id, 0, xPower, yPower);
    }

    bool linearField::stopColliding() {
        return false;
    }

    void linearField::tickSet() {}

    void linearField::tickGet() {
        isOn = world -> getChannel(channel);
    }

    bool linearField::finalize() {
        return false;
    }

    void linearField::print() {
        if (isOn) {
            theScreen -> drawLayer(x, y, onTint, sizeFactor, tex.texture, false);
        }
        else {
            theScreen -> drawLayer(x, y, tint, sizeFactor, tex.texture, doLighting);
        }
    }

/*****************************************************************************/
//forceField
//Attracts or repels physical entities within its influence
/*****************************************************************************/

    forceField::forceField(float newX, float newY, Color newTint, Color newOnTint, float newSizeFactor,  int newChannel, float newPower, float newRange) :
        entity(newX, newY, newTint, newSizeFactor),
        channel(newChannel),
        power(newPower),
        isOn(false),
        range(newRange),
        onTint(newOnTint) {
            nextCollision.type = FORCEFIELDTYPE;
            nextCollision.id = id;
        }

    unsigned int forceField::type() {
        return FORCEFIELDTYPE;
    }

    bool forceField::doesCollide(float otherX, float otherY, int otherType, unsigned int otherID) {
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

    collision forceField::getCollision(float otherX, float otherY, int otherType, unsigned int otherID) {
        if (otherType == WATERTYPE) {
            return collision(FORCEFIELDTYPE, id, range, x, y, "", power);
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
            float angle = GetRandomValue(0, 2 * PI * 1000) / 1000.0;
            particle* p;
            if (power > 0) { //Attractor force field
                p = new particle(x + cos(angle) * range, y + sin(angle) * range, onTint, sizeFactor, cos(angle) * power * -40, sin(angle) * power * -40, 0, range / power / 40);
            }
            else {  //repeller force field
                p = new particle(x, y, onTint, sizeFactor, cos(angle) * power * -40, sin(angle) * power * -40, 0, range / power / -40);
            }
            p -> setDoLighting(false);
            world -> addEntity(p);
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
        if (isOn) {
            theScreen -> draw(x, y, onTint, sizeFactor, "F", false);
        }
        else {
            theScreen -> draw(x, y, tint, sizeFactor, "F", doLighting);
        }
    }

/*****************************************************************************/
//explosion 
//Attracts or repels physical entities for one tick, then disappears.
/*****************************************************************************/

    explosion::explosion(float newX, float newY, Color newTint, float newSizeFactor,  int newChannel, float newPower, float newRange) :
        entity(newX, newY, newTint, newSizeFactor),
        forceField(newX, newY, newTint, newTint, newSizeFactor, newChannel, newPower, newRange)
        {
            isOn = true;
        }

    bool explosion::doesCollide(float otherX, float otherY, int otherType, unsigned int otherID) {
        return exploding && forceField::doesCollide(otherX, otherY, otherType, otherID);
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


