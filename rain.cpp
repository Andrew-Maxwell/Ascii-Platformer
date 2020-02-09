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



    void drop::tickGet(collider& col) {
        physicalParticle::tickGet(col);
        if (isUnderWater) {
            shouldDelete = true;
        }
    }


/******************************************************************************/
//Rain
//Constantly spawns particles above the top of the screen
/******************************************************************************/

    rain::rain(float newX, float newY, Color newTint, float newSizeFactor, entityList* newEList, float newDropsPerTick, float newXMomentum, bool newIsSnow) :
        entity(newX, newY, newTint, newSizeFactor),
        dropsPerTick(newDropsPerTick),
        xMomentum(newXMomentum),
        isSnow(newIsSnow),
        firstTick(true) {
            eList = newEList;
        }

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

/*  physicalParticle constructor:
                        float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                        float newSizeFactor, float newXSpeed, float newYSpeed, int c, int newLifetime,
                        float newElasticity, float newMaxSpeed, float newGravity, float newFriction) :*/

                raindrop = new drop(GetRandomValue(0, col.getCols() * 10) / 10, GetRandomValue(0, 10) / 10, tint, sizeFactor, snowflake, 0, xMomentum, 0.2, 0.2, GRAVITY, 0.5, 1000);
            }
            else {
                raindrop = new drop(GetRandomValue(0, col.getCols() * 10) / 10, GetRandomValue(0, 10) / 10, tint, sizeFactor, 0, 0, xMomentum, 0.7, 0.7, GRAVITY, 0.5, 200);
            }
            eList -> addEntity(raindrop);
            col.addParticle(raindrop);
        }
    }

    void rain::tickGet(collider& col) {}

    bool rain::finalize() {
        return false;
    }

    void rain::print(float cameraX, float cameraY, Font displayFont) {}

