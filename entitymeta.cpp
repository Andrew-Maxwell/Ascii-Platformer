#include "entitymeta.hpp"

/******************************************************************************/
//Virtual entity parent class.
/******************************************************************************/

entity::entity(float newx, float newy, Color newTint, float newSizeFactor) {
    x = newx;
    y = newy;
    tint = newTint;
    sizeFactor = newSizeFactor;
}

/******************************************************************************/
//A list for containing entities. Every tick, every entity in the list has its four functions called
//once. Also, entities can contain entityLists (e.g. an entity containing the bullets it's fired) as
//long as the four functions are called by that entity's four functions.
/******************************************************************************/

    entityList::~entityList() {
        while (entities.begin() != entities.end()) {
            if ((*entities.begin()) -> type != 1) { //Don't delete the player!
                delete (*entities.begin());
            }
            entities.erase(entities.begin());
        }
    }

    void entityList::clear() {
        while (entities.begin() != entities.end()) {
            delete (*entities.begin());
            entities.erase(entities.begin());
        }
        entities.clear();
    }

    int entityList::size() {
        return entities.size();
    }

    void entityList::tickSet(collider& col) {
        list<entity*>::iterator e = entities.begin();
        while (e != entities.end()) {
            (*e) -> tickSet(col);
            e++;
        }
    }

    void entityList::tickGet(collider& col) {
        list<entity*>::iterator e = entities.begin();
        while (e != entities.end()) {
            (*e) -> tickGet(col);
            e++;
        }
    }

    void entityList::finalize() {
        list<entity*>::iterator e = entities.begin();
        while (e != entities.end()) {
            if ((*e) -> finalize()) {
                delete *e;
                e = entities.erase(e);
            }
            else {
                e++;
            }
        }
    }

    void entityList::print(float cameraX, float cameraY, Font displayFont) {
        list<entity*>::iterator e = entities.begin();
        while (e != entities.end()) {
            (*e) -> print(cameraX, cameraY, displayFont);
            e++;
        }
    }

    void entityList::addEntity(entity* toAdd) {
        entities.push_back(toAdd);
    }
