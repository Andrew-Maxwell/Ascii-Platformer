#ifndef BULLET_HPP
#define BULLET_HPP

#include "col.hpp"
#include "effects.hpp"
#include "entitymeta.hpp"
#include "meta.hpp"
#include "particles.hpp"
#include "physicalentities.hpp"

using namespace rapidjson;

/*****************************************************************************/
//Bullet
//Also self-explanatory
/*****************************************************************************/

class bullet : public physicalParticle {

    int damage;
    bool hit = false;
    bool exploded = false;
    entityList collisionParticles;
    float width = 0.8;

    public:

    bullet(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor,
                   float newXMomentum, float newYMomentum, int c, int newLifeTime, float newElasticity,
                   float newMaxSpeed, float newGravity, float newFriction, int newDamage);

    bool doesCollide(float otherX, float otherY, int type);

    collision getCollision();

    bool stopColliding();

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);

};

#endif //BULLET_HPP
