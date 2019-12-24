#include "meta.hpp"
#include "game_meta_classes.hpp"

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
        DrawTextEx(displayFont, toPrint, (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
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
            xMomentum *= FRICTION / 2;
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
