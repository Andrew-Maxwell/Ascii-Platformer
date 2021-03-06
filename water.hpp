#ifndef WATER_HPP
#define WATER_HPP


#include <deque>
#include "world.hpp"
#include "entity.hpp"
#include "meta.hpp"

using namespace std;

/*****************************************************************************/
//Water
//Animates the surface of water
/*****************************************************************************/

class water : public collideable, public entityParent {

    deque<float> splashTimes, splashPositions, splashSizes;
    vector<float> surface, lastSurface;
    int width;
    float height, wavelength, amplitude, k, omega, pulseSpeed;
    bool reverse;
    float sTime;

    public:

    explicit water (float newX, float newY, Color newTint, float newScale,  int newWidth, float newheight, float newWavelength, float newAmplitude);

    unsigned int type ();

    float calculateSurface (float atX, int tickOffSet);

    float calculateSurfaceLastTick (float atX);

    bool doesCollide (float otherX, float otherY, int otherType, unsigned int otherID);

    collision getCollision (float otherX, float otherY, int otherType, unsigned int otherID);

    bool stopColliding ();

    void tickSet ();

    void tickGet ();

    bool finalize ();

    void print ();
};

#endif //WATER_HPP
