#include "player.hpp"

    unsigned int player::type() {
        return PLAYERTYPE;
    }

//Outfit handling functions

    outfit player::getCurrentOutfit() {
        outfit toReturn;
        toReturn.name = outfitName;
        toReturn.display = display;
        toReturn.health = health;
        toReturn.maxHealth = maxHealth;
        toReturn.air = air;
        toReturn.maxAir = maxAir;
        toReturn.elasticity = elasticity;
        toReturn.gravity = gravity;

        toReturn.acceleration = acceleration;
        toReturn.speed = speed;
        toReturn.friction = playerFriction;
        toReturn.waterAcceleration = waterAcceleration;
        toReturn.waterSpeed = waterSpeed;
        toReturn.waterFriction = waterFriction;

        toReturn.jumpSpeed = jumpSpeed;
        toReturn.jumpCount = jumpCount;
        toReturn.autoRejump = autoRejump;
        toReturn.walljump = walljump;
        toReturn.guns = guns;
        toReturn.ops = ops;
        for (int i = 0; i < 10; i++) {
            toReturn.channels[i] = channels[i];
        }
        toReturn.collectedPickups = collectedPickups;
        return toReturn;
    }

    void player::setOutfit(outfit newOutfit) {
        outfitName = newOutfit.name;
        display = newOutfit.display;
        displayStr = string(TextToUtf8(&display, 1));
        health = newOutfit.health;
        maxHealth = newOutfit.maxHealth;
        hurtTimer = -10000000;
        air = newOutfit.air;
        maxAir = newOutfit.maxAir;
        elasticity = newOutfit.elasticity;
        gravity = newOutfit.gravity;

        acceleration = newOutfit.acceleration;
        speed = newOutfit.speed;
        playerFriction = newOutfit.friction;
        waterAcceleration = newOutfit.waterAcceleration;
        waterSpeed = newOutfit.waterSpeed;
        waterFriction = newOutfit.waterFriction;

        jumpSpeed = newOutfit.jumpSpeed;
        jumpCount = newOutfit.jumpCount;
        autoRejump = newOutfit.autoRejump;
        walljump = newOutfit.walljump;
        guns = newOutfit.guns;
        gunSelect = 0;

        //Kludge to make sure we don't leave gunSelect on a locked gun
        //if an unlocked gun is available

        for (int i = 0; i < guns.size(); i++) {
            if (guns[gunSelect].unlocked) {
                break;
            }
            gunSelect = (gunSelect + 1) % guns.size();
        }
        ops = newOutfit.ops;
        for (int i = 0; i < 10; i++) {
            channels[i] = newOutfit.channels[i];
            channelColors[i].r = 255 - (channels[i] & bitset<8>(192)).to_ulong();
            channelColors[i].g = 255 - ((channels[i] & bitset<8>(56)).to_ulong() << 2);
            channelColors[i].b = 255 - ((channels[i] & bitset<8>(7)).to_ulong() << 5);
        }
        collectedPickups = newOutfit.collectedPickups;
    }

    void player::moveTo(Vector2 position) {
        x = position.x;
        y = position.y;
    }

    Vector2 player::getPosition() {
        return (Vector2){x, y};
    }

    set<int> player::getCollectedPickups() {
        return collectedPickups;
    }

//Accessors

    void player::setColor(Color newTint) {
        tint = newTint;
    }

    void player::setSizeFactor(float newSizeFactor) {
        sizeFactor = newSizeFactor;
    }

    float player::getSizeFactor() {
        return sizeFactor;
    }

    void player::goToDoor() {
        x = nextX;
        y = nextY;
    }
