#include "newtestentity.hpp"

    newTestEntity::newTestEntity(  float newX, float newY, Color newTint, float newSizeFactor, int displayChar, float elasticity, float newXMomentum,
                                float newYMomentum, float newMaxSpeed, float newGravity, float newFriction, int maxHealth, int newDamage) :
                        entity(newX, newY, newTint, newSizeFactor),
                        physicalEntity(newX, newY, newTint, newSizeFactor, elasticity, newXMomentum,
                                newYMomentum, newMaxSpeed, newGravity, newFriction)
    {
        health = maxHealth;
        damage = newDamage;
    }

    unsigned int newTestEntity::type() {
        return ENEMYTYPE;
    }

    bool newTestEntity::doesCollide(float otherX, float otherY, int otherType) {
        return physicalEntity::doesCollide(otherX, otherY, otherType);
    }

    collision newTestEntity::getCollision(float otherX, float otherY, int otherType) {
        if (otherType == WATERTYPE) {
            return physicalEntity::getCollision(otherX, otherY, otherType);
        }
        else {
            return collision(type(), damage);
        }
    }

    bool newTestEntity::stopColliding() {
        return isDead;
    }

    void newTestEntity::tickSet() {

        // TODO: AI

        // spikes
        int spikeDamage = world->getDamage((int)(x + 0.5), (int)(y + 0.5));
        if (spikeDamage) {
            health += spikeDamage;
            hurtTimer = 15;
        }

        // am i dead
        if(health <= 0) {
            isDead = true;
        }
        physicalEntity::tickSet();
    }

    void newTestEntity::tickGet() {
        list<collision>::iterator colIter = collisions.begin();
        while (colIter != collisions.end()) {
            switch(colIter -> type) {
                case BULLETTYPE: // bullet
                    health += colIter -> damage;
                    colIter = collisions.erase(colIter);
                    damageIndicator(colIter -> damage, x, y, HURTCOLOR, sizeFactor);
                    break;
                default:
                    colIter++;
            }
        }
        physicalEntity::tickGet();
    }

    bool newTestEntity::finalize() {
        return isDead;
    }

    void newTestEntity::print() {
        theScreen -> draw(x, y, tint, sizeFactor, "test");
    }