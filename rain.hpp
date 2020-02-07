#ifndef RAIN_HPP
#define RAIN_HPP

#include "col.hpp"
#include "entitymeta.hpp"
#include "meta.hpp"
#include "particles.hpp"

using namespace rapidjson;

/******************************************************************************/
//rain
//Constantly spawns particles above the top of the screen
/******************************************************************************/

class rain : public entity {

    entityList raindrops;
    float dropsPerTick, dropBuffer = 0, xMomentum;
    bool firstTick;
    bool isSnow;

    public:

    rain(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, float newDropsPerTick, float newXMomentum, bool isSnow);

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);

};

#endif //RAIN_HPP
