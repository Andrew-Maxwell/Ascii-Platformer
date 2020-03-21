#include "rain.hpp"

/******************************************************************************/
//drop
//Physical particle that disappears on contact with water.
/******************************************************************************/

    drop::drop( float newx, float newy, Color newTint, float newSizeFactor, int displayChar, float elasticity, float newXMomentum,
              float newYMomentum, float newMaxSpeed, float newGravity, float newFriction, int newLifetime) :
                entity(newx, newy, newTint, newSizeFactor),
                physicalParticle(newx, newy, newTint, newSizeFactor, displayChar, elasticity, newXMomentum,
                newYMomentum, newMaxSpeed, newGravity, newFriction, newLifetime) {}

    unsigned int drop::type() {
        return DROPTYPE;
    }

    void drop::tickGet() {
        physicalParticle::tickGet();
        if (lastTickUnderWater) {
            shouldDelete = true;
        }
    }


/******************************************************************************/
//Rain
//Constantly spawns particles above the top of the screen
/******************************************************************************/

    rain::rain(float newX, float newY, Color newTint, float newSizeFactor,  float newDropsPerTick, float newXMomentum, bool newIsSnow) :
        entity(newX, newY, newTint, newSizeFactor),
        dropsPerTick(newDropsPerTick),
        xMomentum(newXMomentum),
        isSnow(newIsSnow) {}

    unsigned int rain::type() {
        return RAINTYPE;
    }

    void rain::tickSet() {
        //Do rain for a bit on first tick so that raindrops appear to already have been falling when the room is loaded
        //Note that since collider can't be run this way, interactions with other entities won't work properly
        //in these preloaded raindrops. Probably not an issue

        dropBuffer += dropsPerTick;
        while (dropBuffer > 1) {
            dropBuffer--;
            drop* raindrop;
            if (isSnow) {
                int snowflake = 0;
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
                raindrop = new drop(GetRandomValue(0, world -> getCols() * 10) / 10, GetRandomValue(0, 10) / 10, tint, sizeFactor, snowflake, 0, xMomentum, 0.2, 0.2, GRAVITY, 0.5, 1000);
            }
            else {
                raindrop = new drop(GetRandomValue(0, world -> getCols() * 10) / 10, GetRandomValue(0, 10) / 10, tint, sizeFactor, 0, 0, xMomentum, 0.7, 0.7, GRAVITY, 0.5, 200);
            }
            world -> addParticle(raindrop);
        }
    }

    void rain::tickGet() {}

    bool rain::finalize() {
        return false;
    }

    void rain::print() {}

