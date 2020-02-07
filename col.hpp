#ifndef COL_HPP
#define COL_HPP

#include "entitymeta.hpp"
#include "layer.hpp"
#include "meta.hpp"

using namespace rapidjson;

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

#endif //COL_HPP

