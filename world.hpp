#ifndef WORLD_HPP
#define WORLD_HPP

#include "meta.hpp"
#include "layer.hpp"

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
    float magnitude;
    string message;

    collision(int newType = 0, int newDamage = 0, float newXVal = 0, float newYVal = 0, string newMessage = "", float newMagnitude = 0) :
        type(newType),
        damage(newDamage),
        xVal(newXVal),
        yVal(newYVal),
        message(newMessage),
        magnitude(newMagnitude) {}
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

    virtual collision getCollision(float otherX = 0, float otherY = 0, int otherType = 0) {cerr << "collideable::getCollision() called. This function should be overridden."; return collision();}

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

    bool deleted = false;
    list<entity*> entities;
    list<collideable*> collideables;
    list<collideable*> particles;
    bool channel[512] = {false};

    public:

    //Constructor

    collider (float newX = 0.0, float newY = 0.0, string fileName = "");

    //destructor

    ~collider();

    void clear();

    //Tick functions

    void tickSet();

    void tickGet();

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);

    //Add functions

    void addEntity(entity* toAdd);

    void addCollideable(collideable* newCollideable);

    void addParticle(collideable* newParticle);

    //physical tilemap functions

    bool isSolid(int checkX, int checkY);

    bool isLiquid(int checkX, int checkY);
    
    int getPlayerDamage(int checkX, int checkY);
    
    int getDamage(int checkX, int checkY);

    float checkHeight(float checkX, float checkY);

    float getFloorLevel(float entityX, float entityY);

    //Broadcast functions

    void setChannel(int freq);

    bool getChannel(int freq);

};

#endif //WORLD_HPP

