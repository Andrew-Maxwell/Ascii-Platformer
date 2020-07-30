#ifndef WORLD_HPP
#define WORLD_HPP

#include "meta.hpp"
#include "layer.hpp"
#include <set>

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
    double magnitude;
    string message;
    unsigned int id;
    unsigned int flags;

    collision(int newType = 0, unsigned int newID = 0, int newDamage = 0, float newXVal = 0, float newYVal = 0, string newMessage = "", double newMagnitude = 0, unsigned int newFlags = 0) :
        type(newType),
        id(newID),
        damage(newDamage),
        xVal(newXVal),
        yVal(newYVal),
        message(newMessage),
        magnitude(newMagnitude),
        flags(newFlags) {}
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

    virtual bool doesCollide(float otherX, float otherY, int otherType, unsigned int otherID) {cerr << "collideable::doesCollide() called. This function should be overridden."; return false;}

    //B's getCollision is used to get a collision object containing how A should respond to colliding with B

    virtual collision getCollision(float otherX, float otherY, int otherType, unsigned int otherID) {cerr << "collideable::getCollision() called. This function should be overridden."; return collision();}

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
    list<hudEntity*> hudEntities;
    list<collideable*> collideables;
    list<collideable*> particles;
    bool channel[512] = {false};
    bool persistentChannel[512] = {false};
    uint8_t togglingChannel[512] = {0};
    set<uint8_t> interceptedCodes;

    public:

    //Constructor

    collider (float newX = 0.0, float newY = 0.0, string fileName = "");

    void readLayer();

    //destructor

    ~collider();

    void clear();

    //Tick functions

    void tickSet();

    void tickGet();

    bool finalize();

    void print();

    //Add functions

    void addEntity(entity* toAdd);

    void addEntity(entity* toAdd, list<entity*>::iterator zPosition);

    void addHudEntity(hudEntity* toAdd);

    void addCollideable(collideable* newCollideable);

    void addCollideable(collideable* newCollideable, list<entity*>::iterator zPosition);

    void addParticle(collideable* newParticle);

    void addParticle(collideable* newParticle, list<entity*>::iterator zPosition);

    //Used exclusively when creating entities to be added

    list<entity*>::iterator getZPosition();

    //physical tilemap functions

    bool isSolid(int checkX, int checkY);

    void setSolid(int x, int y, char solid);
    
    int getPlayerDamage(int checkX, int checkY);
    
    int getDamage(int checkX, int checkY);

    float checkHeight(float checkX, float checkY);

    float getFloorLevel(float entityX, float entityY);

    //Broadcast functions

    void setChannel(int freq, bool newChannel);

    void toggleChannel(int freq, bool newChannel);

    bool getChannel(int freq);

    set<uint8_t> getInterceptedChannels();
};

#endif //WORLD_HPP

