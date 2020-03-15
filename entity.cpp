#include "entity.hpp"

/******************************************************************************/
//Virtual entity parent class.
/******************************************************************************/

entity::entity(float newx, float newy, Color newTint, float newSizeFactor) {
    x = newx;
    y = newy;
    tint = newTint;
    sizeFactor = newSizeFactor;
}