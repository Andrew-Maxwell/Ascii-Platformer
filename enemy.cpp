#include "enemy.hpp"

#define ENEMY_MAX_HEALTH 4
#define PHYSICAL_DAMAGE -2

    //Constructor

    enemy::enemy(  float newX, float newY, Color newTint, float newScale, int displayChar, float elasticity, float newXMomentum,
                                float newYMomentum, float newMaxSpeed, float newGravity, float newFriction, int maxHealth, int newDamage) :
                        entity(newX, newY, newTint, newScale),
                        physicalEntity(newX, newY, newTint, newScale, 1, 1, elasticity, newXMomentum,
                                newYMomentum, newMaxSpeed, newGravity, newFriction)
    {
        health = maxHealth;
        damage = newDamage;
    }

    unsigned int type()
    {
       return ENEMYTYPE;
    }
    //Tick functions

    void enemy::tickSet() {  // Called every tick, movement and autonomous actions go here

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

    void enemy::tickGet() { // Getting shot by player
        list<collision>::iterator colIter = collisions.begin();
        while (colIter != collisions.end()) {
            switch(colIter -> type) {
                case 6: // bullet
                health += colIter -> damage;
                colIter = collisions.erase(colIter);
                break;
            }
        }
        physicalEntity::tickGet();
    }

    bool enemy::finalize() {
        return isDead;
    }

    //Collision functions

    bool enemy::doesCollide(float otherX, float otherY, int otherType, unsigned int otherID)
    {
       return physicalEntity::doesCollide(otherX, otherY, otherType);
    } // Called in collider::tickSet for w/ every other object as param EVERY tick

    collision enemy::getCollision(float otherX, float otherY, int otherType, unsigned int otherID) { // Called collider::tickSet after doesCollide() returns true
        return collision(type(), id, PHYSICAL_DAMAGE); // TODO: add knockback
    }
    bool enemy::stopColliding() {
        return isDead;
    }

    void enemy::print(float cameraX, float cameraY, Font displayFont)
    {
       theScreen -> draw(x, y, tint, scale, "@");
    }
