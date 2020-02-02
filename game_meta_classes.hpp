#ifndef GAME_META_CLASSES_HPP
#define GAME_META_CLASSES_HPP

#include "meta.hpp"

using namespace std;

/******************************************************************************/
/*gameLayer
Implements layer functionality (e.g. animations) which the editor doesn't use.*/
/******************************************************************************/

class gameLayer : public layer {
    
    public:
    
    gameLayer(float newx, float newy, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, string newFileName);
  
};

/******************************************************************************/
//Collision
//A sort of message between entities
/******************************************************************************/

struct collision {
    int type;
    int damage;
    float xVal;
    float yVal;
    string message;

    collision(int newType = 0, int newDamage = 0, float newXVal = 0, float newYVal = 0, string newMessage = "") :
        type(newType),
        damage(newDamage),
        xVal(newXVal),
        yVal(newYVal),
        message(newMessage) {}
};

/******************************************************************************/
//Collideable
//An entity that can interact directly with other entities via the Collider.
/******************************************************************************/

class collideable : virtual public entity {

    protected:

    list<collision> collisions;

    public:

    collideable() {}

    //Collision function
    //If entity A "initiates" a collision with entity B, then the following functions are called during tickSet:

    //B's doesCollide() is called with A's data to check if A is close enough to collide

    virtual bool doesCollide(float otherX, float otherY, int otherType) {cerr << "collideable::doesCollide() called. This function should be overridden."; return false;}

    //B's getCollision is used to get a collision object containing how A should respond to colliding with B

    virtual collision getCollision() {cerr << "collideable::getCollision() called. This function should be overridden."; return collision();}

    //This function is called to add the collision object from B to A's collision list. Then, A responds during tickGet.

    void addCollision(collision newCollision) {
        collisions.push_back(newCollision);
    }

    //Return true when the collideable should be removed from collision list.

    virtual bool stopColliding() {cerr << "collideable::stopCollision called. This function should be overridden."; return false;}
};

/******************************************************************************/
/*Collidor*/
/******************************************************************************/

class collider : public layer {

    //Collideables can exchange collisions and pass collisions to particles
    //Particles can't pass collisions to anything

    vector<collideable*> collideables;
    vector<collideable*> particles;
    bool channel[512] = {false};

    public:

    //Constructor

    collider(   float newX, float newY, string fileName = "collider.txt");

    //Accessors

    void addCollideable(collideable* newCollideable);

    void addParticle(collideable* newParticle);

    bool isSolid(int row, int col);

    bool isLiquid(int row, int col);
    
    int getPlayerDamage(int row, int col);
    
    int getDamage(int row, int col);

    float checkHeight(float checkX, float checkY);

    float getFloorLevel(float entityX, float entityY);

    void setChannel(int freq);

    bool getChannel(int freq);

    //tickSet, tickGet, and finalize are each called here AFTER they have been called on all other entities.

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();
};

/******************************************************************************/
//Particle
//Entity represented by any char that moves in a predefined direction until its lifetime runs out.
//If the character passed to constructor is 0, then a character is chosen based on direction.
/******************************************************************************/

class particle : virtual public entity {

    protected:

    float xSpeed, ySpeed;
    int lifetime;
    int toPrint;

    void setDirection();

    public:

    particle(  float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                        float newSizeFactor, float newXSpeed, float newYSpeed, int c, int newLifetime);

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};


/******************************************************************************/
//lightPhysicalEntity
//An entity to which physics (gravity and not travelling through solid objects)
//applies -- loosely.
/******************************************************************************/

class lightPhysicalEntity : virtual public entity {

    protected:

    float elasticity, xMomentum, yMomentum, maxSpeed, gravity, friction;
    int xSign = 1, ySign = 1;

    public:

    explicit lightPhysicalEntity( float newx, float newy, uint8_t R, uint8_t G, uint8_t B,
                                  uint8_t A, float newSizeFactor, float elasticity, float newXMomentum,
                                  float newYMomentum, float newMaxSpeed = 100, float newGravity = GRAVITY, float newFriction = FRICTION);

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print();
};

/******************************************************************************/
//realPhysicalEntity
//An entity to which physics (gravity and not travelling through solid objects)
//applies, more rigorously.
/******************************************************************************/

class realPhysicalEntity : virtual public entity {

    protected:

    float elasticity, xMomentum, yMomentum, maxSpeed, gravity, friction;
    int xSign = 1, ySign = 1;
    float width = 0.8;

    public:

    explicit realPhysicalEntity(float newx, float newy,  uint8_t R, uint8_t G, uint8_t B,
                                uint8_t A, float newSizeFactor, float elasticity, float newXMomentum,
                                float newYMomentum, float newMaxSpeed = 100, float newGravity = GRAVITY, float newFriction = FRICTION);

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print();
};

/*****************************************************************************/
//pickUp
//Any sort of collectable bonus
/*****************************************************************************/

class pickUp : public collideable {

    protected:

    bool collected = false;
    bool exploded = false;
    bool touch;
    int lifetime, ID;
    entityList myParticles;

    public:

    int displayChar;

    explicit pickUp(  float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                    float newSizeFactor, int newDisplayChar, int newLifetime, int newID, bool newTouch);

    bool doesCollide(float otherX, float otherY, int otherType);

    virtual collision getCollision();

    bool stopColliding();

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};


#endif //GAME_META_CLASSES_HPP


