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

    explicit snakeWall(float newX, float newY, Color newTint, float newScale, int newSnakeLength, int newSnakeHead, int newTicksPerMove, bool newLoop, int newForwardChannel, int newReverseChannel, string newDisplay);

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

    explicit activeWall(  float newX, float newY, Color newTint, float newScale, int newWidth, int newHeight, int newChannel, int newDisplay);

    unsigned int type();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

//TODO: change elevator to inherit from a layer

class elevator : virtual public collideable {

    vector<Vector2> points;
    int direction = 1;
    float speed;
    Vector2 move;
    int nextPoint = 0;
    int width, height;

    public:

    explicit elevator(float newX, float newY, Color newTint, float newScale, int newWidth, int newHeight, float newSpeed);

    void addPoint(Vector2 newPoint);

    void finish(int startingPoint);

    unsigned int type();

    bool doesCollide(float otherX, float otherY, int otherType, unsigned int otherID);

    collision getCollision(float otherX, float otherY, int otherType, unsigned int otherID);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

#endif //MOVINGWALLS_HPP
