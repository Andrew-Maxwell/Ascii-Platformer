#include "door.hpp"

/*****************************************************************************/
//Door
//Triggers a transition to another room when interacted with.
/*****************************************************************************/

    door::door(  float newX, float newY, Color newTint,
                    float newSizeFactor, string newDestinationRoom, float newDestX, float newDestY) :
        entity(newX, newY, newTint, newSizeFactor),
        destinationX(newDestX),
        destinationY(newDestY),
        destinationRoom(newDestinationRoom) {}

    unsigned int door::type() {
        return DOORTYPE;
    }

    bool door::doesCollide(float otherX, float otherY, int otherType) {
        return (otherType == PLAYERTYPE && otherX > x - 1 && otherX < x + 1 && otherY > y - 1 && otherY < y + 1);
    }

    collision door::getCollision(float otherX, float otherY, int otherType) {
        return collision(type(), 0, destinationX, destinationY, destinationRoom);
    }

    bool door::stopColliding() {
        return false;
    }

    void door::tickSet() {}

    void door::tickGet() {}

    bool door::finalize() {return false;}

    void door::print() {
        theCanvas -> draw(x, y, tint, sizeFactor, "D");
    }

