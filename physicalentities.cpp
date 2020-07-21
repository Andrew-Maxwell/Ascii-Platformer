#include "physicalentities.hpp"

/******************************************************************************/
//physicalEntity
//An entity to which physics (gravity and not travelling through solid objects)
//applies, more rigorously.
/******************************************************************************/

    physicalEntity::physicalEntity(float newX, float newY,  Color newTint, float newSizeFactor, float elasticity, float newXMomentum,
                                float newYMomentum, float newMaxSpeed, float newGravity, float newFriction) :
                                entity(newX, newY, newTint, newSizeFactor),
                                maxSpeed(newMaxSpeed),
                                gravity(newGravity),
                                friction(newFriction),
                                xMomentum(newXMomentum),
                                yMomentum(newYMomentum) {}

    unsigned int physicalEntity::type() {
        return PHYSICALENTITYTYPE;
    }

    bool physicalEntity::doesCollide(float otherX, float otherY, int otherType, unsigned int otherID) {
        return ((otherType == WATERTYPE && lastTickUnderWater != isUnderWater) || (otherX >= x - 1 && otherX <= x + 1 && otherY >= y - 1 && otherY <= y + 1));
    }

    collision physicalEntity::getCollision(float otherX, float otherY, int otherType, unsigned int otherID) {
        if (otherType == WATERTYPE) {
            return collision(PHYSICALENTITYTYPE, id, 0, x, yMomentum);
        }
        else {
            return collision(PHYSICALENTITYTYPE, id, 0, xMomentum, yMomentum);
        }
    }

    bool physicalEntity::stopColliding() {
        return false;
    }

    void physicalEntity::tickSet() {}

    void physicalEntity::tickGet() {
        hit = false;
        float waterMoveX = 0, waterMoveY = 0;
        lastTickUnderWater = isUnderWater;
        isUnderWater = false;
        for (auto c : collisions) {
            if (c.type == WATERTYPE) {
                isUnderWater = true;
                waterMoveX += c.xVal;
                waterMoveY += c.yVal;
            }
            else if (c.type == FORCEFIELDTYPE) {
                xMomentum += c.xVal * 0.8;
                yMomentum += c.yVal * 0.8;
            }
        }
        collisions.clear();

        if (isUnderWater) {
            yMomentum *= WATERRESISTANCE;
            xMomentum *= WATERRESISTANCE;
        }
        else {
            yMomentum += gravity;
        }

        float momentumMagnitude = pow(pow(xMomentum, 2) + pow(yMomentum, 2), 0.5);
        if (momentumMagnitude > maxSpeed) {
            xMomentum *= maxSpeed / momentumMagnitude;
            yMomentum *= maxSpeed / momentumMagnitude;
        }

        float xStep = (xMomentum + waterMoveX) / (abs(xMomentum + waterMoveX) + 1);
        for (int i = 0; i < abs(xMomentum + waterMoveX) + 1; i++) {
            if (world -> isSolid((int)(x + xStep) + (xStep > 0), (int)y)) {// || world -> isSolid((int)y + 0.5, (int)(x + xStep) + (xStep > 0))) {
                x = floor(x + xStep) + (xStep < 0);
                xMomentum *= (-1 * elasticity);
                hit = true;
                break;
            }
            else {
                x += xStep;
            }
        }

        float yStep = (yMomentum + waterMoveY) / (abs(yMomentum + waterMoveY) + 1);
        for (int i = 0; i < abs(yMomentum + waterMoveY) + 1; i++) {
            if (world -> isSolid((int)(x + 0.5 - width / 2), (int)(y + yStep) + (yStep > 0)) || world -> isSolid((int)(x + 0.5 + width / 2), (int)(y + yStep) + (yStep > 0))) {
                y = floor(y + yStep) + (yStep < 0);
                yMomentum *= (-1 * elasticity);
                xMomentum *= friction;
                hit = true;
                break;
            }
            else {
                y += yStep;
            }
        }
    }

    bool physicalEntity::finalize() {return false;}

    void physicalEntity::print() {}


/******************************************************************************/
//physicalParticle
//A physicalEntity with some particle characteristics
/******************************************************************************/

    physicalParticle::physicalParticle( float newX, float newY, Color newTint, float newSizeFactor, int displayChar, float newElasticity, float newXMomentum,
                                  float newYMomentum, float newMaxSpeed, float newGravity, float newFriction, int newLifetime) :
                                  entity(newX, newY, newTint, newSizeFactor),
                                  physicalEntity(newX, newY, newTint, newSizeFactor, newElasticity, newXMomentum, newYMomentum, newMaxSpeed, newGravity, newFriction),
                                  particle(newX, newY, newTint, newSizeFactor, newXMomentum, newYMomentum, displayChar, newLifetime),
                                  dynamicChar(displayChar == 0),
                                  lifetime(newLifetime) {
        elasticity = newElasticity;
    }

    unsigned int physicalParticle::type() {
        return PHYSICALPARTICLETYPE;
    }

    bool physicalParticle::doesCollide(float otherX, float otherY, int otherType, unsigned int otherID) {
        return false;
    }

    bool physicalParticle::stopColliding() {
        return shouldDelete;
    }

    void physicalParticle::tickSet() {
        if (world -> isSolid(x, y)) {
            shouldDelete = true;
        }
        else if (lifetime-- < 0 || (!isUnderWater && xMomentum < 0.01 && xMomentum > -0.01 && yMomentum < 0.01 && yMomentum > -0.01) || y > world -> getRows() + 100) {
            shouldDelete = true;
        }
        if (IsKeyPressed(KEY_J)) {
            shouldDelete = true;
        }
    }

    void physicalParticle::tickGet() {
        physicalEntity::tickGet();
    }

    bool physicalParticle::finalize() {
        return shouldDelete;
    }

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
        theScreen -> draw(x + 0.125, y, tint, sizeFactor, toPrint, doLighting);
    }

