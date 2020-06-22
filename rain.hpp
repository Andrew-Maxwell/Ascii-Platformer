#ifndef RAIN_HPP
#define RAIN_HPP

#include "world.hpp"
#include "entity.hpp"
#include "meta.hpp"
#include "physicalentities.hpp"

using namespace rapidjson;

/******************************************************************************/
//drop
//Physical particle that disappears on contact with water.
/******************************************************************************/

class drop : public physicalParticle {

    public:

     explicit drop( float newx, float newy, Color newTint, float newSizeFactor, int displayChar, float elasticity, float newXMomentum,
                                  float newYMomentum, float newMaxSpeed = 100, float newGravity = GRAVITY, float newFriction = FRICTION, int newLifetime = 2147483647);

    unsigned int type();

    void tickGet();

};


/******************************************************************************/
//rain
//Constantly spawns particles above the top of the screen
/******************************************************************************/

class rain : public entity, public entityParent {

    float dropsPerTick, dropBuffer = 0, xMomentum;
    bool isSnow;

    public:

    rain(float newX, float newY, Color newTint, float newSizeFactor,  float newDropsPerTick, float newXMomentum, bool isSnow);

    unsigned int type();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();

};

#endif //RAIN_HPP
