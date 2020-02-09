#include "physicalentities.hpp"


/******************************************************************************/
//physicalParticle
//An entity to which physics (gravity and not travelling through solid objects)
//applies -- loosely.
/******************************************************************************/

    physicalParticle::physicalParticle( float newX, float newY, Color newTint, float newSizeFactor, int displayChar, float newElasticity, float newXMomentum,
                                  float newYMomentum, float newMaxSpeed, float newGravity, float newFriction, int newLifetime) :
                                  entity(newX, newY, newTint, newSizeFactor),
                                  particle(newX, newY, newTint, newSizeFactor, newXMomentum, newYMomentum, displayChar, newLifetime),
                                  dynamicChar(displayChar == 0),
                                  xMomentum(newXMomentum),
                                  yMomentum(newYMomentum),
                                  elasticity(newElasticity),
                                  maxSpeed(newMaxSpeed),
                                  gravity(newGravity),
                                  friction(newFriction),
                                  lifetime(newLifetime) {}

    bool physicalParticle::doesCollide(float otherX, float otherY, int otherType) {
        return false;
    }

    bool physicalParticle::stopColliding() {
        return shouldDelete;
    }

    void physicalParticle::tickSet(collider& col) {
        if (col.isSolid(x, y) || lifetime-- < 0 || (xMomentum < 0.01 && xMomentum > -0.01 && yMomentum < 0.01 && yMomentum > -0.01)) {
            shouldDelete = true;
        }
    }

    void physicalParticle::tickGet(collider& col) {

        isUnderWater = false;
        for (auto c : collisions) {
            if (c.type == 'w') {
                isUnderWater = true;
            }
            else if (c.type == 5) {
                xMomentum += c.xVal;
                yMomentum += c.yVal;
            }
        }
        collisions.clear();

        if (isUnderWater) {
            yMomentum -= gravity / 3;
            yMomentum *= pow (friction, 2);
            xMomentum *= pow (friction, 2);
        }
        else {
            yMomentum += gravity;
        }

        if (col.isSolid((int)(x + xMomentum) + (xMomentum > 0), (int)(y))) {
            x = floor(x + xMomentum) + (xMomentum < 0);
            xMomentum *= (-1 * elasticity);
        }
        else {
            x += xMomentum;
        }

        if (col.isSolid((int)x, (int)(y + yMomentum) + (yMomentum > 0))) {
            y = floor(y + yMomentum) + (yMomentum < 0);
            yMomentum *= (-1 * elasticity);
            xMomentum *= friction;
        }
        else {
            y += yMomentum;
        }
    }

    bool physicalParticle::finalize() {return shouldDelete;}

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
        myDrawText(displayFont, TextToUtf8(&toPrint, 1), (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 0, tint);
    }

/******************************************************************************/
//realPhysicalEntity
//An entity to which physics (gravity and not travelling through solid objects)
//applies, more rigorously.
/******************************************************************************/

    realPhysicalEntity::realPhysicalEntity(float newX, float newY,  Color newTint, float newSizeFactor, float elasticity, float newXMomentum,
                                float newYMomentum, float newMaxSpeed, float newGravity, float newFriction) :
                                entity(newX, newY, newTint, newSizeFactor),
                                maxSpeed(newMaxSpeed),
                                gravity(newGravity),
                                friction(newFriction),
                                xMomentum(newXMomentum),
                                yMomentum(newYMomentum) {}

    bool realPhysicalEntity::doesCollide(float otherX, float otherY, int otherType) {
        return ((otherType == 'w' && lastTickUnderWater != isUnderWater) || (otherX >= x && otherX <= x + 1 && otherY >= y && otherY <= y + 1));
    }

    collision realPhysicalEntity::getCollision() {
        return collision(-1, 0, x, yMomentum);
    }

    bool realPhysicalEntity::stopColliding() {
        return false;
    }

    void realPhysicalEntity::tickSet(collider& col) {}

    void realPhysicalEntity::tickGet(collider& col) {
        lastTickUnderWater = isUnderWater;
        isUnderWater = false;
        for (auto c : collisions) {
            if (c.type == 'w') {
                isUnderWater = true;
            }
            else if (c.type == 5) {
                xMomentum += c.xVal;
                yMomentum += c.yVal;
            }
        }
        collisions.clear();

        if (isUnderWater) {
            yMomentum -= gravity / 3;
            yMomentum *= pow (friction, 2);
            xMomentum *= pow (friction, 2);
        }
        else {
            yMomentum += gravity;
        }

        float momentumMagnitude = pow(pow(xMomentum, 2) + pow(yMomentum, 2), 0.5);
        if (momentumMagnitude > maxSpeed) {
            xMomentum *= momentumMagnitude / maxSpeed;
            yMomentum *= momentumMagnitude / maxSpeed;
        }

        float xDist = xMomentum / (abs(xMomentum) + 1);
        for (int i = 0; i < abs(xMomentum) + 1; i++) {
            if (col.isSolid((int)(x + xDist) + (xDist > 0), (int)y)) {// || col.isSolid((int)y + 0.5, (int)(x + xDist) + (xDist > 0))) {
                x = floor(x + xDist) + (xDist < 0);
                xMomentum *= (-1 * elasticity);
                break;
            }
            else {
                x += xDist;
            }
        }

        float yDist = yMomentum / (abs(yMomentum) + 1);
        for (int i = 0; i < abs(yMomentum) + 1; i++) {
            if (col.isSolid((int)(x + 0.5 - width / 2), (int)(y + yDist) + (yDist > 0)) || col.isSolid((int)(x + 0.5 + width / 2), (int)(y + yDist) + (yDist > 0))) {
                y = floor(y + yDist) + (yDist < 0);
                yMomentum *= (-1 * elasticity);
                xMomentum *= friction;
                break;
            }
            else {
                y += yDist;
            }
        }
    }

    bool realPhysicalEntity::finalize() {return false;}

    void realPhysicalEntity::print() {}
