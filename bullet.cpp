#include "bullet.hpp"

/*****************************************************************************/    
//Bullet
//a physicalParticle that deals damage
/*****************************************************************************/

    bullet::bullet(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor,
                   float newXMomentum, float newYMomentum, int c, int newLifeTime, float newElasticity,
                   float newMaxSpeed, float newGravity, float newFriction, int newDamage) :
        entity(newX, newY, R, G, B, A, newSizeFactor),
        physicalParticle(newX, newY, R, G, B, A, newSizeFactor, newXMomentum, newYMomentum, c, newLifeTime, newElasticity,
                         newMaxSpeed, newGravity, newFriction),
        damage(newDamage)
    {
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
            explosion(col, collisionParticles, 6, x, y, tint, sizeFactor, 0.25, 0, 60, 0.3);
        }

        collisionParticles.tickSet(col);
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

        collisionParticles.tickGet(col);
    }

    bool bullet::finalize() {
        collisionParticles.finalize();  //Need to finalize collision particles so they have a chance to return true to stopColliding before they/this object are deleted
        return ((physicalParticle::finalize() || hit) && collisionParticles.size() == 0);
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
        else {
            collisionParticles.print(cameraX, cameraY, displayFont);
        }
    }

