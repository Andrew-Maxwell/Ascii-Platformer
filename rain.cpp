#include "rain.hpp"

/******************************************************************************/
//Rain
//Constantly spawns particles above the top of the screen
/******************************************************************************/

    rain::rain(float newX, float newY, Color newTint, float newSizeFactor, float newDropsPerTick, float newXMomentum, bool newIsSnow) :
        entity(newX, newY, newTint, newSizeFactor),
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

                raindrop = new physicalParticle(GetRandomValue(0, col.getCols() * 10) / 10, GetRandomValue(0, 10) / 10, tint, sizeFactor, xMomentum, 0.2, snowflake, 1000, 0, 0.2, GRAVITY, 0);
            }
            else {
                raindrop = new physicalParticle(GetRandomValue(0, col.getCols() * 10) / 10, GetRandomValue(0, 10) / 10, tint, sizeFactor, xMomentum, 1, 0, 200, 0, 1, GRAVITY, 0.88);
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

