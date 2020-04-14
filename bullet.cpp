#include "bullet.hpp"

/*****************************************************************************/    
//Bullet
//a physicalParticle that deals damage
/*****************************************************************************/

    bullet::bullet( float newX, float newY, Color newTint, float newSizeFactor, 
                    float newXMomentum, float newYMomentum, int c, int newParticleCount, int newLifetime,
                    float newElasticity, float newMaxSpeed, float newGravity, float newFriction,
                    int newDamage, float newExplosionPower, float newExplosionRange,
                    bool newHitWall, bool newHitWater, bool newHitEntity, bool newSticky) :
        entity(newX, newY, newTint, newSizeFactor),
        physicalEntity(newX, newY, newTint, newSizeFactor, newElasticity, newXMomentum, newYMomentum, 
            newMaxSpeed, newGravity, newFriction),
        particle(newX, newY, newTint, newSizeFactor, 0, 0, c, newLifetime),
        damage(newDamage),
        dynamicChar(c == 0),
        particleCount(newParticleCount),
        lifetime(newLifetime),
        explosionPower(newExplosionPower),
        explosionRange(newExplosionRange),
        hitWall(newHitWall),
        hitWater(newHitWater),
        hitEntity(newHitEntity),
        sticky(newSticky) {}

    unsigned int bullet::type() {
        return BULLETTYPE;
    }

    bool bullet::doesCollide(float otherX, float otherY, int otherType) {
        return physicalEntity::doesCollide(otherX, otherY, otherType);
    }

    collision bullet::getCollision(float otherX, float otherY, int otherType) {
        if (otherType == WATERTYPE) {
            return physicalEntity::getCollision(otherX, otherY, otherType);
        }
        return collision(type(), damage, xMomentum, yMomentum);
    }

    bool bullet::stopColliding() {
        return bulletHit;
    }

    void bullet::tickSet() {
        if (!stuck) {
            physicalEntity::tickSet();
        }
        if ((physicalEntity::hit && hitWall) || lifetime-- < 0) {   //If hit a wall during physicalParticle::tickSet()
            bulletHit = true;
        }
        if (physicalEntity::hit && sticky) {
            stuck = true;
            xMomentum = 0;
            yMomentum = 0;
        }
    }

    void bullet::tickGet() {
        if (!bulletHit) {
            for (auto c : collisions) {
                if (hitWater && c.type == WATERTYPE) {
                    bulletHit = true;
                    break;
                }
                if (hitEntity && c.type != WATERTYPE) {
                    bulletHit = true;
                    break;
                }
            }
        }
        if (!stuck) {
            physicalEntity::tickGet();
        }
        if (bulletHit && explosionRange > 0) {
            explode (particleCount, x, y, tint, sizeFactor, 0.02, 0, 600, 0.5);
            explosion* e = new explosion(x, y, tint, sizeFactor, 0, explosionPower, explosionRange);
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

