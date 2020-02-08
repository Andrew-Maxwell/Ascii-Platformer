#include "meta.hpp"
#include "entitymeta.hpp"

#ifndef DUMMYENTITY_HPP
#define DUMMYENTITY_HPP

using namespace rapidjson;

/*****************************************************************************/
//dummyEntity
//An entity that doesn't do anything. Used in mouse interface, mostly.
/*****************************************************************************/

class dummyEntity : public entity {

    char toPrint[2] = " ";
    int lifetime;

    public:

    dummyEntity(  float newX, float newY, Color newTint,
                    float newSizeFactor, char newC, int newLifetime = 10000000);

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};

#endif //DUMMYENTITY_HPP
