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
        hit = false;
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

        float momentumMagnitude = pow(pow(xMomentum, 2) + pow(yMomentum, 2), 0.5);
        if (momentumMagnitude > maxSpeed) {
            xMomentum *= maxSpeed / momentumMagnitude;
            yMomentum *= maxSpeed / momentumMagnitude;
        }

        float dX = xMomentum + pushedX;
        float dY = yMomentum + pushedY;
        bool hitX, hitY;
        Vector2 newPosition = world -> go((Vector2){x, y}, (Vector2){dX, dY}, width, height, hitX, hitY, xMomentum, yMomentum, elasticity, id);
        if (hitX) {
//            xMomentum = xMomentum * (-1 * elasticity);
            hit = true;
        }
        if (hitY) { 
//            yMomentum = yMomentum * (-1 * elasticity);
            xMomentum *= friction;
            hit = true;
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


/******************************************************************************/
//physicalParticle
//A physicalEntity with some particle characteristics
/******************************************************************************/

    physicalParticle::physicalParticle( float newX, float newY, Color newTint, float newScale, int displayChar, float newElasticity, float newXMomentum,
        float newYMomentum, float newMaxSpeed, float newGravity, float newFriction, int newLifetime) :
        entity(newX, newY, newTint, newScale),
        physicalEntity(newX, newY, newTint, newScale, 0.5, 0.5, newElasticity, newXMomentum, newYMomentum, newMaxSpeed, newGravity, newFriction),
        particle(newX, newY, newTint, newScale, newXMomentum, newYMomentum, displayChar, newLifetime),
        dynamicChar(displayChar == 0),
        lifetime(newLifetime) {
            width = 0.5;
            height = 0.5;
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

    void physicalParticle::tickSet() {}

    void physicalParticle::tickGet() {
        physicalEntity::tickGet();
        //Delete if in a wall, or timed out, or stopped moving, or fell out of level.
        if (world -> isSolid(x + width / 2, y + height / 2)) {
            shouldDelete = true;
        }
        else if (lifetime-- < 0 || (!isUnderWater && xMomentum < 0.01 && xMomentum > -0.01 && yMomentum < 0.01 && yMomentum > -0.01) || y > world -> getRows() + 100) {
            shouldDelete = true;
        }
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
                display[0] = '.';
            }
        }
//        theScreen -> draw(x, y, tint, scale, display, doLighting, doHighlight);
        theScreen -> draw(x - 0.25, y - 0.25, tint, scale, display, doLighting, doHighlight);
    }

