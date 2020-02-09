#include "bullet.hpp"

/*****************************************************************************/    
//Bullet
//a physicalParticle that deals damage
/*****************************************************************************/

    bullet::bullet(float newX, float newY, Color newTint, float newSizeFactor, entityList* newEList,
                   float newXMomentum, float newYMomentum, int c, int newLifetime, float newElasticity,
                   float newMaxSpeed, float newGravity, float newFriction, int newDamage) :
        entity(newX, newY, newTint, newSizeFactor),
        particle(newX, newY, newTint, newSizeFactor, 0, 0, c, newLifetime),
        realPhysicalEntity(newX, newY, newTint, newSizeFactor, newElasticity, newXMomentum, newYMomentum, 
            newMaxSpeed, newGravity, newFriction),
        damage(newDamage),
        dynamicChar(c == 0),
        lifetime(newLifetime)
    {
        eList = newEList;
        type = 6;
    }

    bool bullet::doesCollide(float otherX, float otherY, int type) {
        if (realPhysicalEntity::doesCollide(otherX, otherY, type) && !hit) {
            hit = true;
            return true;
        }
        return false;
    }

    collision bullet::getCollision() {
        return collision(6, damage, xMomentum, yMomentum);
    }

    bool bullet::stopColliding() {
        return hit;
    }

    void bullet::tickSet(collider& col) {
        realPhysicalEntity::tickSet(col);

        if (xMomentum == 0 || lifetime < 0) {   //If hit a wall during physicalParticle::tickSet()
            hit = true;
        }

        if (hit & !exploded) {
            exploded = true;
            explosion(col, eList, 7, x, y, tint, sizeFactor, 0.2, 0, 60, 1);
        }
    }

    void bullet::tickGet(collider& col) {
        realPhysicalEntity::tickGet(col);
    }

    bool bullet::finalize() {
        return hit;
    }

    void bullet::print(float cameraX, float cameraY, Font displayFont) {
         if (!hit) {
            if (dynamicChar) {
                xSpeed = xMomentum;
                ySpeed = yMomentum;
                particle::setDirection();
            }
            particle::print(cameraX, cameraY, displayFont);
        }
    }

