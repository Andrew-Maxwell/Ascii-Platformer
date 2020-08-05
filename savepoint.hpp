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

class savePoint : public collideable, public entityParent {

    bool savedGame = false;
    bool gotCollision = false;

    public:

    explicit savePoint(float newX, float newY, Color newTint, float newScale);

    unsigned int type();

    bool doesCollide(float otherX, float otherY, int otherType, unsigned int otherID);

    collision getCollision(float otherX, float otherY, int otherType, unsigned int otherID);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

#endif //SAVEPOINT_HPP
