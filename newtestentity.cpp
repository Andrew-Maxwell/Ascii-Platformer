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
        return 1000;
    }

    bool newTestEntity::doesCollide(float otherX, float otherY, int otherType) {
        return false;
    }

    collision newTestEntity::getCollision(float otherX, float otherY, int otherType) {
        return collision(type());
    }

    bool newTestEntity::stopColliding() {
        return true;
    }

    void newTestEntity::tickSet() {}

    void newTestEntity::tickGet() {}

    bool newTestEntity::finalize() {
        return false;
    }

    void newTestEntity::print() {
        theCanvas -> draw(x, y, tint, sizeFactor, "test");
    }
