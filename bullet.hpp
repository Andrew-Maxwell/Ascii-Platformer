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

class bullet : public realPhysicalEntity, public particle {

    int damage;
    bool hit = false;
    bool exploded = false;
    bool dynamicChar;
    
    float width = 0.8;
    int lifetime;
    float power, range;

    public:

    bullet(float newX, float newY, Color newTint, float newSizeFactor, 
                   float newXMomentum, float newYMomentum, int c, int newLifeTime, float newElasticity,
                   float newMaxSpeed, float newGravity, float newFriction, int newDamage, float power, float range);

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision(float otherX, float otherY, int otherType);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);

};

#endif //BULLET_HPP
