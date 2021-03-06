#include "bullet.hpp"

/*****************************************************************************/    
//Bullet
//a physicalParticle that deals damage
/*****************************************************************************/

    bullet::bullet( float newX, float newY, Color newTint, float newScale, 
                    float newXMomentum, float newYMomentum, int c, int newParticleCount, int newLifetime,
                    float newElasticity, float newMaxSpeed, float newGravity, float newFriction,
                    int newDamage, float newExplosionPower, float newExplosionRange,
                    bool newHitWall, bool newHitWater, bool newHitEntity, bool newSticky) :
        entity(newX, newY, newTint, newScale),
        physicalEntity(newX, newY, newTint, newScale, 0.5, 0.5, newElasticity, newXMomentum, newYMomentum, 
            newMaxSpeed, newGravity, newFriction),
        particle(newX, newY, newTint, newScale, 0, 0, utf8(c), newLifetime),
        damage(newDamage),
        dynamicChar(c == 0),
        particleCount(newParticleCount),
        lifetime(newLifetime),
        explosionPower(newExplosionPower),
        explosionRange(newExplosionRange),
        hitWall(newHitWall),
        hitWater(newHitWater),
        hitEntity(newHitEntity),
        sticky(newSticky),
        lastX(newX),
        lastY(newY) {
            physicalEntity::tickSet();
            physicalEntity::tickSet();
        }

    unsigned int bullet::type() {
        return BULLETTYPE;
    }

    bool bullet::doesCollide(float otherX, float otherY, int otherType, unsigned int otherID) {
        if (otherType == TRIGGERTYPE) {
            return true;
        }
        //Check for any objects between this position and the last position
        if (otherX >= min(lastX, x) - 1 && otherX <= max(lastX, x) + 1 && otherY >= min(lastY, y) - 1 && otherY <= max(lastY, y) + 1) {
            int numSteps = max(abs(int(x - lastX)), abs(int(y - lastY))) + 1;
            float xStep = (x - lastX) / numSteps;
            float yStep = (y - lastY) / numSteps;
            for (int i = 1; i <= numSteps; i++) {
                if (otherX >= x - xStep * i - 1 && otherX <= x - xStep * i + 1 && otherY >= y - yStep * i - 1 && otherY <= y + yStep * i + 1) {
                    if (hitEntity && (otherType == PLAYERTYPE || otherType == PHYSICALENTITYTYPE || otherType == ENEMYTYPE)) {
                        bulletHit = true;
                    }
                    if (hitWater && otherType == WATERTYPE) {
                        bulletHit = true;
                    }
                    return true;
                }
            }
        }
        return false;
    }

    collision bullet::getCollision(float otherX, float otherY, int otherType, unsigned int otherID) {
        if (otherType == TRIGGERTYPE) {
            return collision(type(), 0, x, y);
        }
        if (otherType == WATERTYPE) {
            return physicalEntity::getCollision(otherX, otherY, otherType, otherID);
        }
        return collision(type(), id, damage, xMomentum, yMomentum);
    }

    bool bullet::stopColliding() {
        return bulletHit;
    }

    void bullet::tickSet() {
        if (!stuck) {
            physicalEntity::tickSet();
        }
        if (((hitX || hitY) && hitWall) || lifetime-- < 0) {   //If hit a wall during physicalParticle::tickSet()
            bulletHit = true;
        }
        if ((hitX || hitY) && sticky) {
            stuck = true;
            xMomentum = 0;
            yMomentum = 0;
        }
    }

    void bullet::tickGet() {
        collisions.clear();
        if (!stuck) {
            lastX = x;
            lastY = y;
            physicalEntity::tickGet();
        }
        if (bulletHit && explosionRange > 0) {
            explode (particleCount, x, y, tint, scale, 0.02, 0, 600, 0.5, zPosition);
            explosion* e = new explosion(x, y, tint, scale, 0, explosionPower, explosionRange);
            world -> addCollideable(e);
        }
    }

    bool bullet::finalize() {
        return bulletHit;
    }

    void bullet::print() {
         if (!bulletHit) {
            if (dynamicChar) {
                xSpeed = xMomentum;
                ySpeed = yMomentum;
                particle::setDirection();
            }
            particle::print();
        }
    }

