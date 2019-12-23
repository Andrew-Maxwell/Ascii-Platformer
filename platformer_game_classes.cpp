#include "platformer.h"

using namespace std;

/******************************************************************************/
/*Collidor*/
/******************************************************************************/

    //Accessors

    collider::collider(float newX, float newY, string fileName) :
        layer(newX, newY, 0, 0, 0, 0, 1, fileName),
        entity(newX, newY, 0, 0, 0, 0, 1) {}

    void collider::addCollideable(collideable* newCollideable) {
        collideables.push_back(newCollideable);
    }

    bool collider::isSolid(int row, int col) {
        if (row >= 0 && row < canvas.size() && col >= 0 && col < canvas[row].size()) {
            return canvas[row][col] == 's' || canvas[row][col] == '8';
        }
        return false;
    }

    bool collider::isLiquid(int row, int col) {
        return canvas[row][col] == 'w';
    }

    //Stairs/ramps currently on hold.

    //Helper function for getFloorLevel. Used to check multiple points along the bottom of the entity.

    float collider::checkHeight(float checkX, float checkY) {
        if (checkY >= 0 && checkY < canvas.size() && checkX >= 0 && checkX < canvas[checkY].size()) {
            if (48 <= canvas[checkY][checkX] && canvas[checkY][checkX] < 56) {
                return (canvas[checkY][checkX] - '0') / 8.0;
            }
            if (canvas[checkY][checkX] == 'R' || canvas[checkY][checkX] == 'r') {
                return fmod(checkX, 1.0);
            }
            if (canvas[checkY][checkX] == 'L' || canvas[checkY][checkX] == 'l') {
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

    //tickSet, tickGet, and finalize are each called here AFTER they have been called on all other entities.

    void collider::tickSet(collider& col) {

        //Check for collisions between any two pairs of collideables.
        //Note that collisions can be asymmetric: A can collide with B but not B with A.
        //May change this depending on usage later.

        for (int i = 0; i < collideables.size(); i++) {
            for (int j = i + 1; j < collideables.size(); j++) {
                if (collideables[i] -> doesCollide(collideables[j] -> x, collideables[j] -> y, collideables[j] -> type)) {
                    collideables[j] -> addCollision(collideables[i] -> getCollision());
                }
                if (collideables[j] -> doesCollide(collideables[i] -> x, collideables[i] -> y, collideables[i] -> type)) {
                    collideables[i] -> addCollision(collideables[j] -> getCollision());
                }
            }
        }
    }

    void collider::tickGet(collider& col) {
        //Iterate through and remove collideables for which stopColliding is true
        //called here because entities may be deleted in the finalize() step

        vector<collideable*>::iterator c = collideables.begin();
        while (c != collideables.end()) {
            if ((*c) -> stopColliding()) {
                c = collideables.erase(c);
            }
            else {
                c++;
            }
        }
    }

    bool collider::finalize() {}

/******************************************************************************/
//Particle
//Entity represented by any char that moves in a predefined direction until its lifetime runs out.
//If the character passed to constructor is 0, then a character is chosen based on direction.
/******************************************************************************/

    void particle::setDirection() {
        if (xSpeed == 0) {
            toPrint[0] = '|';
        }
        else {
            if (ySpeed / xSpeed < -1.09) {
                toPrint[0] = '|';
            }
            else if (ySpeed / xSpeed < -0.383) {
                toPrint[0] = '/';
            }
            else if (ySpeed / xSpeed < 0.383) {
                toPrint[0] = '-';
            }
            else if (ySpeed / xSpeed < 1.09) {
                toPrint[0] = '\\';
            }
            else {
                toPrint[0] = '|';
            }
        }
    }

    particle::particle(  float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                        float newSizeFactor, float newXSpeed, float newYSpeed, char c, int newLifetime) :
                        entity(newX, newY, R, G, B, A, newSizeFactor) {
        xSpeed = newXSpeed;
        ySpeed = newYSpeed;
        lifetime = newLifetime;
        if (c == 0) {
            setDirection();
        }
        else {
            toPrint[0] = c;
        }
        toPrint[1] = '\0';
    }

    void particle::tickSet(collider& col) {
        x += xSpeed;
        y += ySpeed;
    }

    void particle::tickGet(collider& col) {}

    bool particle::finalize() {
        if (lifetime-- == 0) {
            return true;
        }
        else {
            return false;
        }
    }

    void particle::print(float cameraX, float cameraY, Font displayFont) {
        DrawTextEx(displayFont, toPrint, (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
    }

/*****************************************************************************/
//Door
//Triggers a transition to another room when interacted with.
/*****************************************************************************/

    door::door(  float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                    float newSizeFactor, string newDestinationRoom, float newDestX, float newDestY) :
        entity(newX, newY, R, G, B, A, newSizeFactor),
        destinationRoom(newDestinationRoom),
        destinationX(newDestX),
        destinationY(newDestY) {
        type = 3;
    }

    bool door::doesCollide(float otherX, float otherY, int otherType) {
        return (IsKeyPressed(KEY_DOWN) && otherX > x - 1 && otherX < x + 1 && otherY > y - 1 && otherY < y + 1 && otherType == 1);
    }

    collision door::getCollision() {
        return collision(type, 0, destinationX, destinationY, destinationRoom);
    }

    bool door::stopColliding() {
        return false;
    }

    void door::tickSet(collider& col) {}

    void door::tickGet(collider& col) {}

    bool door::finalize() {return false;}

    void door::print(float cameraX, float cameraY, Font displayFont) {
        DrawTextEx(displayFont, "D", (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
    }


/*****************************************************************************/
//SavePoint
//Saves the game when interacted with.
/*****************************************************************************/

    savePoint::savePoint(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor) : entity(newX, newY, R, G, B, A, newSizeFactor) {type = 4;}

    bool savePoint::doesCollide(float otherX, float otherY, int otherType) {
        bool collided = (IsKeyPressed(KEY_DOWN) && otherX > x - 1 && otherX < x + 1 && otherY > y - 1 && otherY < y + 1 && otherType == 1);
        savedGame |= collided;
        return collided;
    }

    collision savePoint::getCollision() {
        return collision(4);
    }

    bool savePoint::stopColliding() {
        return false;
    }

    void savePoint::tickSet(collider& col) {
        myParticles.tickSet(col);;
    }

    void savePoint::tickGet(collider& col) {
        if (savedGame) {;
            savedGame = false;
            explosion(myParticles, 16, x, y, tint.r, tint.g, tint.b, tint.a, sizeFactor, 0.3, '*', 100, 0.5);
        }
        myParticles.tickGet(col);
    }

    bool savePoint::finalize() {
        myParticles.finalize();
        return false;
    }

    void savePoint::print(float cameraX, float cameraY, Font displayFont) {
        DrawTextEx(displayFont, "S", (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
        myParticles.print(cameraX, cameraY, displayFont);
    }

/*****************************************************************************/
//forceField
//Attracts or repels physical entities within its influende
/*****************************************************************************/

    forceField::forceField(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, float newPower, float newRange) :
        entity(newX, newY, R, G, B, A, newSizeFactor),
        power(newPower),
        range(newRange),
        isOn(false) {
            nextCollision.type = 5;
            //nextCollision.xVal = 0.2;
            //nextCollision.yVal = 0.2;
        }

    bool forceField::doesCollide(float otherX, float otherY, int otherType) {
        if (!isOn) {
            return false;
        }
        if (pow(pow(x - otherX, 2) + pow(y - otherY, 2), 0.5) > range) {
            return false;
        }
        cout << "Forcefield activated, entity within range.\n";
        nextCollision.xVal = copysign(pow(pow(power, 2) / (1 + pow((y - otherY) / (x - otherX), 2)), 0.5), x - otherX) * copysign(1, power);
        nextCollision.yVal = copysign(pow(pow(power, 2) / (1 + pow((x - otherX) / (y - otherY), 2)), 0.5), y - otherY) * copysign(1, power);
        cout << nextCollision.xVal << "\t" <<  nextCollision.yVal << endl;
        return true;
    }

    collision forceField::getCollision() {
        cout << "Collision transmitted.\n";
        return nextCollision;
    }

    bool forceField::stopColliding() {
        return false;
    }

    void forceField::tickSet(collider& col) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            isOn = true;
            if (++particleCount % 200 * power == 0) {
                if (power > 0) { //Attractor force field
                    for (float angle = 0; angle < 2 * M_PI; angle += (2 * M_PI / 40)) {
                        myParticles.addEntity(new particle(x + cos(angle) * range, y + sin(angle) * range, tint.r, tint.g, tint.b, tint.a, sizeFactor, cos(angle) * power * -40, sin(angle) * power * -40, 0, range / power / 40));
                    }
                }
                else {  //repeller force field
                    for (float angle = 0; angle < 2 * M_PI; angle += (2 * M_PI / 40)) {
                        cout << "Spawning particle." << range / power / 40 << "\n";
                        myParticles.addEntity(new particle(x, y, tint.r, tint.g, tint.b, tint.a, sizeFactor, cos(angle) * power * 40, sin(angle) * power * 40, 0, range / abs(power) / 40));
                    }
                }
            }
        }
        else {
            isOn = false;
            particleCount = -1;
        }
        myParticles.tickSet(col);
    }

    void forceField::tickGet(collider& col) {
        myParticles.tickGet(col);
    }

    bool forceField::finalize() {
        myParticles.finalize();
        return false;
    }

    void forceField::print(float cameraX, float cameraY, Font displayFont) {
        DrawTextEx(displayFont, "F", (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
        myParticles.print(cameraX, cameraY, displayFont);
    }


/*****************************************************************************/
//endingGate
//an imposing gate that spawns particles that float slowly upwards.
/*****************************************************************************/

    endingGate::endingGate(float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                        float newSizeFactor, int newWidth = 0, int newHeight = 0) :
                        entity(newX, newY, R, G, B, A, newSizeFactor) {
        toPrint = {"O----O", "|    |", "|    |", "|    |", "O----O"};
        width = 5;
        height = 5;
        type = 2;
    }

    bool endingGate::doesCollide(float otherX, float otherY, int otherType) {
        return (otherX > x && otherX < x + width && otherY > y && otherY < y + height && otherType == 1);
    }

    collision endingGate::getCollision() {
        return collision(type, 0, 0, 0, "");
    }

    bool endingGate::stopColliding() {
        return false;
    }

    void endingGate::tickSet(collider& col) {
        if (GetRandomValue(0, 10) == 0) {
            myParticles.addEntity(new particle( x + GetRandomValue(0, 10 * width) / 10.0, y + height - 1.5, tint.r, tint.g,
                                                tint.b, tint.a, sizeFactor, 0, -0.01, '*', (height - 1.5) * 100));
        }
        myParticles.tickSet(col);
    }

    void endingGate::tickGet(collider& col) {
        myParticles.tickGet(col);
    }

    bool endingGate::finalize() {
        myParticles.finalize();
        return false;
    }

    void endingGate::print(float cameraX, float cameraY, Font displayFont) {
        for (int i = 0; i < height; i++) {
            DrawTextEx(displayFont, toPrint[i].c_str(), (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y + i) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 0, tint);
        }
        myParticles.print(cameraX, cameraY, displayFont);
    }

/******************************************************************************/
//lightPhysicalEntity
//An entity to which physics (gravity and not travelling through solid objects)
//applies -- loosely.
/******************************************************************************/

    lightPhysicalEntity::lightPhysicalEntity( float newx, float newy, uint8_t R, uint8_t G, uint8_t B,
                                  uint8_t A, float newSizeFactor, float elasticity, float newXMomentum,
                                  float newYMomentum) :
                                  entity(newx, newy, R, G, B, A, newSizeFactor) {
        xMomentum = newXMomentum;
        yMomentum = newYMomentum;
    }

    void  lightPhysicalEntity::tickSet(collider& col) {
        yMomentum += GRAVITY;

        if (col.isSolid((int)y, (int)(x + xMomentum) + (xMomentum > 0))) {
            x = floor(x + xMomentum) + (xMomentum < 0);
            xMomentum *= (-1 * elasticity);
        }
        else {
            x += xMomentum;
        }

        if (col.isSolid((int)(y + yMomentum) + (yMomentum > 0), (int)x)) {
            y = floor(y + yMomentum) + (yMomentum < 0);
            yMomentum *= (-1 * elasticity);
            xMomentum *= FRICTION;
        }
        else {
            y += yMomentum;
        }
    }

    void lightPhysicalEntity::tickGet(collider& col) {}

    bool lightPhysicalEntity::finalize() {return false;}

    void lightPhysicalEntity::print() {}

/******************************************************************************/
//realPhysicalEntity
//An entity to which physics (gravity and not travelling through solid objects)
//applies, more rigorously.
/******************************************************************************/

    realPhysicalEntity::realPhysicalEntity(float newx, float newy,  uint8_t R, uint8_t G, uint8_t B,
                                uint8_t A, float newSizeFactor, float elasticity, float newXMomentum,
                                float newYMomentum) :
                                entity(newx, newy, R, G, B, A, newSizeFactor) {
        xMomentum = newXMomentum;
        yMomentum = newYMomentum;
    }

    void realPhysicalEntity::tickSet(collider& col) {
        yMomentum += GRAVITY;

        float xDist = xMomentum / (abs(xMomentum) + 1);
        for (int i = 0; i < abs(xMomentum) + 1; i++) {
            if (col.isSolid((int)y, (int)(x + xDist) + (xDist > 0))) {// || col.isSolid((int)y + 0.5, (int)(x + xDist) + (xDist > 0))) {
                x = floor(x + xDist) + (xDist < 0);
                xMomentum *= (-1 * elasticity);
                break;
            }
            else {
                x += xDist;
            }
        }

        float yDist = yMomentum / (abs(yMomentum) + 1);
        for (int i = 0; i < abs(yMomentum) + 1; i++) {
            if (col.isSolid((int)(y + yDist) + (yDist > 0), (int)(x + 0.5 - width / 2)) || col.isSolid((int)(y + yDist) + (yDist > 0), (int)(x + 0.5 + width / 2))) {
                y = floor(y + yDist) + (yDist < 0);
                yMomentum *= (-1 * elasticity);
                xMomentum *= FRICTION;
                break;
            }
            else {
                y += yDist;
            }
        }
    }

    void realPhysicalEntity::tickGet(collider& col) {}

    bool realPhysicalEntity::finalize() {return false;}

    void realPhysicalEntity::print() {}

/******************************************************************************/
//physicalParticle
//A particle to which physics applies.
/******************************************************************************/

    physicalParticle::physicalParticle(   float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                        float newSizeFactor, float newXSpeed, float newYSpeed, char c, int newLifetime,
                        float newElasticity) :
                        entity(newX, newY, R, G, B, A, newSizeFactor),
                        particle(newX, newY, R, G, B, A, newSizeFactor, 0, 0, c, newLifetime),
                        lightPhysicalEntity(newX, newY, R, G, B, A, newSizeFactor, newElasticity, newXSpeed, newYSpeed) {

        dynamicChar = (c == 0);
        elasticity = newElasticity;
        lifetime = newLifetime;
    }

    void physicalParticle::tickSet(collider& col) {
        lightPhysicalEntity::tickSet(col);
        if (col.isSolid(y, x)) {
            shouldDelete = true;
        }
    }

    void physicalParticle::tickGet(collider& col) {
        lightPhysicalEntity::tickGet(col);
    }

    bool physicalParticle::finalize() {
        return (xMomentum < 0.01 && xMomentum > -0.01 && yMomentum < 0.01 && yMomentum > -0.01) || shouldDelete || particle::finalize();
    }

    void physicalParticle::print(float cameraX, float cameraY, Font displayFont) {
        if (dynamicChar) {
            if (abs(xMomentum) + abs(yMomentum) > 0.1) {
                xSpeed = xMomentum;
                ySpeed = yMomentum;
                particle::setDirection();
            }
            else {
                toPrint[0] = '.';
            }
        }
        DrawTextEx(displayFont, toPrint, (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
    }

/******************************************************************************/
//Explosion()
//Spawn particles moving outwards in a circle
/******************************************************************************/

    void explosion( entityList& entities, int count, float x, float y, uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                    float newSizeFactor, float speed, char c, int lifespan, float elasticity) {
        for (float angle = 0; angle < 2 * M_PI; angle += (2 * M_PI / count)) {
            entities.addEntity(new physicalParticle(x, y, R, G, B, A, newSizeFactor, cos(angle) * speed, sin(angle) * speed, c, lifespan, elasticity));
        }
    }

/******************************************************************************/
//PlayerEntity
//what it sounds like, I guess.
/******************************************************************************/

//Constructor

    playerEntity::playerEntity(  float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, string newNextRoom) :
                            realPhysicalEntity(newX, newY, R, G, B, A, newSizeFactor, 1.0, 0.0, 0.0),
                            entity(newX, newY, R, G, B, A, newSizeFactor)
    {
        nextRoom = newNextRoom;
        yMomentum = 0;
        elasticity = 0;
        type = 1;
    }

//Special accessors used when loading a room
/*
    void playerEntity::loadSave(string savedNextRoom, float savedX, float savedY) {
        x = savedX;
        y = savedY;
        nextRoom = savedNextRoom;
    }
*/
    void playerEntity::setColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A) {
        tint = {R, G, B, A};
    }

    void playerEntity::setSizeFactor(float newSizeFactor) {
        sizeFactor = newSizeFactor;
    }

    float playerEntity::getSizeFactor() {
        return sizeFactor;
    }

//Collision functions

    bool playerEntity::doesCollide(float otherX, float otherY, int type) {
        return (otherX >= x && otherX <= x + 1 && otherY >= y && otherY <= y + 1);
    }

    collision playerEntity::getCollision() {
        collision c;
        return c;
    }

    bool playerEntity::stopColliding() {
        return false;
    }

//Tick functions

    void playerEntity::tickSet(collider& col) {
        lastMovedY = 0;
//        xMomentum *= 0.5;
    /*
        if (xMomentum > 2) {
            xMomentum = 2;
        }
        if (yMomentum > 2) {
            yMomentum = 2;
        }
    */

        //Reload room

        if (IsKeyPressed(KEY_R)) {
            shouldChangeRooms = true;
        }

        //Movement

        if (IsKeyDown(KEY_RIGHT)) {
            if (xMomentum < 0) {
                xMomentum /= 2;
                xMomentum += PLAYERSPEED;
            }
            else if (xMomentum < PLAYERMAXSPEED) {
                xMomentum = min(xMomentum + PLAYERSPEED, PLAYERMAXSPEED);
            }
            lastMovedX = 1;
        }
        if (IsKeyDown(KEY_LEFT)) {
            if (xMomentum > 0) {
                xMomentum /= 2;
                xMomentum -= PLAYERSPEED;
            }
            else if (xMomentum > 0 - PLAYERMAXSPEED) {
                xMomentum = max(xMomentum - PLAYERSPEED, 0 - PLAYERMAXSPEED);
            }
            lastMovedX = -1;
        }
        else if (IsKeyUp(KEY_LEFT) && IsKeyUp(KEY_RIGHT)) {
            xMomentum *= 0.75;
        }

        if (IsKeyDown(KEY_UP)) {
            lastMovedY += -1;
        }
        if (IsKeyDown(KEY_DOWN)) {
            lastMovedY += 1;
        }

        //Boollets

        if (IsKeyPressed(KEY_X)) {
            particle* p;
            if (lastMovedY) {
                p = new particle(x, y, tint.r, tint.g, tint.b, tint.a, sizeFactor, 0, lastMovedY * bulletSpeed, 0, 20);
            }
            else {
                p = new particle(x, y, tint.r, tint.g, tint.b, tint.a, sizeFactor, lastMovedX * bulletSpeed, 0, 0, 20);
            }
            localEntities.addEntity(p);
        }

        //Splosions

        if (IsKeyPressed(KEY_C)) {
            explosion(localEntities, 50, x, y, tint.r, tint.g, tint.b, tint.a, sizeFactor, 1, 0, PARTICLELIFESPAN, 0.2);
        }

        //Jomping

        if (IsKeyPressed(KEY_Z) && (col.isSolid(y + 1, x + (1 - width) / 2) || col.isSolid(y + 1, x + (1 + width) / 2)))  {
            yMomentum -= JUMPSPEED;
        }
        if (IsKeyReleased(KEY_Z)) {
            yMomentum = max(0.0f, yMomentum);
        }

        //Death

        if (y > col.getRows() + 25) {
            won = -1;
        }

        //Apply physics

        realPhysicalEntity::tickSet(col);

        //update all child entities

        localEntities.tickSet(col);
    }

    void playerEntity::tickGet(collider& col) {
        localEntities.tickGet(col);
        list<collision>::iterator colIter = collisions.begin();
        while (colIter != collisions.end()) {
            switch(colIter -> type) {
                case 2:         //Win portal
                    won = 1;
                    break;
                case 3:         //Door
                    x = colIter -> xVal;
                    y = colIter -> yVal;
                    nextRoom = colIter -> message;
                    shouldChangeRooms = true;
                    localEntities.clear();
                    break;
                case 4: {       //Savepoint
                    ofstream saveOut;
                    saveOut.open("save.txt");
                    saveOut << nextRoom << " " << x << " " << y;
                    saveOut.close();
                    break;
                }
                case 5:         //Forcefield
                    cout << "Received forcefield collision.\n";
                    xMomentum += colIter -> xVal;
                    yMomentum += colIter -> yVal;
                    break;
            }
            colIter = collisions.erase(colIter);
        }
    }

    bool playerEntity::finalize() {
        localEntities.finalize();
        return false;
    }

    void playerEntity::print(float cameraX, float cameraY, Font displayFont) {
        DrawTextEx(displayFont, "@", (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
        localEntities.print(cameraX, cameraY, displayFont);
    }

