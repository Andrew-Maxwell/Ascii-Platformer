#include "physicalentities.hpp"


/******************************************************************************/
//lightPhysicalEntity
//An entity to which physics (gravity and not travelling through solid objects)
//applies -- loosely.
/******************************************************************************/

    lightPhysicalEntity::lightPhysicalEntity( float newx, float newy, Color newTint, float newSizeFactor, float newElasticity, float newXMomentum,
                                  float newYMomentum, float newMaxSpeed, float newGravity, float newFriction) :
                                  entity(newx, newy, newTint, newSizeFactor),
                                  xMomentum(newXMomentum),
                                  yMomentum(newYMomentum),
                                  elasticity(newElasticity),
                                  maxSpeed(newMaxSpeed),
                                  gravity(newGravity),
                                  friction(newFriction) {}

    void  lightPhysicalEntity::tickSet(collider& col) {
        yMomentum += gravity;

        yMomentum = min(yMomentum, maxSpeed);
        yMomentum = max(yMomentum, -1 * maxSpeed);
        xMomentum = min(xMomentum, maxSpeed);
        xMomentum = max(xMomentum, -1 * maxSpeed);

        if (col.isSolid((int)y, (int)(x + xMomentum) + (xMomentum > 0))) {
            x = floor(x + xMomentum) + (xMomentum < 0);
            xMomentum *= (-1 * elasticity);
        }
        else {
            x += xMomentum;
        }

        if (col.isSolid((int)(y + yMomentum) + (yMomentum > 0), (int)x)) {
            y = floor(y + yMomentum) + (yMomentum < 0);
            yMomentum *= (-1 * elasticity);
            xMomentum *= friction;
        }
        else {
            y += yMomentum;
        }
    }

    void lightPhysicalEntity::tickGet(collider& col) {}

    bool lightPhysicalEntity::finalize() {return false;}

    void lightPhysicalEntity::print() {}

/******************************************************************************/
//realPhysicalEntity
//An entity to which physics (gravity and not travelling through solid objects)
//applies, more rigorously.
/******************************************************************************/

    realPhysicalEntity::realPhysicalEntity(float newx, float newy,  Color newTint, float newSizeFactor, float elasticity, float newXMomentum,
                                float newYMomentum, float newMaxSpeed, float newGravity, float newFriction) :
                                entity(newx, newy, newTint, newSizeFactor),
                                maxSpeed(newMaxSpeed),
                                gravity(newGravity),
                                friction(newFriction),
                                xMomentum(newXMomentum),
                                yMomentum(newYMomentum) {}

    void realPhysicalEntity::tickSet(collider& col) {
        yMomentum += gravity;

        float momentumMagnitude = pow(pow(xMomentum, 2) + pow(yMomentum, 2), 0.5);
        if (momentumMagnitude > maxSpeed) {
            xMomentum *= momentumMagnitude / maxSpeed;
            yMomentum *= momentumMagnitude / maxSpeed;
        }

        float xDist = xMomentum / (abs(xMomentum) + 1);
        for (int i = 0; i < abs(xMomentum) + 1; i++) {
            if (col.isSolid((int)y, (int)(x + xDist) + (xDist > 0))) {// || col.isSolid((int)y + 0.5, (int)(x + xDist) + (xDist > 0))) {
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
            if (col.isSolid((int)(y + yDist) + (yDist > 0), (int)(x + 0.5 - width / 2)) || col.isSolid((int)(y + yDist) + (yDist > 0), (int)(x + 0.5 + width / 2))) {
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

    void realPhysicalEntity::tickGet(collider& col) {}

    bool realPhysicalEntity::finalize() {return false;}

    void realPhysicalEntity::print() {}

