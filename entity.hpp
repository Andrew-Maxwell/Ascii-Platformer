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
    bool doLighting = true, doHighlight = false;
    float scale = 1;

    public:

    unsigned int id;
    float x;
    float y;

    //Constructor and destructor

    entity(float newx, float newy, Color newTint, float newScale);

    entity() {}

    virtual ~entity() = default;

    virtual unsigned int type() = 0;

    //Tick functions
    //Move entities. Update collider

    virtual void tickSet() {cout << "Error: entity::tickset() should be overloaded.";}

    //Update entities based on new collider values

    virtual void tickGet() {cout << "Error: entity::tickget() should be overloaded.";}

    //perform additional cleanup. If this function returns true, then the entity is deleted by the entityList.

    virtual bool finalize() {cout << "Error: entity::finalize() should be overloaded."; return true;}

    //Display the entity.

    virtual void print() {cout << "Error: entity::print() should be overloaded.";}

    //In lieu of adding a new term to 20+ constructors

    void setLighting(bool newDoLighting, bool newDoHighlight);
};

class entityParent {

    protected:

    list<entity*>::iterator zPosition;

    public:

    void setZPosition(list<entity*>::iterator newZPosition) {
        zPosition = newZPosition;
    }

};

class hudEntity : virtual public entity {

    public:

    virtual void printHud() = 0;

};

#endif //ENTITY_HPP
