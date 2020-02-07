#include "col.hpp"

/******************************************************************************/
/*Collidor*/
/******************************************************************************/

    //Accessors

    collider::collider(float newX, float newY, string fileName) :
        entity(newX, newY, {0, 0, 0, 0}, 1),
        layer(newX, newY, {0, 0, 0, 0}, 1, fileName)
        {
            layer::readLayer();
        }

    void collider::addCollideable(collideable* newCollideable) {
        collideables.push_back(newCollideable);
    }

    void collider::addParticle(collideable* newParticle) {
        particles.push_back(newParticle);
    }

    bool collider::isSolid(int checkX, int checkY) {
        if (checkY >= 0 && checkY < canvas.size() && checkX >= 0 && checkX < canvas[checkY].size()) {
            return canvas[checkY][checkX] == 's' || canvas[checkY][checkX] == '8';
        }
        return false;
    }

    bool collider::isLiquid(int checkX, int checkY) {
        if (checkY >= 0 && checkY < canvas.size() && checkX >= 0 && checkX < canvas[checkY].size()) {
            return canvas[checkY][checkX] == 'w';
        }
        return false;
    }

    int collider::getPlayerDamage(int checkX, int checkY) {
        if (checkY >= 0 && checkY < canvas.size() && checkX >= 0 && checkX < canvas[checkY].size()) {
            if (canvas[checkY][checkX] == 'x' || canvas[checkY][checkX] == 'y') {
                return -2;
            }
            else if (canvas[checkY][checkX] == 'X' || canvas[checkY][checkX] == 'Y') {
                return -10;
            }
            else if (canvas[checkY][checkX] == 'Z') {
                return -500;
            }
        }
        return 0;
    }
    
    int collider::getDamage(int checkX, int checkY) {
        if (checkY >= 0 && checkY < canvas.size() && checkX >= 0 && checkX < canvas[checkY].size()) {
            if (canvas[checkY][checkX] == 'x') {
                return -2;
            }
            else if (canvas[checkY][checkX] == 'X') {
                return -10;
            }
            else if (canvas[checkY][checkX] == 'Z') {
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

