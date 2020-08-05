#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "bullet.hpp"
#include "world.hpp"
#include "effects.hpp"
#include "entity.hpp"
#include "meta.hpp"
#include "particles.hpp"
#include "physicalentities.hpp"

using namespace rapidjson;

/******************************************************************************/
//enemy
//what it sounds like, I guess.
/******************************************************************************/

class enemy : protected physicalEntity, virtual public collideable {  // exists in entityList AND collidableList

    int health, hurtTimer = 0;
    int damage;
    bool isDead = false;

public:

    explicit enemy(  float newX, float newY, Color newTint, float newScale, int displayChar, float elasticity, float newXMomentum,
                                float newYMomentum, float newMaxSpeed, float newGravity, float newFriction, int maxHealth, int newDamage);

    unsigned int type();

    //Collision functions

    bool doesCollide(float otherX, float otherY, int otherType, unsigned int otherID);

    collision getCollision(float otherX, float otherY, int otherType, unsigned int otherID);

    bool stopColliding();

    //Tick functions

    void tickSet();

    void tickGet();

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};

#endif
