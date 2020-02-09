#ifndef FORCEFIELD_HPP
#define FORCEFIELD_HPP

#include "col.hpp"
#include "effects.hpp"
#include "entitymeta.hpp"
#include "meta.hpp"
#include "particles.hpp"

using namespace rapidjson;

/*****************************************************************************/
//forceField
//Attracts or repels physical entities within its influende
/*****************************************************************************/

class forceField : public collideable {

    int channel;
    bool isOn;
    float power, range;
    entityList* eList;
    collision nextCollision;
    int tickCount;

    public:

    explicit forceField(float newX, float newY, Color newTint, float newSizeFactor, entityList* newEList, int newChannel, float newPower, float newRange);

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision();

    bool stopColliding();

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};

#endif //FORCEFIELD_HPP