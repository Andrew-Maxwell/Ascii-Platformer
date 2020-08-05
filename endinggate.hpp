#ifndef ENDINGGATE_HPP
#define ENDINGGATE_HPP

#include "world.hpp"
#include "entity.hpp"
#include "meta.hpp"
#include "particles.hpp"
#include "screen.hpp"

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
                        float newScale,  int newWidth, int newHeight);

    unsigned int type();

    bool doesCollide(float otherX, float otherY, int otherType, unsigned int otherID);

    collision getCollision(float otherX, float otherY, int otherType, unsigned int otherID);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

#endif //ENDINGGATE_HPP
