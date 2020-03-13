#ifndef ENTITYMETA_HPP
#define ENTITYMETA_HPP

#include "meta.hpp"

using namespace rapidjson;

//forward declaration

class collider;

/******************************************************************************/
//Virtual entity parent class.
/******************************************************************************/

class entity {

    protected:

    Color tint;
    float sizeFactor = 1;

    public:

    int type = ERRORTYPE;
    int id;
    float x;
    float y;

    //Constructor and destructor

    entity(float newx, float newy, Color newTint, float newSizeFactor);

    entity() {}

    virtual ~entity() = default;

    //Tick functions
    //Move entities. Update collider

    virtual void tickSet(collider& col) {cerr << "Entity tickset called illegally. This function should be overridden!";}

    //Update entities based on new collider values

    virtual void tickGet(collider& col) {cerr << "Entity tickGet() called illegally. This function should be overridden!";}

    //perform additional cleanup. If this function returns true, then the entity is deleted by the entityList.

    virtual bool finalize() {cerr << "Entity finalize() called illegally. This function should be overridden!"; return false;}

    //Display the entity.

    virtual void print(float cameraX, float cameraY, Font displayFont) {cerr << "Entity print() called illegally. This function should be overridden!";}
};

/******************************************************************************/
//A list for containing entities. Every tick, every entity in the list has its four functions called
//once. Also, entities can contain entityLists (e.g. an entity containing the bullets it's fired) as
//long as the four functions are called by that entity's four functions.
/******************************************************************************/

class entityList {

    list<entity*> entities;

    public:

    ~entityList();

    void clear(); //Has the potential to cause big issues with collideables. should ONLY be called in one place - by player when changing rooms.

    int size();

    void tickSet(collider& col);

    void tickGet(collider& col);

    void finalize();

    void print(float cameraX, float cameraY, Font displayFont);

    void addEntity(entity* toAdd);
};

#endif //ENTITYMETA_HPP

