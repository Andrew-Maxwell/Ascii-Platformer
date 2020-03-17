#ifndef SAVEPOINT_HPP
#define SAVEPOINT_HPP

#include "world.hpp"
#include "effects.hpp"
#include "entity.hpp"
#include "particles.hpp"

using namespace rapidjson;

/*****************************************************************************/
//SavePoint
//Saves the game when interacted with.
/*****************************************************************************/

class savePoint : public collideable {

    bool savedGame = false;
    

    public:

    explicit savePoint(float newX, float newY, Color newTint, float newSizeFactor);

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision(float otherX, float otherY, int otherType);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

#endif //SAVEPOINT_HPP
