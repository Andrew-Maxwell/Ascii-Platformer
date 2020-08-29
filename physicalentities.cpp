#include "physicalentities.hpp"

/******************************************************************************/
//physicalEntity
//An entity to which physics (gravity and not travelling through solid objects)
//applies, more rigorously.
/******************************************************************************/

    physicalEntity::physicalEntity(float newX, float newY,  Color newTint, float newScale, float newWidth, float newHeight, float newElasticity, float newXMomentum,
        float newYMomentum, float newMaxSpeed, float newGravity, float newFriction) :
        entity(newX, newY, newTint, newScale),
        width(newWidth),
        height(newHeight),
        elasticity(newElasticity),
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
        hitX = hitY = false;
        float pushedX = 0, pushedY = 0;
        lastTickUnderWater = isUnderWater;
        isUnderWater = false;
        for (auto c : collisions) {
            if (c.type == WATERTYPE) {
                isUnderWater = true;
                pushedX += c.xVal;
                pushedY += c.yVal;
            }
            else if (c.type == FORCEFIELDTYPE) {
                xMomentum += c.xVal;
                yMomentum += c.yVal;
            }
            else if (c.type == ELEVATORTYPE) {
                pushedX += c.xVal;
                if (c.yVal < gravity * 5) {
                    pushedY += c.yVal;
                }
            }
        }
        collisions.clear();

        if (isUnderWater) {
            yMomentum *= WATERRESISTANCE;
            xMomentum *= WATERRESISTANCE;
        }
        else if (!onGround) {
            yMomentum += gravity;
        }

        if (abs(xMomentum) > maxSpeed) {
            xMomentum *= (maxSpeed / abs(xMomentum) + 4.0) / 5.0;
        }
        if (abs(yMomentum) > maxSpeed) {
            yMomentum *= (maxSpeed / abs(yMomentum) + 4.0) / 5.0;
        }

/*
        xMomentum = min(xMomentum, maxSpeed);
        xMomentum = max(xMomentum, -1 * maxSpeed);
        yMomentum = min(yMomentum, maxSpeed);
        yMomentum = max(yMomentum, -1 * maxSpeed);
*/
        float dX = xMomentum + pushedX;
        float dY = yMomentum + pushedY;
        Vector2 newPosition = world -> go((Vector2){x, y}, (Vector2){dX, dY}, width, height, hitX, hitY, xMomentum, yMomentum, elasticity, id);
        if (hitY) { 
//            yMomentum = yMomentum * (-1 * elasticity);
            xMomentum *= friction;
            if (newPosition.y < y + dY) {
                onGround = true;
            }
        }
        else {
            onGround = false;
        }
        x = newPosition.x;
        y = newPosition.y;
    }

    bool physicalEntity::finalize() {return false;}

    void physicalEntity::print() {}


