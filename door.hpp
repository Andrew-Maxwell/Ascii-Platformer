#ifndef DOOR_HPP
#define DOOR_HPP

#include "col.hpp"
#include "entitymeta.hpp"
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

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision(float otherX, float otherY, int otherType);

    bool stopColliding();

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};

#endif //DOOR_HPP
