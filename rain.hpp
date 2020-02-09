#ifndef RAIN_HPP
#define RAIN_HPP

#include "col.hpp"
#include "entitymeta.hpp"
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

    void tickGet(collider& col);

};


/******************************************************************************/
//rain
//Constantly spawns particles above the top of the screen
/******************************************************************************/

class rain : public entity {

    entityList* eList;
    float dropsPerTick, dropBuffer = 0, xMomentum;
    bool firstTick;
    bool isSnow;

    public:

    rain(float newX, float newY, Color newTint, float newSizeFactor, entityList* newEList, float newDropsPerTick, float newXMomentum, bool isSnow);

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);

};

#endif //RAIN_HPP
