#include "meta.hpp"
#include "entity.hpp"
#include "screen.hpp"

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
                    float newScale, char newC, int newLifetime = 10000000);

    unsigned int type();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

#endif //DUMMYENTITY_HPP
