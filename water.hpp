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

class water : public collideable {

    deque<float> splashTimes, splashPositions, splashSizes;
    vector<float> surface;
    int width;
    float depth, wavelength, amplitude, k, omega;
    bool reverse;
    int time = 0;
    
    float sTime, location, size;

    public:

    explicit water( float newX, float newY, Color newTint, float newSizeFactor,  int newWidth, float newDepth, float newWavelength, float newAmplitude);

    bool doesCollide( float otherX, float otherY, int type);

    collision getCollision( float otherX, float otherY, int otherType);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};

#endif //WATER_HPP
