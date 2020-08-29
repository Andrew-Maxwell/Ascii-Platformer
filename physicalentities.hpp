#ifndef PHYSICALENTITIES_HPP
#define PHYSICALENTITIES_HPP

#include "world.hpp"
#include "entity.hpp"
#include "meta.hpp"

using namespace rapidjson;

/******************************************************************************/
//physicalEntity
//An entity to which physics (gravity and not travelling through solid objects)
//applies, more rigorously.
/******************************************************************************/

class physicalEntity : virtual public collideable {

    protected:

    bool hitX = false, hitY = false, onGround = false;
    bool isUnderWater = false, lastTickUnderWater = false;
    float elasticity, xMomentum, yMomentum, maxSpeed, gravity, friction;
    float width, height;

    public:

    explicit physicalEntity(float newx, float newy, Color newTint, float newScale, float newWidth, float newHeight, float elasticity, float newXMomentum,
                                float newYMomentum, float newMaxSpeed = 100, float newGravity = GRAVITY, float newFriction = FRICTION);

    unsigned int type();

    bool doesCollide(float otherX, float otherY, int otherType, unsigned int otherID);

    collision getCollision(float otherX, float otherY, int otherType, unsigned int otherID);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};


#endif //PHYSICALENTITIES_HPP
