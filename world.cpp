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
        //Print out movingRectangle visualizers
/*        list<movingRectangle>::iterator r = rectangles.begin();
        while (r != rectangles.end()) {
            theScreen -> drawBox(r -> oldX1, r -> oldY1, r -> oldX2 - r -> oldX1, r -> oldY2 - r -> oldY1, (Color){255, 0, 0, 128}, 1, false, false);
            theScreen -> drawBox(r -> newX1, r -> newY1, r -> newX2 - r -> newX1, r -> newY2 - r -> newY1, (Color){0, 255, 0, 128}, 1, false, false);
            r++;
        }*/
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

    void collider::addRectangle(movingRectangle newRectangle) {
        rectangles.push_back(newRectangle);
    }

    //Used to tell entities where they are in the list at creation time

    list<entity*>::iterator collider::getZPosition() {
        list<entity*>::iterator zPosition = entities.end();
        zPosition--;
        return zPosition;
    }


    //Given a starting position (start) and a movement (d), does an object collide with anything solid?
    Vector2 collider::go(Vector2 start, Vector2 d, float width, float height, bool& hitX, bool& hitY, float& xMomentum, float& yMomentum, float elasticity, unsigned int id) {
        hitX = hitY = false;
        //If on top of an elevator block, apply sideways force.
        for (movingRectangle r : rectangles) {
            if (r.id != id && start.y == r.oldY1 - height && start.x > r.oldX1 - width && start.x < r.oldX2) {
                d.x += (r.newX1 - r.oldX1);
                d.y += (r.newY1 - r.oldY1);
            }
        }

        //Determine whether there are any solid tiles in the way
        Vector2 next = start;
        float numSteps = ceil(max(abs(d.x), abs(d.y)));
        float xStep = d.x / numSteps;
        float yStep = d.y / numSteps;
        const float buf = 0.1;
        for (int i = 0; i < numSteps; i++) {
            if (!hitX && (isTileSolid(int(next.x + xStep + buf + (width - buf) * (xStep > 0)), int(next.y + buf))
            || isTileSolid(int(next.x + xStep + buf + (width - buf) * (xStep > 0)), int(next.y + height - buf)))) {
                if (xStep > 0) {
                    next.x = floor(next.x + xStep + width) - width;
                }
                else {
                    next.x = ceil(next.x + xStep);
                }
                hitX = true;
                xMomentum = xMomentum * -1 * elasticity;
            }
            else {
                next.x += xStep;
            }
            if (!hitY && (isTileSolid(int(next.x + buf), int(next.y + yStep + buf + (height - buf) * (yStep > 0)))
            || isTileSolid(int(next.x + width - buf), int(next.y + yStep + buf + (height - buf) * (yStep > 0))))) {
                if (yStep > 0) {
                    next.y = floor(next.y + yStep + height) - height;
                }
                else {
                    next.y = ceil(next.y + yStep);
                }
                hitY = true;
                yMomentum = yMomentum * -1 * elasticity;
                if (abs(yMomentum) < 0.01) {    //Kludge to prevent many very small bounces when elasticity is high
                    yMomentum = 0;
                }
            }
            else {
                next.y += yStep;
            }
        }

        float slope = (next.x - start.x == 0) ? 0 : (next.y - start.y) / (next.x - start.x);
        for (movingRectangle r : rectangles) {
            if (id != r.id) {
                if ((start.x <= r.oldX1 - width) && (next.x > r.newX1 - width)) {
                    float yIntersect = start.y + (r.oldX1 - start.x - width) * slope;
                    if (r.newY1 - height < yIntersect && yIntersect < r.newY2) {
                        xMomentum = xMomentum * -1 * elasticity + min(0.0f, max(r.newX1 - r.oldX1, r.newX1 - (next.x + width)));
                        next.x = r.newX1 - width;
                        hitX = true;
                    }
                }
                if ((start.x >= r.oldX2) && (next.x < r.newX2)) {
                    float yIntersect = start.y + (r.oldX2 - start.x) * slope;
                    if (r.newY1 - height < yIntersect && yIntersect < r.newY2) {
                        xMomentum = xMomentum * -1 * elasticity + max(0.0f, min(r.newX2 - r.oldX2, r.newX2 - next.x));
                        next.x = r.newX2;
                        hitX = true;
                    }
                }
                if ((start.y <= r.oldY1 - height) && (next.y > r.newY1 - height)) {
                    float xIntersect = start.x + (slope == 0 ? 0 : (r.oldY1 - height - start.y) / slope);
                    if (r.newX1 - width < xIntersect && xIntersect < r.newX2) {
                        yMomentum = yMomentum * -1 * elasticity;//+ min(0.0f, max(r.newY1 - r.oldY1, r.newY1 - (start.y + height)));
                        next.y = r.newY1 - height;
                        hitY = true;
                    }
                }
                if ((start.y >= r.oldY2) && (next.y < r.newY2)) {
                    float xIntersect = start.x + (slope == 0 ? 0 : (r.oldY2 - start.y) / slope);
                    if (r.newX1 - width < xIntersect && xIntersect < r.newX2) {
                        yMomentum = yMomentum * -1 * elasticity + max(0.0f, min(r.newY2 - r.oldY2, r.newY2 - start.y));
                        next.y = r.newY2;
                        hitY = true;
                    }
                }
                //failsafes
                if (next.x < r.newX1 && next.x + width > r.newX1 && next.y > r.newY1 - height && next.y < r.newY2) {
                    cout << "in block x1\n";
                    xMomentum = xMomentum * -1 * elasticity + min(0.0f, max(r.newX1 - r.oldX1, r.newX1 - (next.x + width)));
                    next.x = r.newX1 - width;
                    hitX = true;
                }
                if (next.x < r.newX2 && next.x + width > r.newX2 && next.y > r.newY1 - height && next.y < r.newY2) {
                    cout << "In block x2\n";
                    xMomentum = xMomentum * -1 * elasticity + max(0.0f, min(r.newX2 - r.oldX2, r.newX2 - next.x));
                    next.x = r.newX2;
                    hitX = true;
                }
                if (next.y < r.newY1 && next.y + height > r.newY1 && next.x > r.newX1 - width && next.x < r.newX2) {
                    cout << "In block y1\n";
                    yMomentum = yMomentum * -1 * elasticity;
                    next.y = r.newY1 - height;
                    hitY = true;
                }
                if (next.y < r.newY2 && next.y + height > r.newY2 && next.x > r.newX1 - width && next.x < r.newX2) {
                    cout << "In block y2\n";
                    yMomentum = yMomentum * -1 * elasticity + max(0.0f, min(r.newY2 - r.oldY2, r.newY2 - next.y));
                    next.y = r.newY2;
                    hitY = true;
                }
            }
        }

        return next;
    }

    bool collider::isSolid(float checkX, float checkY) {
        if (isTileSolid(checkX, checkY)) {
            return true;
        }
        for (movingRectangle rect : rectangles) {
            if (checkX > rect.newX1 && checkX < rect.newX2 && checkY > rect.newY1 && checkY < rect.newY2) {
                return true;
            }
        }
        return false;
    }

    bool collider::isTileSolid(int checkX, int checkY) {
        if (checkY >= 0 && checkY < screen.size() && checkX >= 0 && checkX < screen[checkY].size()) {
            return screen[checkY][checkX] == 's' || screen[checkY][checkX] == '8';
        }
        return false;
    }

    bool collider::isLadder(int checkX, int checkY) {
        if (checkY >= 0 && checkY < screen.size() && checkX >= 0 && checkX < screen[checkY].size()) {
            return screen[checkY][checkX] == 'l' || screen[checkY][checkX] == 'L' || screen[checkY][checkX] == 'H';
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
                return -999;
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
                return -999;
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
        if (freq == -2) {
            return true;
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
