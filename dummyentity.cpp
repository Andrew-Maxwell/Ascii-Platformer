#include "dummyentity.hpp"

using namespace rapidjson;

/*****************************************************************************/
//dummyEntity
//An entity that doesn't do anything. Used in mouse interface, mostly.
/*****************************************************************************/

    dummyEntity::dummyEntity(  float newX, float newY,  Color newTint,
                    float newSizeFactor, char newC, int newLifetime) :
        entity(newX, newY, newTint, newSizeFactor),
        lifetime(newLifetime) {
            toPrint[0] = newC;
            type = -1;
        }

    void dummyEntity::tickSet() {}

    void dummyEntity::tickGet() {}

    bool dummyEntity::finalize() {return --lifetime < 0;}

    void dummyEntity::print(float cameraX, float cameraY, Font displayFont)  {
        myDrawText(displayFont, toPrint, (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
    }
