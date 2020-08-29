#ifndef PARTICLES_HPP
#define PARTICLES_HPP

#include "world.hpp"
#include "entity.hpp"
#include "meta.hpp"
#include "physicalentities.hpp"

using namespace rapidjson;

/*
There are three mutually exclusive display modes.
In one mode, the particle constructor is passed a string and always displays that string.
In another, the particle constructor is passed a vector of ints which it switches between
    randomly at a specified speed.
In the third mode, the particle is passed an empty vector of ints and it changes between
    a hardcoded set of characters depending on direction it's moving.
*/

/******************************************************************************/
//Particle
//Entity represented by any char that moves in a predefined direction until its lifetime runs out.
//If the character passed to constructor is 0, then a character is chosen based on direction.
/******************************************************************************/

class particle : virtual public entity {

    protected:

    float xSpeed, ySpeed;
    int lifetime, startLifetime;
    int displayMix;
    vector<int> displayList;
    string display;
    bool fade;
    uint8_t startA;
    float xOffset = 0, yOffset = 0;

    void setDirection();

    Rectangle changeDisplay();

    public:

    particle(float newX, float newY, Color newTint, float newScale, float newXSpeed, float newYSpeed, vector<int> newDisplayList, int displayMix, int newLifetime, bool newFade = false);

    particle(float newX, float newY, Color newTint, float newScale, float newXSpeed, float newYSpeed, int newDisplay, int newLifetime, bool newFade = false);

    particle(float newX, float newY, Color newTint, float newScale, float newXSpeed, float newYSpeed, string newDisplay, int newLifetime, bool newFade = false);

    unsigned int type();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

/******************************************************************************/
//physicalParticle
//physicalEntity with some particle features
/******************************************************************************/

class physicalParticle : public physicalEntity, protected particle {

    protected:

    bool shouldDelete = false, dynamicChar;
    int lifetime;
    bool waterDelete, hitDelete, restDelete;
    float wobble;

    public:

    explicit physicalParticle(
        float newx, float newy, Color newTint, float newScale, vector<int> newDisplayList, int newDisplayMix, float elasticity,
        float newXMomentum, float newYMomentum, float newMaxSpeed = 100, bool newWaterDelete = false, bool newHitDelete = false, bool newRestDelete = false,
        float newWobble = 0, float newGravity = GRAVITY, float newFriction = FRICTION,
        int newLifetime = 2147483647, bool newFade = false);

    explicit physicalParticle(
        float newx, float newy, Color newTint, float newScale, int newDisplay, float elasticity,
        float newXMomentum, float newYMomentum, float newMaxSpeed = 100, bool newWaterDelete = false, bool newHitDelete = false, bool newRestDelete = false,
        float newWobble = 0, float newGravity = GRAVITY, float newFriction = FRICTION,
        int newLifetime = 2147483647, bool newFade = false);

    explicit physicalParticle(
        float newx, float newy, Color newTint, float newScale, string newDisplay, float elasticity,
        float newXMomentum, float newYMomentum, float newMaxSpeed = 100, bool newWaterDelete = false, bool newHitDelete = false, bool newRestDelete = false,
        float newWobble = 0, float newGravity = GRAVITY, float newFriction = FRICTION,
        int newLifetime = 2147483647, bool newFade = false);

    unsigned int type();

    bool doesCollide(float otherX, float otherY, int otherType, unsigned int otherID);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};


/******************************************************************************/
//particleSpawner
//Spawns particles randomly in a given zone
/******************************************************************************/

class particleSpawner : public entity, public entityParent {

    float width, height, depth;
    int channel;
    vector<int> displayChars;
    float elasticity, xMomentum, yMomentum, maxSpeed;
    bool waterDelete, hitDelete, restDelete;
    int mix;
    float wobble, gravity, friction;
    int lifetime;
    bool fade;

    float spawnsPerTick, spawnBuffer = 0;
    list<physicalParticle*> initParticles;

    public:

    particleSpawner(
        float newX, float newY, Color newTint, float newScale, float newWidth, float newHeight,
        float newDepth, int newChannel, vector<int>& newDisplayChars, int newMix, float newElasticity, float newXMomentum,
        float newYMomentum, float newMaxSpeed, bool newWaterDelete, bool newHitDelete, bool newRestDelete, float newWobble,
        float newGravity, float newFriction, int newLifetime, float newSpawnsPerTick, bool newFade);

    unsigned int type();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();

};

#endif //PARTICLES_HPP
