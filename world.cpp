#include "world.hpp"

/******************************************************************************/
/*Collidor*/
/******************************************************************************/

    //Constructor

    collider::collider(float newX, float newY, string fileName) :
        entity(newX, newY, {0, 0, 0, 0}, 1),
        layer(newX, newY, {0, 0, 0, 0}, 1, fileName)
    {
        if (fileName != "") {
            layer::readLayer();
        }
    }

    //destructor

    collider::~collider() {
        while (entities.begin() != entities.end()) {
            if ((*entities.begin()) -> type() != PLAYERTYPE &&
                (*entities.begin()) -> type() != GAMELAYERTYPE) {
                delete (*entities.begin());
            }
            entities.erase(entities.begin());
        }
    }

    void collider::clear() {
        while (entities.begin() != entities.end()) {
            if ((*entities.begin()) -> type() != PLAYERTYPE &&
                (*entities.begin()) -> type() != GAMELAYERTYPE) {
                delete (*entities.begin());
            }
            entities.erase(entities.begin());
        }
        entities.clear();
        collideables.clear();
        particles.clear();
    }

    //Tick functions

    void collider::tickSet() {

        if (STATS && tickCounter % 60 == 0) {
            cout << "Entities: " << entities.size() << " ";
        }

        //(Formerly entitylist functionality)

        list<entity*>::iterator e = entities.begin();
        while (e != entities.end()) {
            (*e) -> tickSet();
            e++;
        }

        //Check for collisions between any two pairs of collideables.

        list<collideable*>::iterator colIter1 = collideables.begin();
        while (colIter1 != collideables.end()) {
            list<collideable*>::iterator colIter2 = colIter1;
            colIter2++;
            while (colIter2 != collideables.end()) {
                if ((*colIter1) -> doesCollide((*colIter2) -> x, (*colIter2) -> y, (*colIter2) -> type())) {
                    (*colIter2) -> addCollision((*colIter1) -> getCollision((*colIter2) -> x, (*colIter2) -> y, (*colIter2) -> type()));
                }
                if ((*colIter2) -> doesCollide((*colIter1) -> x, (*colIter1) -> y, (*colIter1) -> type())) {
                    (*colIter1) -> addCollision((*colIter2) -> getCollision((*colIter1) -> x, (*colIter1) -> y, (*colIter1) -> type()));
                }
                colIter2++;
            }
            colIter1++;
        }

        //Check for one-way collisions from collideables to particles

        list<collideable*>::iterator colIter = collideables.begin();
        while (colIter != collideables.end()) {
            list<collideable*>::iterator partIter = particles.begin();
            while (partIter != particles.end()) {
                if ((*colIter) -> doesCollide((*partIter) -> x, (*partIter) -> y, (*partIter) -> type())) {
                    (*partIter) -> addCollision((*colIter) -> getCollision((*partIter) -> x, (*partIter) -> y, (*partIter) -> type()));
                }
                partIter++;
            }
            colIter++;
        }

/*
        for (unsigned int i = 0; i < collideables.size(); i++) {
            for (unsigned int j = i + 1; j < collideables.size(); j++) {
                if (collideables[i] -> doesCollide(collideables[j] -> x, collideables[j] -> y, collideables[j] -> type)) {
                    collideables[j] -> addCollision(collideables[i] -> getCollision(collideables[j] -> x, collideables[j] -> y, collideables[j] -> type));
                }
                if (collideables[j] -> doesCollide(collideables[i] -> x, collideables[i] -> y, collideables[i] -> type)) {
                    collideables[i] -> addCollision(collideables[j] -> getCollision(collideables[i] -> x, collideables[i] -> y, collideables[i] -> type));
                }
            }
        }

        //Check for collideables affecting particles

        for (unsigned int i = 0; i < collideables.size(); i++) {
            for (unsigned int j = 0; j < particles.size(); j++) {
                if (collideables[i] -> doesCollide(particles[j] -> x, particles[j] -> y, particles[j] -> type)) {
                    particles[j] -> addCollision(collideables[i] -> getCollision(particles[j] -> x, particles[j] -> y, particles[j] -> type));
                }
            }
        }
    */
    }


    void collider::tickGet() {

        //Former entitylist functionality

        list<entity*>::iterator e = entities.begin();
        while (e != entities.end()) {
            (*e) -> tickGet();
            e++;
        }

        //Iterate through and remove collideables for which stopColliding is true
        //called here because entities may be deleted in the finalize() step

        list<collideable*>::iterator c = collideables.begin();
        while (c != collideables.end()) {
            if ((*c) -> stopColliding()) {
                c = collideables.erase(c);
            }
            else {
                c++;
            }
        }

        list<collideable*>::iterator p = particles.begin();
        while (p != particles.end()) {
            if ((*p) -> stopColliding()) {
                p = particles.erase(p);
            }
            else {
                p++;
            }
        }

        //Clear the channel bus

        for (int i = 0; i < 512; i++) {
            channel[i] = false;
        }
    }

    bool collider::finalize() {
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
        return false;
    }

    void collider::print() {
        list<entity*>::iterator e = entities.begin();
        while (e != entities.end()) {
            (*e) -> print();
            e++;
        }
        list<hudEntity*>::iterator h = hudEntities.begin();
        while (h != hudEntities.end()) {
            (*h) -> printHud();
            h++;
        }
    }

    //Add functions

    void collider::addEntity(entity* toAdd) {
        entities.push_back(toAdd);
    }

    void collider::addEntity(entity* toAdd, list<entity*>::iterator zPosition) {
        entities.insert(zPosition, toAdd);
    }

    void collider::addHudEntity(hudEntity* toAdd) {
        hudEntities.push_back(toAdd);
    }

    void collider::addCollideable(collideable* newCollideable) {
        collideables.push_back(newCollideable);
        entities.push_back(newCollideable);
    }

    void collider::addCollideable(collideable* newCollideable, list<entity*>::iterator zPosition) {
        collideables.push_back(newCollideable);
        entities.insert(zPosition, newCollideable);
    }

    void collider::addParticle(collideable* newParticle) {
        particles.push_back(newParticle);
        entities.push_back(newParticle);
    }

    void collider::addParticle(collideable* newParticle, list<entity*>::iterator zPosition) {
        particles.push_back(newParticle);
        entities.insert(zPosition, newParticle);
    }

    //Used to tell entities where they are in the list at creation time

    list<entity*>::iterator collider::getZPosition() {
        list<entity*>::iterator zPosition = entities.end();
        zPosition--;
        return zPosition;
    }

    //Physical tilemap functions

    bool collider::isSolid(int checkX, int checkY) {
        if (checkY >= 0 && checkY < screen.size() && checkX >= 0 && checkX < screen[checkY].size()) {
            return screen[checkY][checkX] == 's' || screen[checkY][checkX] == '8';
        }
        return false;
    }

    bool collider::isLiquid(int checkX, int checkY) {
        if (checkY >= 0 && checkY < screen.size() && checkX >= 0 && checkX < screen[checkY].size()) {
            return screen[checkY][checkX] == 'w';
        }
        return false;
    }

    int collider::getPlayerDamage(int checkX, int checkY) {
        if (checkY >= 0 && checkY < screen.size() && checkX >= 0 && checkX < screen[checkY].size()) {
            if (screen[checkY][checkX] == 'x' || screen[checkY][checkX] == 'y') {
                return -2;
            }
            else if (screen[checkY][checkX] == 'X' || screen[checkY][checkX] == 'Y') {
                return -10;
            }
            else if (screen[checkY][checkX] == 'Z') {
                return -500;
            }
        }
        return 0;
    }
    
    int collider::getDamage(int checkX, int checkY) {
        if (checkY >= 0 && checkY < screen.size() && checkX >= 0 && checkX < screen[checkY].size()) {
            if (screen[checkY][checkX] == 'x') {
                return -2;
            }
            else if (screen[checkY][checkX] == 'X') {
                return -10;
            }
            else if (screen[checkY][checkX] == 'Z') {
                return -500;
            }
        }
        return 0;
    }

    //Broadcasts

    void collider::setChannel(int freq) {
        channel[freq] = true;
    }

    bool collider::getChannel(int freq) {
        return channel[freq];
    }

    //Stairs/ramps currently on hold.

    //Helper function for getFloorLevel. Used to check multiple points along the bottom of the entity.

    float collider::checkHeight(float checkX, float checkY) {
        if (checkY >= 0 && checkY < screen.size() && checkX >= 0 && checkX < screen[checkY].size()) {
            if (48 <= screen[checkY][checkX] && screen[checkY][checkX] < 56) {
                return (screen[checkY][checkX] - '0') / 8.0;
            }
            if (screen[checkY][checkX] == 'R' || screen[checkY][checkX] == 'r') {
                return fmod(checkX, 1.0);
            }
            if (screen[checkY][checkX] == 'L' || screen[checkY][checkX] == 'l') {
                return 1 - fmod(checkX, 1.0);
            }
        }
        return 0.0;
    }

    //Check for existence of ramp or stairs in order to update y-position

    float collider::getFloorLevel(float entityX, float entityY) {
        float level1 = checkHeight(entityX, entityY);
        float level2 = checkHeight(entityX + 1, entityY);
        return max(level1, level2);
    }
