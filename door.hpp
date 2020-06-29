#ifndef DOOR_HPP
#define DOOR_HPP

#include "world.hpp"
#include "entity.hpp"
#include "screen.hpp"
#include "meta.hpp"

using namespace rapidjson;

/*****************************************************************************/
//Door
//Triggers a transition to another room when interacted with.
/*****************************************************************************/

class door : public collideable {

    float destinationX, destinationY;
    string destinationRoom;

    public:

    explicit door(  float newX, float newY, Color newTint,
                    float newSizeFactor, string newDestinationRoom, float newDestX, float newDestY);

    unsigned int type();

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision(float otherX, float otherY, int otherType);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

#endif //DOOR_HPP
