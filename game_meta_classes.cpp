#include "meta.hpp"
#include "game_meta_classes.hpp"
#include "game_classes.hpp"     //For explosion in pickUp

using namespace std;

/******************************************************************************/
/*gameLayer
Implements layer functionality (e.g. animations) which the editor doesn't use.*/
/******************************************************************************/

    gameLayer::gameLayer( float newx, float newy, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, string newFileName) :
        entity(newx, newy, R, G, B, A, newSizeFactor),
        layer(newx, newy, R, G, B, A, newSizeFactor, newFileName) {
 
            if (!layer::readLayer()) {
                cout << "layer file " << fileName << " not found. Checking \\levels...\n";
                fileName = string("levels\\").append(fileName);
                if (!layer::readLayer()) {
                    cerr << "Error: layer file " << fileName << " not found.\n";
                    exit(EXIT_FAILURE);
                }
                cout << "Found!\n";
            }
        }

/******************************************************************************/
/*Collidor*/
/******************************************************************************/

    //Accessors

    collider::collider(float newX, float newY, string fileName) :
        entity(newX, newY, 0, 0, 0, 0, 1),
        layer(newX, newY, 0, 0, 0, 0, 1, fileName)
        {
            if (!layer::readLayer()) {
                cout << "collider file " << fileName << " not found. Checking \\levels...\n";
                fileName = string("levels\\").append(fileName);
                if (!layer::readLayer()) {
                    cerr << "Error: collider file " << fileName << " not found.\n";
                    exit(EXIT_FAILURE);
                }
                cout << "Found!\n";
            }
        }

    void collider::addCollideable(collideable* newCollideable) {
        collideables.push_back(newCollideable);
    }

    void collider::addParticle(collideable* newParticle) {
        particles.push_back(newParticle);
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

        for (unsigned int i = 0; i < collideables.size(); i++) {
            for (unsigned int j = i + 1; j < collideables.size(); j++) {
                if (collideables[i] -> doesCollide(collideables[j] -> x, collideables[j] -> y, collideables[j] -> type)) {
                    collideables[j] -> addCollision(collideables[i] -> getCollision());
                }
                if (collideables[j] -> doesCollide(collideables[i] -> x, collideables[i] -> y, collideables[i] -> type)) {
                    collideables[i] -> addCollision(collideables[j] -> getCollision());
                }
            }
        }

        //Check for collideables affecting particles

        for (unsigned int i = 0; i < collideables.size(); i++) {
            for (unsigned int j = 0; j < particles.size(); j++) {
                if (collideables[i] -> doesCollide(particles[j] -> x, particles[j] -> y, particles[j] -> type)) {
                    particles[j] -> addCollision(collideables[i] -> getCollision());
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

        vector<collideable*>::iterator p = particles.begin();
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
        return false;
    }

/******************************************************************************/
//Particle
//Entity represented by any char that moves in a predefined direction until its lifetime runs out.
//If the character passed to constructor is 0, then a character is chosen based on direction.
/******************************************************************************/

    particle::particle(  float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                        float newSizeFactor, float newXSpeed, float newYSpeed, int c, int newLifetime) :
                        entity(newX, newY, R, G, B, A, newSizeFactor),
                        xSpeed(newXSpeed),
                        ySpeed(newYSpeed),
                        lifetime(newLifetime) {
        if (c == 0) {
            setDirection();
        }
        else {
            toPrint = c;
        }
    }

    void particle::setDirection() {
        if (xSpeed == 0) {
            toPrint = '|';
        }
        else {
            if (ySpeed / xSpeed < -1.09) {
                toPrint = '|';
            }
            else if (ySpeed / xSpeed < -0.383) {
                toPrint = '/';
            }
            else if (ySpeed / xSpeed < 0.383) {
                toPrint = '-';
            }
            else if (ySpeed / xSpeed < 1.09) {
                toPrint = '\\';
            }
            else {
                toPrint = '|';
            }
        }
    }

    void particle::tickSet(collider& col) {
        x += xSpeed;
        y += ySpeed;
        lifetime--;
    }

    void particle::tickGet(collider& col) {}

    bool particle::finalize() {
        if (lifetime == 0) {
            return true;
        }
        else {
            return false;
        }
    }

    void particle::print(float cameraX, float cameraY, Font displayFont) {
        myDrawText(displayFont, TextToUtf8(&toPrint, 1), (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
    }

/******************************************************************************/
//lightPhysicalEntity
//An entity to which physics (gravity and not travelling through solid objects)
//applies -- loosely.
/******************************************************************************/

    lightPhysicalEntity::lightPhysicalEntity( float newx, float newy, uint8_t R, uint8_t G, uint8_t B,
                                  uint8_t A, float newSizeFactor, float newElasticity, float newXMomentum,
                                  float newYMomentum, float newMaxSpeed, float newGravity, float newFriction) :
                                  entity(newx, newy, R, G, B, A, newSizeFactor),
                                  xMomentum(newXMomentum),
                                  yMomentum(newYMomentum),
                                  elasticity(newElasticity),
                                  maxSpeed(newMaxSpeed),
                                  gravity(newGravity),
                                  friction(newFriction) {}

    void  lightPhysicalEntity::tickSet(collider& col) {
        yMomentum += gravity;

        yMomentum = min(yMomentum, maxSpeed);
        yMomentum = max(yMomentum, -1 * maxSpeed);
        xMomentum = min(xMomentum, maxSpeed);
        xMomentum = max(xMomentum, -1 * maxSpeed);

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
            xMomentum *= friction;
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
                                float newYMomentum, float newMaxSpeed, float newGravity, float newFriction) :
                                entity(newx, newy, R, G, B, A, newSizeFactor),
                                maxSpeed(newMaxSpeed),
                                gravity(newGravity),
                                friction(newFriction),
                                xMomentum(newXMomentum),
                                yMomentum(newYMomentum) {}

    void realPhysicalEntity::tickSet(collider& col) {
        yMomentum += gravity;

        float momentumMagnitude = pow(pow(xMomentum, 2) + pow(yMomentum, 2), 0.5);
        if (momentumMagnitude > maxSpeed) {
            xMomentum *= momentumMagnitude / maxSpeed;
            yMomentum *= momentumMagnitude / maxSpeed;
        }

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
                xMomentum *= friction;
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

/*****************************************************************************/
//pickUp
//Any sort of item that can be collected by the player once
/*****************************************************************************/

    pickUp::pickUp(  float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                    float newSizeFactor, int newDisplayChar, int newLifetime) :
        entity(newX, newY, R, G, B, A, newSizeFactor),
        displayChar(newDisplayChar),
        lifetime(newLifetime) {
    }

    bool pickUp::doesCollide(float otherX, float otherY, int otherType) {
        if (!collected && IsKeyPressed(KEY_S) && otherX > x - 1 && otherX < x + 1 && otherY > y - 1 && otherY < y + 1 && otherType == 1) {
            collected = true;
            return true;
        }
        return false;
    }

    collision pickUp::getCollision() {
        return collision(type);
    }

    bool pickUp::stopColliding() {
        return collected || lifetime < 0;
    }

    void pickUp::tickSet(collider& col) {
	lifetime--;
        myParticles.tickSet(col);
    }

    void pickUp::tickGet(collider& col) {
        if (collected && !exploded) {
            exploded = true;
            explosion(col, myParticles, 16, x, y, tint.r, tint.g, tint.b, tint.a, sizeFactor, 0.3, '*', 100, 0.5);
        }

        myParticles.tickGet(col);
    }

    bool pickUp::finalize() {
        myParticles.finalize();
        return (collected && myParticles.size() == 0) || (!(collected) && lifetime < 0);
    }

    void pickUp::print(float cameraX, float cameraY, Font displayFont) {
        myParticles.print(cameraX, cameraY, displayFont);
        if (!collected) {
            myDrawText(displayFont, TextToUtf8(&displayChar, 1), (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
        }
    }
