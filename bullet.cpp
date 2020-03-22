#include "bullet.hpp"

/*****************************************************************************/    
//Bullet
//a physicalParticle that deals damage
/*****************************************************************************/

    bullet::bullet(float newX, float newY, Color newTint, float newSizeFactor, 
                   float newXMomentum, float newYMomentum, int c, int newLifetime, float newElasticity,
                   float newMaxSpeed, float newGravity, float newFriction, int newDamage, float newPower, float newRange) :
        entity(newX, newY, newTint, newSizeFactor),
        realPhysicalEntity(newX, newY, newTint, newSizeFactor, newElasticity, newXMomentum, newYMomentum, 
            newMaxSpeed, newGravity, newFriction),
        particle(newX, newY, newTint, newSizeFactor, 0, 0, c, newLifetime),
        damage(newDamage),
        dynamicChar(c == 0),
        lifetime(newLifetime),
        power(newPower),
        range(newRange)
    {}

    unsigned int bullet::type() {
        return BULLETTYPE;
    }

    bool bullet::doesCollide(float otherX, float otherY, int otherType) {
        if (realPhysicalEntity::doesCollide(otherX, otherY, otherType) && !hit) {
            hit = true;
            return true;
        }
        return false;
    }

    collision bullet::getCollision(float otherX, float otherY, int otherType) {
        if (otherType == WATERTYPE) {
            return realPhysicalEntity::getCollision(otherX, otherY, otherType);
        }
        return collision(type(), damage, xMomentum, yMomentum);
    }

    bool bullet::stopColliding() {
        return hit;
    }

    void bullet::tickSet() {
        realPhysicalEntity::tickSet();
        if (xMomentum == 0 || lifetime < 0) {   //If hit a wall during physicalParticle::tickSet()
            hit = true;
        }

        if (hit && !exploded) {
            exploded = true;
            explode (60, x, y, tint, sizeFactor, 1, 0, 600, 0.5);
            explosion* e = new explosion(x, y, tint, sizeFactor, 0, -1.0, 30);
            world -> addCollideable(e);
        }
    }

    void bullet::tickGet() {
        realPhysicalEntity::tickGet();
    }

    bool bullet::finalize() {
        return hit;
    }

    void bullet::print() {
         if (!hit) {
            if (dynamicChar) {
                xSpeed = xMomentum;
                ySpeed = yMomentum;
                particle::setDirection();
            }
            particle::print();
        }
    }

