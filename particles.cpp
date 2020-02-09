#include "particles.hpp"


/******************************************************************************/
//Particle
//Entity represented by any char that moves in a predefined direction until its lifetime runs out.
//If the character passed to constructor is 0, then a character is chosen based on direction.
/******************************************************************************/

    particle::particle(  float newX, float newY, Color newTint,
                        float newSizeFactor, float newXSpeed, float newYSpeed, int c, int newLifetime) :
                        entity(newX, newY, newTint, newSizeFactor),
                        xSpeed(newXSpeed),
                        ySpeed(newYSpeed),
                        lifetime(newLifetime) {
        if (c == 0) {
            setDirection();
        }
        else {
            toPrint = c;
        }
    }

    void particle::setDirection() {
        if (xSpeed == 0) {
            toPrint = '|';
        }
        else {
            if (ySpeed / xSpeed < -1.09) {
                toPrint = '|';
            }
            else if (ySpeed / xSpeed < -0.383) {
                toPrint = '/';
            }
            else if (ySpeed / xSpeed < 0.383) {
                toPrint = '-';
            }
            else if (ySpeed / xSpeed < 1.09) {
                toPrint = '\\';
            }
            else {
                toPrint = '|';
            }
        }
    }

    void particle::tickSet(collider& col) {
        x += xSpeed;
        y += ySpeed;
        lifetime--;
    }

    void particle::tickGet(collider& col) {}

    bool particle::finalize() {
        if (lifetime == 0) {
            return true;
        }
        else {
            return false;
        }
    }

    void particle::print(float cameraX, float cameraY, Font displayFont) {
        myDrawText(displayFont, TextToUtf8(&toPrint, 1), (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
    }



/******************************************************************************/
//physicalParticle
//A particle to which physics applies.
/******************************************************************************/
/*
    physicalParticle::physicalParticle(   float newX, float newY, Color newTint,
                        float newSizeFactor, float newXSpeed, float newYSpeed, int c, int newLifetime,
                        float newElasticity, float newMaxSpeed, float newGravity, float newFriction) :
                        entity(newX, newY, newTint, newSizeFactor),
                        particle(newX, newY, newTint, newSizeFactor, 0, 0, c, newLifetime),
                        lightPhysicalEntity(newX, newY, newTint, newSizeFactor, newElasticity, newXSpeed, newYSpeed, newMaxSpeed, newGravity, newFriction),
                        dynamicChar(c == 0) {}

    bool physicalParticle::doesCollide(float otherX, float otherY, int otherType) {
        return false;
    }

    collision physicalParticle::getCollision() {
        return collision(0);
    }

    bool physicalParticle::stopColliding() {
        return finalize();
    }

    void physicalParticle::tickSet(collider& col) {
        lifetime--;
        lightPhysicalEntity::tickSet(col);
        if (col.isSolid(x, y)) {
            shouldDelete = true;
        }
    }

    void physicalParticle::tickGet(collider& col) {
        for (collision thisCollision : collisions) {
            xMomentum += thisCollision.xVal;
            yMomentum += thisCollision.yVal;
            if (thisCollision.type == 'w') {
                shouldDelete = true;
            }
        }
        lightPhysicalEntity::tickGet(col);
        collisions.clear();
    }

    bool physicalParticle::finalize() {
        return ((xMomentum < 0.01 && xMomentum > -0.01 && yMomentum < 0.01 && yMomentum > -0.01) || shouldDelete || particle::finalize());
    }

    void physicalParticle::print(float cameraX, float cameraY, Font displayFont) {
        if (dynamicChar) {
            if (abs(xMomentum) + abs(yMomentum) > 0.2) {
                xSpeed = xMomentum;
                ySpeed = yMomentum;
                particle::setDirection();
            }
            else {
                toPrint = '.';
            }
        }
        myDrawText(displayFont, TextToUtf8(&toPrint, 1), (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
    }
*/
