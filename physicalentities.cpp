#include "physicalentities.hpp"

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

    unsigned int realPhysicalEntity::type() {
        return REALPHYSICALENTITYTYPE;
    }

    bool realPhysicalEntity::doesCollide(float otherX, float otherY, int otherType) {
        return ((otherType == WATERTYPE && lastTickUnderWater != isUnderWater) || (otherX >= x && otherX <= x + 1 && otherY >= y && otherY <= y + 1));
    }

    collision realPhysicalEntity::getCollision(float otherX, float otherY, int otherType) {
        if (otherType == WATERTYPE) {
            return collision(type(), 0, x, yMomentum);
        }
        else {
            return collision(type(), 0, xMomentum, yMomentum);
        }
    }

    bool realPhysicalEntity::stopColliding() {
        return false;
    }

    void realPhysicalEntity::tickSet() {}

    void realPhysicalEntity::tickGet() {
        float waterMoveX = 0, waterMoveY = 0;
        lastTickUnderWater = isUnderWater;
        isUnderWater = false;
        for (auto c : collisions) {
            if (c.type == WATERTYPE) {
                isUnderWater = true;
                if (lastTickUnderWater) {
                    waterMoveX += c.xVal / 2;
                    waterMoveY += c.yVal / 2;
                }
            }
            else if (c.type == FORCEFIELDTYPE) {
                xMomentum += c.xVal;
                yMomentum += c.yVal;
            }
        }
        collisions.clear();

        if (isUnderWater) {
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

        float xDist = (xMomentum + waterMoveX) / (abs(xMomentum + waterMoveX) + 1);
        for (int i = 0; i < abs(xMomentum + waterMoveX) + 1; i++) {
            if (world -> isSolid((int)(x + xDist) + (xDist > 0), (int)y)) {// || world -> isSolid((int)y + 0.5, (int)(x + xDist) + (xDist > 0))) {
                x = floor(x + xDist) + (xDist < 0);
                xMomentum *= (-1 * elasticity);
                break;
            }
            else {
                x += xDist;
            }
        }

        float yDist = (yMomentum + waterMoveY) / (abs(yMomentum + waterMoveY) + 1);
        for (int i = 0; i < abs(yMomentum + waterMoveY) + 1; i++) {
            if (world -> isSolid((int)(x + 0.5 - width / 2), (int)(y + yDist) + (yDist > 0)) || world -> isSolid((int)(x + 0.5 + width / 2), (int)(y + yDist) + (yDist > 0))) {
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

    unsigned int physicalParticle::type() {
        return PHYSICALPARTICLETYPE;
    }

    bool physicalParticle::doesCollide(float otherX, float otherY, int otherType) {
        return false;
    }

    bool physicalParticle::stopColliding() {
        return shouldDelete;
    }

    void physicalParticle::tickSet() {
        if (world -> isSolid(x, y) || lifetime-- < 0 || !isUnderWater && (xMomentum < 0.01 && xMomentum > -0.01 && yMomentum < 0.01 && yMomentum > -0.01)) {
            shouldDelete = true;
        }
    }

    void physicalParticle::tickGet() {

        float xMomentumUnderwater, yMomentumUnderwater, xForce = 0, yForce = 0;

        lastTickUnderWater = isUnderWater;
        isUnderWater = false;
        for (auto c : collisions) {
            if (c.type == WATERTYPE) {
                isUnderWater = true;
                xMomentum = 0.8 * xMomentum + 0.2 * c.xVal;
                yMomentum = 0.8 * yMomentum + 0.2 * c.yVal;
            }
            else if (c.type == FORCEFIELDTYPE) {
                xMomentum += c.xVal;
                yMomentum += c.yVal;
            }
        }
        collisions.clear();

        if (!isUnderWater) {
            yMomentum += gravity;
        }

        if (world -> isSolid((int)(x + xMomentum) + (xMomentum > 0), (int)(y))) {
            x = floor(x + xMomentum) + (xMomentum < 0);
            xMomentum *= (-1 * elasticity);
        }
        else {
            x += xMomentum;
        }

        if (world -> isSolid((int)x, (int)(y + yMomentum) + (yMomentum > 0))) {
            y = floor(y + yMomentum) + (yMomentum < 0);
            yMomentum *= (-1 * elasticity);
            xMomentum *= friction;
        }
        else {
            y += yMomentum;
        }
    }

    bool physicalParticle::finalize() {return shouldDelete;}

    void physicalParticle::print() {
        if (dynamicChar) {
            if (abs(xMomentum) + abs(yMomentum) > 0.5) {
                xSpeed = xMomentum;
                ySpeed = yMomentum;
                particle::setDirection();
            }
            else {
                toPrint[0] = '.';
            }
        }
        theCanvas -> draw(x, y, tint, sizeFactor, toPrint);
    }

