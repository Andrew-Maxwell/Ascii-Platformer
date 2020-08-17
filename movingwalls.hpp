#ifndef MOVINGWALLS_HPP
#define MOVINGWALLS_HPP

#include "world.hpp"
#include "effects.hpp"
#include "entity.hpp"
#include "meta.hpp"
#include "gamelayer.hpp"

using namespace rapidjson;

/*****************************************************************************/
//Snakewall
//A wall that moves like the snake in snake (but also diagonally)
/*****************************************************************************/

class snakeWall : virtual public entity {

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

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

/*****************************************************************************/
//activeWall
//Appears only when triggered
/*****************************************************************************/

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

/*****************************************************************************/
//elevator
//A block with fixed size and shape which can move in multiple directions
//Takes any entities on top of it with it.
/*****************************************************************************/

class elevator : virtual public gameLayer {

    vector<Vector2> points;
    float speed;
    Vector2 move;
    int nextPoint = 0;
    float width, height;
    int forwardChannel, reverseChannel;
    bool loop;

    public:

    explicit elevator(float newX, float newY, Color newTint, float newScale, float newWidth, float newHeight, float newSpeed, bool newLoop, int newForwardChannel, int newReverseChannel, gameLayer& newLayer);

    void addPoint(Vector2 newPoint);

    void finish(int startingPoint);

    unsigned int type();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

/*****************************************************************************/
//physicalBlock
//A solid block which has physical attributes (gravity, momentum)
/*****************************************************************************/

class physicalBlock : public physicalEntity, virtual public gameLayer {

    float oldX, oldY;

    public:

    explicit physicalBlock(float newX, float newY, Color newTint, float newScale, float newWidth, float newHeight, float newElasticity, float newXMomentum, float newYMomentum, float newMaxSpeed, float newGravity, float newFriction, gameLayer& newLayer);

    unsigned int type();

    void tickSet();

    void tickGet();

    bool stopColliding();

    bool finalize();

    void print();

};

#endif //MOVINGWALLS_HPP
