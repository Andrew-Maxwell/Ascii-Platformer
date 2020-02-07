#include "dummyentity.hpp"

using namespace rapidjson;

/*****************************************************************************/
//dummyEntity
//An entity that doesn't do anything. Used in mouse interface, mostly.
/*****************************************************************************/

    dummyEntity::dummyEntity(  float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                    float newSizeFactor, char newC) :
        entity(newX, newY, R, G, B, A, newSizeFactor) {
            toPrint[0] = newC;
        }

    void dummyEntity::tickSet(collider& col) {}

    void dummyEntity::tickGet(collider& col) {}

    bool dummyEntity::finalize() {return false;}

    void dummyEntity::print(float cameraX, float cameraY, Font displayFont)  {
        myDrawText(displayFont, toPrint, (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
    }
