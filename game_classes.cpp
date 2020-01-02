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
        myDrawText(displayFont, "D", (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
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
        myDrawText(displayFont, "S", (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
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
        myDrawText(displayFont, "F", (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
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
            myDrawText(displayFont, toPrint[i].c_str(), (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y + i) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 0, tint);
        }
        myParticles.print(cameraX, cameraY, displayFont);
    }

/******************************************************************************/
//physicalParticle
//A particle to which physics applies.
/******************************************************************************/

    physicalParticle::physicalParticle(   float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                        float newSizeFactor, float newXSpeed, float newYSpeed, int c, int newLifetime,
                        float newElasticity, float newMaxSpeed, float newGravity, float newFriction) :
                        entity(newX, newY, R, G, B, A, newSizeFactor),
                        particle(newX, newY, R, G, B, A, newSizeFactor, 0, 0, c, newLifetime),
                        lightPhysicalEntity(newX, newY, R, G, B, A, newSizeFactor, newElasticity, newXSpeed, newYSpeed, newMaxSpeed, newGravity, newFriction),
                        dynamicChar(c == 0) {}

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
            if (abs(xMomentum) + abs(yMomentum) > 0.2) {
                xSpeed = xMomentum;
                ySpeed = yMomentum;
                particle::setDirection();
            }
            else {
                toPrint = '.';
            }
        }
        myDrawText(displayFont, TextToUtf8(&toPrint, 1), (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
    }

/******************************************************************************/
//Rain
//Constantly spawns particles above the top of the screen
/******************************************************************************/

    rain::rain(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, float newDropsPerTick, float newXMomentum, bool newIsSnow) :
        entity(newX, newY, R, G, B, A, newSizeFactor),
        dropsPerTick(newDropsPerTick),
        xMomentum(newXMomentum),
        isSnow(newIsSnow),
        firstTick(true) {}

    void rain::tickSet(collider& col) {
        //Do rain for a bit on first tick so that raindrops appear to already have been falling when the room is loaded
        //Note that since collider can't be run this way, interactions with other entities won't work properly
        //in these preloaded raindrops. Probably not an issue

        if(firstTick) {
            firstTick = false;
            if (isSnow) {
                for (int i = 0; i < 500; i++) {
                    tickSet(col);
                    tickGet(col);
                }
            }
            else {
                for (int i = 0; i < 50; i++) {
                    tickSet(col);
                    tickGet(col);
                }
            }
        }

        dropBuffer += dropsPerTick;
        while (dropBuffer > 1) {
            dropBuffer--;
            physicalParticle* raindrop;
            if (isSnow) {
                int snowflake;
                switch(GetRandomValue(1, 5)) {
                    case 1:
                        snowflake = '.';
                        break;
                    case 2:
                        snowflake = '*';
                        break;
                    case 3:
                        snowflake = 0x00a4;
                        break;
                    case 4:
                        snowflake = 0x02da;
                        break;
                    case 5:
                        snowflake = 0x263c;
                        break;
                }

/*  physicalParticle constructor:
                        float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                        float newSizeFactor, float newXSpeed, float newYSpeed, int c, int newLifetime,
                        float newElasticity, float newMaxSpeed, float newGravity, float newFriction) :*/

                raindrop = new physicalParticle(GetRandomValue(0, col.getCols() * 10) / 10, GetRandomValue(0, 10) / 10, tint.r, tint.g, tint.b, tint.a, sizeFactor, xMomentum, 0.2, snowflake, 1000, 0, 0.2, GRAVITY, 0);
            }
            else {
                raindrop = new physicalParticle(GetRandomValue(0, col.getCols() * 10) / 10, GetRandomValue(0, 10) / 10, tint.r, tint.g, tint.b, tint.a, sizeFactor, xMomentum, 1, 0, 200, 0, 1, GRAVITY, 0.88);
            }
            raindrops.addEntity(raindrop);
            col.addParticle(raindrop);
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
            col.addParticle(p);
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
        shouldChangeRooms = false;
        nextRoom = newNextRoom;
        yMomentum = 0;
        elasticity = 0;
        type = 1;
        health = maxHealth = 8;
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

        if (IsKeyDown(KEY_D)) {
            if (xMomentum < 0) {
                xMomentum /= 2;
                xMomentum += PLAYERSPEED;
            }
            else if (xMomentum < PLAYERMAXSPEED) {
                xMomentum = min(xMomentum + PLAYERSPEED, PLAYERMAXSPEED);
            }
            lastMovedX = 1;
        }
        if (IsKeyDown(KEY_A)) {
            if (xMomentum > 0) {
                xMomentum /= 2;
                xMomentum -= PLAYERSPEED;
            }
            else if (xMomentum > 0 - PLAYERMAXSPEED) {
                xMomentum = max(xMomentum - PLAYERSPEED, 0 - PLAYERMAXSPEED);
            }
            lastMovedX = -1;
        }
        else if (IsKeyUp(KEY_A) && IsKeyUp(KEY_D)) {
            xMomentum *= 0.75;
        }

        if (IsKeyDown(KEY_W)) {
            lastMovedY += -1;
        }
        if (IsKeyDown(KEY_S)) {
            lastMovedY += 1;
        }

        //Boollets

        //Switching guns

        if (IsKeyPressed(KEY_Q)) {
            gunSelect--;
        }
        if (IsKeyPressed(KEY_E)) {
            gunSelect++;
        }

        //Cooldown

        for (int i = 0; i < gunCoolDowns.size(); i++) {
            gunCoolDowns[i]--;
        }

        //Shootin'

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && unlockedGunIDs.size() > 0) {
            gunSelect = gunSelect % unlockedGunIDs.size();
            if (gunAmmos[gunSelect] > 0 && gunCoolDowns[gunSelect] < 0) {
                Vector2 aim = Vector2Subtract(GetMousePosition(), positionOnScreen);
                bullet* b;
                switch(unlockedGunIDs[gunSelect]) {
                    case 0:
                        aim = Vector2Scale(Vector2Normalize(aim), 0.5);
                        b = new bullet(x, y, tint.r, tint.g, tint.b, tint.a, sizeFactor, aim.x, aim.y, 0, 600, 0, 10, GRAVITY, 0, 3);
                        gunCoolDowns[gunSelect] = 60;
                        gunAmmos[gunSelect]--;
                        break;
                    default:
                        cerr << "Fired bullet with invalid gun\n";
                        break;
                }
                b -> tickSet(col);
                b -> tickSet(col);
                localEntities.addEntity(b);
                col.addCollideable(b);
            }
        }
        //Jomping

        if (IsKeyPressed(KEY_W) && (col.isSolid(y + 1, x + (1 - width) / 2) || col.isSolid(y + 1, x + (1 + width) / 2)))  {
            yMomentum -= JUMPSPEED;
        }
        if (IsKeyReleased(KEY_W)) {
            yMomentum = max(0.0f, yMomentum);
        }

        //Death

        if (y > col.getRows() + 25) {
            health = 0;
        }
        if (health <= 0) {
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
                case 7:         //gun pickup
                    unlockedGunIDs.push_back(colIter -> damage);    //Actually the gunID
                    gunCoolDowns.push_back(0);
                    switch(colIter -> damage) {
                        case 0:
                            gunAmmos.push_back(3);
                            gunMaxAmmos.push_back(3);
                            gunDisplayChars.push_back('1');
                            break;
                    }
                case 8:         //Ammo pickup
                    for (int i = 0; i < unlockedGunIDs.size(); i++) {
                        if (unlockedGunIDs[i] == colIter -> damage) {       //damage is actually gunID again
                            gunAmmos[i] = min(gunMaxAmmos[i], gunAmmos[i] + (int) (colIter -> xVal));
                            break;
                        }
                    }
                    break;
                case 9:         //Health pickup
                    health = min(maxHealth, health + colIter -> damage);
                    break;
                case 10:
                    health += (colIter -> damage);
                    maxHealth += (colIter -> damage);
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
        positionOnScreen = (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor };   //Used for mouse aiming
        myDrawText(displayFont, "@", positionOnScreen, FONTSIZE * sizeFactor, 1, tint);
        localEntities.print(cameraX, cameraY, displayFont);

        //Print HUD

        for (int i = 0; i < unlockedGunIDs.size(); i++) {
            if (gunSelect == i) {
                myDrawText(displayFont, TextToUtf8(&gunDisplayChars[i], 1), { FONTSIZE, (i + 2) * FONTSIZE}, FONTSIZE, 0, GREEN);
            }
            else {
                myDrawText(displayFont, TextToUtf8(&gunDisplayChars[i], 1), { FONTSIZE, (i + 2) * FONTSIZE}, FONTSIZE, 0, WHITE);
            }
            if (gunCoolDowns[i] > 0) {
                myDrawText(displayFont, "X", {FONTSIZE, (i + 2) * FONTSIZE}, FONTSIZE, 0, RED);
            }
            myDrawText(displayFont, to_string(gunAmmos[i]).c_str(), { 2 * FONTSIZE, (i + 2) * FONTSIZE}, FONTSIZE, 0, WHITE);
        }

        //Health background bar

        int fullBlock = 0x2588;
        int partBlock = ' ';
        if (maxHealth % 8 != 0) {
            partBlock = 0x2590 - maxHealth % 8;
        }
        for (int i = 0; i < maxHealth / 8; i++) {
            myDrawText(displayFont, TextToUtf8(&fullBlock, 1), { (i + 1) * FONTSIZE, FONTSIZE }, FONTSIZE, 0, { 15, 15, 30, 255});
        }
        myDrawText(displayFont, TextToUtf8(&partBlock, 1), { (maxHealth + 1) * FONTSIZE, FONTSIZE }, FONTSIZE, 0, {15, 15, 30, 255});

        //Health bar

        partBlock = ' ';
        if (health % 8 != 0) {
            partBlock = 0x2590 - health % 8;
        }
        for (int i = 0; i < health / 8; i++) {
            myDrawText(displayFont, TextToUtf8(&fullBlock, 1), { (i + 1) * FONTSIZE, FONTSIZE }, FONTSIZE, 0, { 15, 15, 30, 255});
        }
        myDrawText(displayFont, TextToUtf8(&partBlock, 1), { (health + 1) * FONTSIZE, FONTSIZE }, FONTSIZE, 0, {15, 15, 30, 255});


    }

/*****************************************************************************/    
//Bullet
//Also self-explanatory
/*****************************************************************************/

    bullet::bullet(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor,
                   float newXMomentum, float newYMomentum, int c, int newLifeTime, float newElasticity,
                   float newMaxSpeed, float newGravity, float newFriction, int newDamage) :
        physicalParticle(newX, newY, R, G, B, A, newSizeFactor, newXMomentum, newYMomentum, c, newLifeTime, newElasticity,
                         newMaxSpeed, newGravity, newFriction),
        entity(newX, newY, R, G, B, A, newSizeFactor),
        damage(newDamage)
    {
        type = 6;
    }

    bool bullet::doesCollide(float otherX, float otherY, int type) {
        if (!hit && otherX > x - width && otherX < x + width && otherY > y - width && otherY < y + width) {
            return true;
        }
        return false;
    }

    collision bullet::getCollision() {
        return collision(6, damage, xMomentum, yMomentum);
    }

    bool bullet::stopColliding() {
        return hit || physicalParticle::stopColliding();
    }

    void bullet::tickSet(collider& col) {
        physicalParticle::tickSet(col);

        if (xMomentum == 0) {   //If hit a wall during physicalParticle::tickSet()
            hit = true;
        }

        if (hit & !exploded) {
            exploded = true;
            explosion(col, collisionParticles, 6, x, y, tint.r, tint.g, tint.b, tint.a / 2, sizeFactor, 0.25, 0, 60, 0.3);
        }

        collisionParticles.tickSet(col);
    }

    void bullet::tickGet(collider& col) {
        physicalParticle::tickGet(col);

        for (collision c : collisions) {
            if (c.type == 5) {   //forcefield
                xMomentum += c.xVal;
                yMomentum += c.yVal;
            }
            else {
                hit = true;
            }
        }

        collisionParticles.tickGet(col);
    }

    bool bullet::finalize() {
        collisionParticles.finalize();  //Need to finalize collision particles so they have a chance to return true to stopColliding before they/this object are deleted
        return ((physicalParticle::finalize() || hit) && collisionParticles.size() == 0);
    }

    void bullet::print(float cameraX, float cameraY, Font displayFont) {
         if (!hit) {
            if (dynamicChar) {
                xSpeed = xMomentum;
                ySpeed = yMomentum;
                particle::setDirection();
            }
            particle::print(cameraX, cameraY, displayFont);
        }
        else {
            collisionParticles.print(cameraX, cameraY, displayFont);
        }
    }

/*****************************************************************************/
//gunPickUp
//Unlocks a new gun
/*****************************************************************************/

    gunPickUp::gunPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifetime, int newGunID) :
                        pickUp(newX, newY, R, G, B, A, newSizeFactor, newDisplayChar, newLifetime),
                        entity(newX, newY, R, G, B, A, newSizeFactor),
                        gunID(newGunID) {}

    collision gunPickUp::getCollision() {
        return collision(7, gunID);
    }

/*****************************************************************************/
//ammoPickUp
//Adds ammo to a given gun
/*****************************************************************************/

    ammoPickUp::ammoPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifetime, int newGunID, int newAmmoCount) :
                        pickUp(newX, newY, R, G, B, A, newSizeFactor, newDisplayChar, newLifetime),
                        entity(newX, newY, R, G, B, A, newSizeFactor),
                        gunID(newGunID),
                        ammoCount(newAmmoCount) {}

    collision ammoPickUp::getCollision() {
        return collision(8, gunID, ammoCount);      //ammoCount in xVal
    }

/*****************************************************************************/
//healthPickUp
//Adds health back
/*****************************************************************************/

    healthPickUp::healthPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifetime, int newHealthCount) :
                        pickUp(newX, newY, R, G, B, A, newSizeFactor, newDisplayChar, newLifetime),
                        entity(newX, newY, R, G, B, A, newSizeFactor),
                        healthCount(newHealthCount) {}

    collision healthPickUp::getCollision() {
        return collision(9, healthCount);
    }

/*****************************************************************************/
//maxHealthPickUp
//Adds to max health
/*****************************************************************************/

    maxHealthPickUp::maxHealthPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifetime, int newHealthCount) :
                        pickUp(newX, newY, R, G, B, A, newSizeFactor, newDisplayChar, newLifetime),
                        entity(newX, newY, R, G, B, A, newSizeFactor),
                        healthCount(newHealthCount) {}

    collision maxHealthPickUp::getCollision() {
        return collision(10, healthCount);
    }
