#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "meta.hpp"

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

    unsigned int id;
    float x;
    float y;

    //Constructor and destructor

    entity(float newx, float newy, Color newTint, float newSizeFactor);

    entity() {}

    virtual ~entity() = default;

    virtual unsigned int type() = 0;

    //Tick functions
    //Move entities. Update collider

    virtual void tickSet() {cout << "Error: entity::tickset() should be overloaded.";}

    //Update entities based on new collider values

    virtual void tickGet() {cout << "Error: entity::tickget() should be overloaded.";}

    //perform additional cleanup. If this function returns true, then the entity is deleted by the entityList.

    virtual bool finalize() {cout << "Error: entity::finalize() should be overloaded.";}

    //Display the entity.

    virtual void print() {cout << "Error: entity::print() should be overloaded.";}
};

#endif //ENTITY_HPP
