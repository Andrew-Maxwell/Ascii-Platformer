#include "dummyentity.hpp"

using namespace rapidjson;

/*****************************************************************************/
//dummyEntity
//An entity that doesn't do anything. Used in mouse interface, mostly.
/*****************************************************************************/

    dummyEntity::dummyEntity(  float newX, float newY,  Color newTint,
                    float newScale, char newC, int newLifetime) :
        entity(newX, newY, newTint, newScale),
        lifetime(newLifetime) {
            toPrint[0] = newC;
        }

    unsigned int dummyEntity::type() {
        return DUMMYENTITYTYPE;
    }

    void dummyEntity::tickSet() {}

    void dummyEntity::tickGet() {}

    bool dummyEntity::finalize() {return --lifetime < 0;}

    void dummyEntity::print()  {
        theScreen -> draw(x, y, tint, scale, toPrint, false);
    }
