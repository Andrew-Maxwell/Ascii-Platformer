#include "game_meta_classes.hpp"
#include "game_classes.hpp"

using namespace std;

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
            explosion(col, myParticles, 16, x, y, tint.r, tint.g, tint.b, tint.a, sizeFactor, 0.3, '*', 100, 0.5);
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
        }

    bool forceField::doesCollide(float otherX, float otherY, int otherType) {
        if (!isOn) {
            return false;
        }
        if (pow(pow(x - otherX, 2) + pow(y - otherY, 2), 0.5) > range) {
            return false;
        }
        nextCollision.xVal = copysign(pow(pow(power, 2) / (1 + pow((y - otherY) / (x - otherX), 2)), 0.5), x - otherX) * copysign(1, power);
        nextCollision.yVal = copysign(pow(pow(power, 2) / (1 + pow((x - otherX) / (y - otherY), 2)), 0.5), y - otherY) * copysign(1, power);
        return true;
    }

    collision forceField::getCollision() {
        return nextCollision;
    }

    bool forceField::stopColliding() {
        return false;
    }

    void forceField::tickSet(collider& col) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            isOn = true;
            if (++tickCount % (int)(1 / power) == 0) {
                if (power > 0) { //Attractor force field
                    for (float angle = 0; angle < 2 * M_PI; angle += (2 * M_PI / 20)) {
                        myParticles.addEntity(new particle(x + cos(angle) * range, y + sin(angle) * range, tint.r, tint.g, tint.b, tint.a, sizeFactor, cos(angle) * power * -40, sin(angle) * power * -40, 0, range / power / 40));
                    }
                }
                else {  //repeller force field
                    for (float angle = 0; angle < 2 * M_PI; angle += (2 * M_PI / 20)) {
                        myParticles.addEntity(new particle(x, y, tint.r, tint.g, tint.b, tint.a, sizeFactor, cos(angle) * power * 40, sin(angle) * power * 40, 0, range / abs(power) / 40));
                    }
                }
            }
        }
        else {
            isOn = false;
            tickCount = -1;
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

    bool physicalParticle::doesCollide(float otherX, float otherY, int otherType) {
        return false;
    }

    collision physicalParticle::getCollision() {
        return collision(0);
    }

    bool physicalParticle::stopColliding() {
        return finalize();
    }

    void physicalParticle::tickSet(collider& col) {
        lifetime--;
        lightPhysicalEntity::tickSet(col);
        if (col.isSolid(y, x)) {
            shouldDelete = true;
        }
    }

    void physicalParticle::tickGet(collider& col) {
        for (collision thisCollision : collisions) {
            xMomentum += thisCollision.xVal;
            yMomentum += thisCollision.yVal;
        }
        lightPhysicalEntity::tickGet(col);
        collisions.clear();
    }

    bool physicalParticle::finalize() {
        return ((xMomentum < 0.01 && xMomentum > -0.01 && yMomentum < 0.01 && yMomentum > -0.01) || shouldDelete || particle::finalize());
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
//Rain
//Constantly spawns particles above the top of the screen
/******************************************************************************/

    rain::rain(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, float newDropsPerTick) :
        entity(newX, newY, R, G, B, A, newSizeFactor),
        dropsPerTick(newDropsPerTick),
        firstTick(true) {}

    void rain::tickSet(collider& col) {
    {
        //Do rain for a bit on first tick so that raindrops appear to already have been falling when the room is loaded
        //Note that since collider can't be run this way, interactions with other entities won't work properly
        //in these preloaded raindrops. Probably not an issue

        if (firstTick) {
            firstTick = false;
            for (int i = 0; i < 50; i++) {
                tickSet(col);
                tickGet(col);
                finalize();
            }
        }
    }


        dropBuffer += dropsPerTick;
        while (dropBuffer > 1) {
            dropBuffer--;
            physicalParticle* raindrop = new physicalParticle(GetRandomValue(0, col.getCols() * 10) / 10, GetRandomValue(0, 10) / 10, tint.r, tint.g, tint.b, tint.a, sizeFactor, 0, 1, 0, 200, 0);
            col.addCollideable(raindrop);
            raindrops.addEntity(raindrop);
        }
        raindrops.tickSet(col);
    }

    void rain::tickGet(collider& col) {
        raindrops.tickGet(col);
    }

    bool rain::finalize() {
        raindrops.finalize();
        return false;
    }

    void rain::print(float cameraX, float cameraY, Font displayFont) {
        raindrops.print(cameraX, cameraY, displayFont);
    }


/******************************************************************************/
//Explosion()
//Spawn particles moving outwards in a circle
/******************************************************************************/

    void explosion( collider& col, entityList& entities, int count, float x, float y, uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                    float newSizeFactor, float speed, char c, int lifespan, float elasticity) {
        for (float angle = 0; angle < 2 * M_PI; angle += (2 * M_PI / count)) {
            physicalParticle* p = new physicalParticle(x, y, R, G, B, A, newSizeFactor, cos(angle) * speed, sin(angle) * speed, c, lifespan, elasticity);
            entities.addEntity(p);
            col.addCollideable(p);
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

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            float xMomentum = copysign(pow(pow(bulletSpeed, 2) / (1 + pow((mouse.y - y) / (mouse.x - x), 2)), 0.5), mouse.x - x) * copysign(1, bulletSpeed);
            float yMomentum = copysign(pow(pow(bulletSpeed, 2) / (1 + pow((mouse.x - x) / (mouse.y - y), 2)), 0.5), mouse.y - y) * copysign(1, bulletSpeed);
            bullet* b = new bullet(x, y, tint.r, tint.g, tint.b, tint.a, sizeFactor, xMomentum, yMomentum, bulletDamage);
            b -> tickSet(col);
            b -> tickSet(col);
            localEntities.addEntity(b);
            col.addCollideable(b);
        }

        //Splosions

        if (IsKeyPressed(KEY_C)) {
            explosion(col, localEntities, 50, x, y, tint.r, tint.g, tint.b, tint.a, sizeFactor, 1, 0, PARTICLELIFESPAN, 0.2);
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
                    xMomentum += colIter -> xVal;
                    yMomentum += colIter -> yVal;
                    break;
                case 6:         //Bullet
                    yMomentum += colIter -> yVal * 0.3;
                    xMomentum += colIter -> xVal * 0.3;
                    //Health?
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

/*****************************************************************************/    
//Bullet
//Also self-explanatory
/*****************************************************************************/

    bullet::bullet(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, float newXMomentum, float newYMomentum, int newDamage) :
        particle(newX, newY, R, G, B, A, newSizeFactor, 0, 0, 0, 1000),
        entity(newX, newY, R, G, B, A, newSizeFactor),
        xMomentum(newXMomentum),
        yMomentum(newYMomentum),
        damage(newDamage)
    {
        type = 6;
    }

    bool bullet::doesCollide(float otherX, float otherY, int type) {
        if (!hit && otherX > x - width && otherX < x + width && otherY > y - width && otherY < y + width) {
            hit = true;
            return true;
        }
        return false;
    }

    collision bullet::getCollision() {
        return collision(6, damage, xMomentum, yMomentum);
    }

    bool bullet::stopColliding() {
        return hit;
    }

    void bullet::tickSet(collider& col) {
        yMomentum += GRAVITY;

        float xDist = xMomentum / (abs(xMomentum) + 1);
        for (int i = 0; i < abs(xMomentum) + 1; i++) {
            if (col.isSolid((int)y, (int)(x + xDist) + (xDist > 0))) {
                x = floor(x + xDist) + (xDist < 0);
                xMomentum = 0;
                hit = true;
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
                yMomentum = 0;
                xMomentum = 0;
                break;
            }
            else {
                y += yDist;
            }
        }

        if (hit & !exploded) {
            exploded = true;
            explosion(col, collisionParticles, 10, x, y, tint.r, tint.g, tint.b, tint.a, sizeFactor, 0.5, 0, 60, 0.3);
        }

        collisionParticles.tickSet(col);
    }

    void bullet::tickGet(collider& col) {
        collisionParticles.tickGet(col);
    }

    bool bullet::finalize() {
        return --lifetime == 0;
    }

    void bullet::print(float cameraX, float cameraY, Font displayFont) {
        xSpeed = xMomentum;
        ySpeed = yMomentum;
        particle::setDirection();
        particle::print(cameraX, cameraY, displayFont);
    }


