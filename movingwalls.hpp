#ifndef MOVINGWALLS_HPP
#define MOVINGWALLS_HPP

#include "world.hpp"
#include "effects.hpp"
#include "entity.hpp"
#include "meta.hpp"

using namespace rapidjson;

class snakeWall : virtual public collideable {

    bool loop;
    int forwardChannel, reverseChannel;
    int snakeHead, snakeLength;
    int pushPoint, directionPoint;
    bool swapDirectionPoint;
    int ticksPerMove;
    string display;

    int newDirection, oldDirection;
    vector<intVector2> points;

    public:

    explicit snakeWall(float newX, float newY, Color newTint, float newSizeFactor, int newSnakeLength, int newSnakeHead, int newTicksPerMove, bool newLoop, int newForwardChannel, int newReverseChannel, string newDisplay);

    void addPoint(int px, int py);

    void finish();

    unsigned int type();

    bool doesCollide(float otherX, float otherY, int otherType, unsigned int otherID);

    collision getCollision(float otherX, float otherY, int otherType, unsigned int otherID);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

class activeWall : public entity {

    int width, height, channel;
    string display = "";
    bool active = false, activated = false;

    public:

    explicit activeWall(  float newX, float newY, Color newTint, float newSizeFactor, int newWidth, int newHeight, int newChannel, int newDisplay);

    unsigned int type();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};
/*
class newTestEntity : virtual public collideable {

    int health, hurtTimer = 0;
    int damage;
    bool isDead = false;

    public:

    explicit newTestEntity(  float newX, float newY, Color newTint, float newSizeFactor);

    unsigned int type();

    bool doesCollide(float otherX, float otherY, int otherType, unsigned int otherID);

    collision getCollision(float otherX, float otherY, int otherType, unsigned int otherID);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};
*/


#endif //MOVINGWALLS_HPP
