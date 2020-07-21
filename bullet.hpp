#ifndef BULLET_HPP
#define BULLET_HPP


#include "effects.hpp"
#include "entity.hpp"
#include "meta.hpp"
#include "particles.hpp"
#include "physicalentities.hpp"
#include "forcefield.hpp"
#include "world.hpp"

using namespace rapidjson;

/*****************************************************************************/
//Bullet
//Also self-explanatory
/*****************************************************************************/

class bullet : public physicalEntity, public particle, public entityParent {

    float lastX, lastY;
    int damage, explosionDamage;
    float explosionPower, explosionRange;
    bool hitWall, hitWater, hitEntity, sticky;
    bool bulletHit = false;
    bool stuck = false;
    bool dynamicChar;
    int particleCount;

    float width = 0.8;
    int lifetime;

    public:

    bullet(float newX, float newY, Color newTint, float newSizeFactor, 
           float newXMomentum, float newYMomentum, int c, int particleCount, int newLifeTime,
           float newElasticity, float newMaxSpeed, float newGravity, float newFriction,
           int newDamage, float newExplosionPower, float newExplosionRange,
           bool hitWall, bool hitWater, bool hitEntity, bool sticky);

    unsigned int type();

    bool doesCollide(float otherX, float otherY, int otherType, unsigned int otherID);

    collision getCollision(float otherX, float otherY, int otherType, unsigned int otherID);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();

};

#endif //BULLET_HPP
