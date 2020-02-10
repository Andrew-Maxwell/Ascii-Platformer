#ifndef WATER_HPP
#define WATER_HPP

#include "col.hpp"
#include <deque>
#include "entitymeta.hpp"
#include "meta.hpp"

using namespace std;

/*****************************************************************************/
//Water
//Animates the surface of water
/*****************************************************************************/

class water : public collideable {

    deque<float> splashTimes, splashPositions, splashSizes;
    vector<float> surface;
    int width;
    float depth, wavelength, amplitude, k, omega;
    bool reverse;
    int time = 0;
    entityList* eList;
    float sTime, location, size;

    public:

    explicit water( float newX, float newY, Color newTint, float newSizeFactor, entityList* newEList, int newWidth, float newDepth, float newWavelength, float newAmplitude);

    bool doesCollide( float otherX, float otherY, int type);

    collision getCollision( float otherX, float otherY, int otherType);

    bool stopColliding();

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};

#endif //WATER_HPP
