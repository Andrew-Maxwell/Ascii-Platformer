#include "bullet.hpp"

/*****************************************************************************/    
//Bullet
//a physicalParticle that deals damage
/*****************************************************************************/

    bullet::bullet(float newX, float newY, Color newTint, float newSizeFactor, entityList* newEList,
                   float newXMomentum, float newYMomentum, int c, int newLifeTime, float newElasticity,
                   float newMaxSpeed, float newGravity, float newFriction, int newDamage) :
        entity(newX, newY, newTint, newSizeFactor),
        physicalParticle(newX, newY, newTint, newSizeFactor, newXMomentum, newYMomentum, c, newLifeTime, newElasticity,
                         newMaxSpeed, newGravity, newFriction),
        damage(newDamage)
    {
        eList = newEList;
        type = 6;
    }

    bool bullet::doesCollide(float otherX, float otherY, int type) {
        if (!hit && otherX > x - width && otherX < x + width && otherY > y - width && otherY < y + width) {
            return true;
        }
        return false;
    }

    collision bullet::getCollision() {
        return collision(6, damage, xMomentum, yMomentum);
    }

    bool bullet::stopColliding() {
        return hit || physicalParticle::stopColliding();
    }

    void bullet::tickSet(collider& col) {
        physicalParticle::tickSet(col);

        if (xMomentum == 0) {   //If hit a wall during physicalParticle::tickSet()
            hit = true;
        }

        if (hit & !exploded) {
            exploded = true;
            explosion(col, eList, 6, x, y, tint, sizeFactor, 0.25, 0, 60, 0.3);
        }
    }

    void bullet::tickGet(collider& col) {
        physicalParticle::tickGet(col);

        for (collision c : collisions) {
            if (c.type == 5) {   //forcefield
                xMomentum += c.xVal;
                yMomentum += c.yVal;
            }
            else {
                hit = true;
            }
        }
    }

    bool bullet::finalize() {
        return (physicalParticle::finalize() || hit);
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

