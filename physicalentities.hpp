#ifndef PHYSICALENTITIES_HPP
#define PHYSICALENTITIES_HPP

#include "world.hpp"
#include "entity.hpp"
#include "meta.hpp"
#include "particles.hpp"

using namespace rapidjson;

/******************************************************************************/
//physicalParticle
//An entity to which physics (gravity and not travelling through solid objects)
//applies -- loosely.
/******************************************************************************/

class physicalParticle : virtual public collideable, public particle {

    protected:

    bool isUnderWater = false;
    bool shouldDelete = false, dynamicChar;
    float elasticity, xMomentum, yMomentum, maxSpeed, gravity, friction;
    int xSign = 1, ySign = 1;
    int lifetime;

    public:

    explicit physicalParticle( float newx, float newy, Color newTint, float newSizeFactor, int displayChar, float elasticity, float newXMomentum,
                                  float newYMomentum, float newMaxSpeed = 100, float newGravity = GRAVITY, float newFriction = FRICTION, int newLifetime = 2147483647);

    bool doesCollide(float otherX, float otherY, int otherType);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

/******************************************************************************/
//realPhysicalEntity
//An entity to which physics (gravity and not travelling through solid objects)
//applies, more rigorously.
/******************************************************************************/

class realPhysicalEntity : virtual public collideable {

    protected:

    bool lastTickUnderWater = false, isUnderWater = false;
    float elasticity, xMomentum, yMomentum, maxSpeed, gravity, friction;
    int xSign = 1, ySign = 1;
    float width = 0.8;

    public:

    explicit realPhysicalEntity(float newx, float newy, Color newTint, float newSizeFactor, float elasticity, float newXMomentum,
                                float newYMomentum, float newMaxSpeed = 100, float newGravity = GRAVITY, float newFriction = FRICTION);

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision(float otherX, float otherY, int otherType);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

#endif //PHYSICALENTITIES_HPP
