#ifndef PHYSICALENTITIES_HPP
#define PHYSICALENTITIES_HPP

#include "col.hpp"
#include "entitymeta.hpp"
#include "meta.hpp"

using namespace rapidjson;

/******************************************************************************/
//lightPhysicalEntity
//An entity to which physics (gravity and not travelling through solid objects)
//applies -- loosely.
/******************************************************************************/

class lightPhysicalEntity : virtual public entity {

    protected:

    float elasticity, xMomentum, yMomentum, maxSpeed, gravity, friction;
    int xSign = 1, ySign = 1;

    public:

    explicit lightPhysicalEntity( float newx, float newy, uint8_t R, uint8_t G, uint8_t B,
                                  uint8_t A, float newSizeFactor, float elasticity, float newXMomentum,
                                  float newYMomentum, float newMaxSpeed = 100, float newGravity = GRAVITY, float newFriction = FRICTION);

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print();
};

/******************************************************************************/
//realPhysicalEntity
//An entity to which physics (gravity and not travelling through solid objects)
//applies, more rigorously.
/******************************************************************************/

class realPhysicalEntity : virtual public entity {

    protected:

    float elasticity, xMomentum, yMomentum, maxSpeed, gravity, friction;
    int xSign = 1, ySign = 1;
    float width = 0.8;

    public:

    explicit realPhysicalEntity(float newx, float newy,  uint8_t R, uint8_t G, uint8_t B,
                                uint8_t A, float newSizeFactor, float elasticity, float newXMomentum,
                                float newYMomentum, float newMaxSpeed = 100, float newGravity = GRAVITY, float newFriction = FRICTION);

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print();
};

#endif //PHYSICALENTITIES_HPP
