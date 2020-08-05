#include "world.hpp"
#include <iomanip>

/******************************************************************************/
/*Collidor*/
/******************************************************************************/

    //Constructor

    collider::collider(float newX, float newY, string newFileName) :
        entity(newX, newY, WHITE, 1),
        layer(newX, newY, WHITE, 1, newFileName)
    {
        if (fileName != "") {
            layer::readLayer();
        }
    }

    //destructor

    collider::~collider() {
        while (entities.begin() != entities.end()) {
            if ((*entities.begin()) -> type() != PLAYERTYPE &&
                (*entities.begin()) -> type() != GAMELAYERTYPE &&
                (*entities.begin()) -> type() != LAYERTYPE) {
                delete (*entities.begin());
            }
            entities.erase(entities.begin());
        }
    }

    void collider::clear() {
        while (entities.begin() != entities.end()) {
            if ((*entities.begin()) -> type() != PLAYERTYPE &&
                (*entities.begin()) -> type() != GAMELAYERTYPE &&
                (*entities.begin()) -> type() != LAYERTYPE) {
                delete (*entities.begin());
            }
            entities.erase(entities.begin());
        }
        entities.clear();
        collideables.clear();
        particles.clear();
        hudEntities.clear();
    }

    //Tick functions

    void collider::tickSet() {

        if (STATS && tickCounter % 60 == 0) {
            cout << "Entities: " << entities.size() << " ";
        }

        rectangles.clear();

        //(Formerly entitylist functionality)

        list<entity*>::iterator e = entities.begin();
        while (e != entities.end()) {
            (*e) -> tickSet();
            e++;
        }

        //Check for collisions between any two pairs of collideables.

        list<collideable*>::iterator iter1 = collideables.begin();
        while (iter1 != collideables.end()) {
            list<collideable*>::iterator iter2 = iter1;
            iter2++;
            while (iter2 != collideables.end()) {
                if ((*iter1) -> doesCollide((*iter2) -> x, (*iter2) -> y, (*iter2) -> type(), (*iter2) -> id)) {
                    (*iter2) -> addCollision((*iter1) -> getCollision((*iter2) -> x, (*iter2) -> y, (*iter2) -> type(), (*iter2) -> id));
                }
                if ((*iter2) -> doesCollide((*iter1) -> x, (*iter1) -> y, (*iter1) -> type(), (*iter1) -> id)) {
                    (*iter1) -> addCollision((*iter2) -> getCollision((*iter1) -> x, (*iter1) -> y, (*iter1) -> type(), (*iter1) -> id));
                }
                iter2++;
            }
            iter1++;
        }

        //Check for one-way collisions from collideables to particles

        list<collideable*>::iterator iter = collideables.begin();
        while (iter != collideables.end()) {
            list<collideable*>::iterator partIter = particles.begin();
            while (partIter != particles.end()) {
                if ((*iter) -> doesCollide((*partIter) -> x, (*partIter) -> y, (*partIter) -> type(), (*partIter) -> id)) {
                    (*partIter) -> addCollision((*iter) -> getCollision((*partIter) -> x, (*partIter) -> y, (*partIter) -> type(), (*partIter) -> id));
                }
                partIter++;
            }
            iter++;
        }

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

        //Clear hudEntities
        list<hudEntity*>::iterator h = hudEntities.begin();
        while (h != hudEntities.end()) {
            if ((*h) -> finalize()) {
                h = hudEntities.erase(h);
            }
            else {
                h++;
            }
        }

        //Clear the channel bus

        for (int i = 0; i < 512; i++) {
            channel[i] = false;
        }

        //Manage toggling
        //togglingChannel are incremented when toggleChannel() is still being called
        //If togglingChannel is > 0, then it's multiple consecutive calls and should be ignored.
        for (int i = 0; i < 512; i++) {
            if (togglingChannel[i]) {
                togglingChannel[i]--;
            }
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
        for (movingRectangle rect : rectangles) {
            theScreen -> drawBox(rect.x1 + rect.dX, rect.y1 + rect.dY, rect.x2 - rect.x1, rect.y2 - rect.y1, RED, 1, false);
        }
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

    void collider::addRectangle(float x1, float y1, float width, float height, float dX, float dY) {
        movingRectangle newRect;
        newRect.x1 = x1;
        newRect.y1 = y1;
        newRect.x2 = x1 + width;
        newRect.y2 = y1 + height;
        newRect.dX = dX;
        newRect.dY = dY;
        rectangles.push_back(newRect);
    }

    //Used to tell entities where they are in the list at creation time

    list<entity*>::iterator collider::getZPosition() {
        list<entity*>::iterator zPosition = entities.end();
        zPosition--;
        return zPosition;
    }

    //Physical tilemap functions

    Vector2 collider::go(Vector2 start, Vector2 d) {
        //Determine whether there are any solid tiles in the way
        Vector2 pos = start;
        float numSteps = int(max(abs(d.x), abs(d.y))) + 1;
        float xStep = d.x / numSteps;
        float yStep = d.y / numSteps;
        float buf = 0.1;
        for (int i = 0; i < numSteps; i++) {
            if (xStep != 0 && (isSolid(int(pos.x + xStep + buf + 0.9 * (xStep > 0)), int(pos.y + buf))
            || isSolid(int(pos.x + xStep + buf + 0.9 * (xStep > 0)), int(pos.y + 1 - buf)))) {
                pos.x = floor(pos.x + xStep) + (xStep < 0);
                xStep = 0;
            }
            else {
                pos.x += xStep;
            }
            if (yStep != 0 && (isSolid(int(pos.x + buf), int(pos.y + yStep + buf + 0.9 * (yStep > 0)))
            || isSolid(int(pos.x + 1 - buf), int(pos.y + yStep + buf + 0.9 * (yStep > 0))))) {
                pos.y = floor(pos.y + yStep) + (yStep < 0);
                yStep = 0;
            }
            else {
                pos.y += yStep;
            }
        }

        float width = 1, height = 1;



        for (movingRectangle rect : rectangles) {
            float oldX1 = rect.x1 - width;
            float oldY1 = rect.y1 - height;
            float oldX2 = rect.x2;
            float oldY2 = rect.y2;
            float newX1 = oldX1 + rect.dX;
            float newY1 = oldY1 + rect.dY;
            float newX2 = oldX2 + rect.dX;
            float newY2 = oldY2 + rect.dY;
            if ((start.x <= oldX1) != (pos.x <= newX1)) {
                float yIntersect = start.y + (newX1 - start.x) * d.y / d.x;
                if (newY1 < yIntersect && yIntersect < newY2) {
                    pos.x = newX1;
                }
            }
            if ((start.x >= oldX2) != (pos.x >= oldX2)) {
                float yIntersect = start.y + (newX2 - start.x) * d.y / d.x;
                if (newY1 < yIntersect && yIntersect < newY2) {
                    pos.x = newX2;
                }
            }
            cout << endl << "d.y " << d.y << "rect.y1 " << rect.y1 << " ";
            if (start.y == oldY1) {
                cout << setprecision(10) << start.y << " == " << oldY1 << endl;
            }
            else {
                cout << setprecision(10) << start.y << " != " << oldY1 << endl;
            }
            if (oldY1 < 4) {
                cout << "*******************************************************\n";
            }
            cout << start.y << " <= " << oldY1 << " != " << pos.y << " <= " << newY1 << " " << (start.y <= oldY1) << " " << (pos.y <= newY1);
            if ((start.y <= oldY1) != (pos.y <= newY1)) {
                cout << "all good";
                float xIntersect = start.x + (newY1 - start.y) * d.x / d.y;
                if (newX1 < xIntersect && xIntersect < newX2) {
                    pos.y = newY1;
                    cout << "New pos.y (up) " << pos.y;
                }
            }
            else {
                cout << "Fell through!";
            }
            if ((start.y >= oldY2) != (pos.y >= newY2)) {
                float xIntersect = start.x + (newY2 - start.y) * d.x / d.y;
                if (newX1 - width < xIntersect && xIntersect < newX2) {
                    pos.y = newY2;
                }
            }
            cout << endl;
        }


/*
        //For each side of rectangle:
        //First check if the side is passing by the object (should push object)
        //If not, check if object would cross side (should stop object)
        for (movingRectangle rect : rectangles) {
            float oldX1 = rect.x1 - width;
            float oldY1 = rect.y1 - height;
            float oldX2 = rect.x2;
            float oldY2 = rect.y2;
            float newX1 = oldX1 + rect.dX;
            float newY1 = oldY1 + rect.dY;
            float newX2 = oldX2 + rect.dX;
            float newY2 = oldY2 + rect.dY;

            //left side
            if (((pos.x <= oldX1) != (pos.x <= newX1)) && oldY1 < pos.y && pos.y < oldY2) {
                pos.x = newX1;
            }
            else if ((start.x <= newX1) != (pos.x <= newX1)) {
                float yIntersect = start.y + (newX1 - start.x) * d.y / d.x;
                if (newY1 < yIntersect && yIntersect < newY2) {
                    pos.x = newX1;
                }
            }
            //right side
            if (((pos.x >= oldX2) != (pos.x >= newX2)) && oldY1 < pos.y && pos.y < oldY2) {
                pos.x = newX2;
            }
            else if ((start.x >= newX2) != (pos.x >= newX2)) {
                float yIntersect = start.y + (newX2 - start.x) * d.y / d.x;
                if (newY1 < yIntersect && yIntersect < newY2) {
                    pos.x = newX2;
                }
            }
            //top side
            if (((pos.y <= oldY1) != (pos.y <= newY1)) && oldX1 < pos.x && pos.x < oldX2) {
                pos.y = newY1;
            }
            else if ((start.y <= newY1) != (pos.y <= newY1)) {
                float xIntersect = start.x + (newY1 - start.y) * d.x / d.y;
                if (newX1 < xIntersect && xIntersect < newX2) {
                    pos.y = newY1;
                }
            }
            //bottom side
            if (((pos.y >= oldY2) != (pos.y >= newY2)) && oldX1 < pos.x && pos.x < oldX2) {
                pos.y = newY2;
            }
            else if ((start.y >= newY2) != (pos.y >= newY2)) {
                float xIntersect = start.x + (newY2 - start.y) * d.x / d.y;
                if (newX1 < xIntersect && xIntersect < newX2) {
                    pos.y = newY2;
                }
            }
        }*/
        return pos;
    }



    bool collider::isSolid(int checkX, int checkY) {
        if (checkY >= 0 && checkY < screen.size() && checkX >= 0 && checkX < screen[checkY].size()) {
            return screen[checkY][checkX] == 's' || screen[checkY][checkX] == '8';
        }
        return false;
    }

    void collider::setSolid(int xPoint, int yPoint, char solid) {
        if (yPoint >= 0 && yPoint < screen.size() && xPoint >= 0 && xPoint < screen[yPoint].size()) {
            screen[yPoint][xPoint] = solid;
        }
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

    void collider::setChannel(int freq, bool newChannel) {
        if (newChannel && freq <= 255) {
            interceptedCodes.insert(freq);
        }
        channel[freq] = true;
    }

    void collider::toggleChannel(int freq, bool newChannel) {
        if (newChannel && freq <= 255) {
            interceptedCodes.insert(freq);
        }
        if (!togglingChannel[freq]) {
            togglingChannel[freq] = 2;
            persistentChannel[freq] = !persistentChannel[freq];
        }
        else {  //Multiple consecutive calls to toggleChannel are ignored.
            togglingChannel[freq]++;
        }
    }

    bool collider::getChannel(int freq) {
        if (freq >= 0 && freq < 512) {
            return channel[freq] || persistentChannel[freq];
        }
        return false;
    }

    set<uint8_t> collider::getInterceptedChannels() {
        return interceptedCodes;
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
