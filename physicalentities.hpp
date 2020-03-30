#ifndef PHYSICALENTITIES_HPP
#define PHYSICALENTITIES_HPP

#include "world.hpp"
#include "entity.hpp"
#include "meta.hpp"
#include "particles.hpp"

using namespace rapidjson;

/******************************************************************************/
//physicalEntity
//An entity to which physics (gravity and not travelling through solid objects)
//applies, more rigorously.
/******************************************************************************/

class physicalEntity : virtual public collideable {

    protected:

    bool hit = false;
    bool isUnderWater = false, lastTickUnderWater = false;
    float elasticity, xMomentum, yMomentum, maxSpeed, gravity, friction;
    float width = 0.8;

    public:

    explicit physicalEntity(float newx, float newy, Color newTint, float newSizeFactor, float elasticity, float newXMomentum,
                                float newYMomentum, float newMaxSpeed = 100, float newGravity = GRAVITY, float newFriction = FRICTION);

    unsigned int type();

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision(float otherX, float otherY, int otherType);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

/******************************************************************************/
//physicalParticle
//physicalEntity with some particle features
/******************************************************************************/

class physicalParticle : public physicalEntity, protected particle {

    protected:

    bool shouldDelete = false, dynamicChar;
    int lifetime;

    public:

    explicit physicalParticle( float newx, float newy, Color newTint, float newSizeFactor, int displayChar, float elasticity, float newXMomentum,
                                  float newYMomentum, float newMaxSpeed = 100, float newGravity = GRAVITY, float newFriction = FRICTION, int newLifetime = 2147483647);

    unsigned int type();

    bool doesCollide(float otherX, float otherY, int otherType);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

#endif //PHYSICALENTITIES_HPP
