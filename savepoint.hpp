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
    entityList myParticles;

    public:

    explicit savePoint(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor);

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision();

    bool stopColliding();

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};

#endif //SAVEPOINT_HPP
