#ifndef ENDINGGATE_HPP
#define ENDINGGATE_HPP

#include "world.hpp"
#include "entity.hpp"
#include "meta.hpp"
#include "particles.hpp"
#include "canvas.hpp"

using namespace rapidjson;

/*****************************************************************************/
//endingGate
//an imposing gate that spawns particles that float slowly upwards.
/*****************************************************************************/

class endingGate : public collideable, public entityParent {

    int width, height;
    vector<string> toPrint;

    public:

    explicit endingGate(float newX, float newY, Color newTint,
                        float newSizeFactor,  int newWidth, int newHeight);

    unsigned int type();

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision(float otherX, float otherY, int otherType);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

#endif //ENDINGGATE_HPP
