#ifndef SAVEPOINT_HPP
#define SAVEPOINT_HPP

#include "col.hpp"
#include "effects.hpp"
#include "entitymeta.hpp"
#include "particles.hpp"

using namespace rapidjson;

/*****************************************************************************/
//SavePoint
//Saves the game when interacted with.
/*****************************************************************************/

class savePoint : public collideable {

    bool savedGame = false;
    entityList* eList;

    public:

    explicit savePoint(float newX, float newY, Color newTint, float newSizeFactor, entityList* newEList);

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision(float otherX, float otherY, int otherType);

    bool stopColliding();

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};

#endif //SAVEPOINT_HPP
