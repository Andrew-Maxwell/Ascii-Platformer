#include "entity.hpp"

/******************************************************************************/
//Virtual entity parent class.
/******************************************************************************/

entity::entity(float newx, float newy, Color newTint, float newScale) {
    x = newx;
    y = newy;
    tint = newTint;
    scale = newScale;
    id = ++entityCounter;
}

void entity::setLighting(bool newDoLighting, bool newDoHighlight) {
    doLighting = newDoLighting;
    doHighlight = newDoHighlight;
}
