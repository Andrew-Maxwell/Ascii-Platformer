#ifndef FORCEFIELD_HPP
#define FORCEFIELD_HPP

#include "world.hpp"
#include "effects.hpp"
#include "entity.hpp"
#include "meta.hpp"
#include "particles.hpp"
#include "canvas.hpp"

using namespace rapidjson;

/*****************************************************************************/
//forceField
//Attracts or repels physical entities within its influende
/*****************************************************************************/

class forceField : public collideable {

    protected:

    int channel;
    bool isOn;
    float power, range;
    
    collision nextCollision;
    int tickCount;

    public:

    explicit forceField(float newX, float newY, Color newTint, float newSizeFactor,  int newChannel, float newPower, float newRange);

    unsigned int type();

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision(float otherX, float otherY, int otherType);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

/*****************************************************************************/
//explosion
//Attracts or repels physical entities for one tick, then disappears.
/*****************************************************************************/

class explosion : public forceField {

    bool exploding = false;

    public:

    explicit explosion(float newX, float newY, Color newTint, float newSizeFactor,  int newChannel, float newPower, float newRange);

    bool doesCollide(float otherX, float otherY, int otherType);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

#endif //FORCEFIELD_HPP
