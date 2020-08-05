#ifndef FORCEFIELD_HPP
#define FORCEFIELD_HPP

#include "world.hpp"
#include "effects.hpp"
#include "entity.hpp"
#include "meta.hpp"
#include "particles.hpp"
#include "screen.hpp"

using namespace rapidjson;


/*****************************************************************************/
//linearField
//Forces entities in a certain direction when activated
/*****************************************************************************/

class linearField : public collideable {

    protected:

    int channel;
    bool isOn;
    float xPower, yPower;
    int width, height;
    RenderTexture2D tex;
    Color onTint;

    collision nextCollision;
    int tickCount;

    public:

    explicit linearField(float newX, float newY, Color newTint, Color newOnTint, float newScale,  int newChannel, float newXPower, float newYPower, int newWidth, int newHeight);

    ~linearField();

    unsigned int type();

    bool doesCollide(float otherX, float otherY, int otherType, unsigned int otherID);

    collision getCollision(float otherX, float otherY, int otherType, unsigned int otherID);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

/*****************************************************************************/
//forceField
//Attracts or repels physical entities within its influence 
/*****************************************************************************/

class forceField : public collideable, public entityParent {

    protected:

    int channel;
    bool isOn;
    float power, range;
    collision nextCollision;
    int tickCount;
    Color onTint;

    public:

    explicit forceField(float newX, float newY, Color newTint, Color newOnTint, float newScale,  int newChannel, float newPower, float newRange);

    unsigned int type();

    bool doesCollide(float otherX, float otherY, int otherType, unsigned int otherID);

    collision getCollision(float otherX, float otherY, int otherType, unsigned int otherID);

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

    explicit explosion(float newX, float newY, Color newTint, float newScale,  int newChannel, float newPower, float newRange);

    bool doesCollide(float otherX, float otherY, int otherType, unsigned int otherID);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

#endif //FORCEFIELD_HPP
