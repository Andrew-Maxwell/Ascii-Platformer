#ifndef PARTICLES_HPP
#define PARTICLES_HPP

#include "world.hpp"
#include "entity.hpp"
#include "meta.hpp"

using namespace rapidjson;

/******************************************************************************/
//Particle
//Entity represented by any char that moves in a predefined direction until its lifetime runs out.
//If the character passed to constructor is 0, then a character is chosen based on direction.
/******************************************************************************/

class particle : virtual public entity {

    protected:

    float xSpeed, ySpeed;
    int lifetime;
    char* toPrint;

    void setDirection();

    public:

    particle(  float newX, float newY, Color newTint, float newSizeFactor, float newXSpeed, float newYSpeed, int c, int newLifetime);

    ~particle();

    void tickSet();

    void tickGet();

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};

#endif //PARTICLES_HPP
