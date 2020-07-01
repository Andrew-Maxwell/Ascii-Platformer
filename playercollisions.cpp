#include "player.hpp"

    void player::handleCollision(collision& col) {
        switch(col.type) {
            case ENDINGGATETYPE:
                won = 1;
                break;
            case DOORTYPE:
                if (in.down.isPressed()) {
                    nextX = col.xVal;
                    nextY = col.yVal;
                    nextRoom = col.message;
                    breakDoor = true;
                }
                break;
            case SAVEPOINTTYPE: {
                breakSave = true;
                break;
            }
            case BULLETTYPE:
                if (hurtTimer < 0) {
                    yInertia += col.yVal * 0.3;
                    xInertia += col.xVal * 0.3;
                    health += (col.damage);
                    hurtTimer = 60;
                    damageIndicator(col.damage, x, y, HURTCOLOR, sizeFactor);
                }
                break;
            case ENEMYTYPE:
                if (hurtTimer < 0) {
                    health += (col.damage);
                    hurtTimer = 60;
                    damageIndicator(col.damage, x, y, HURTCOLOR, sizeFactor);
                    Vector2 newInertia = Vector2Scale(Vector2Negate(Vector2Normalize({xInertia, yInertia})), 0.7);
                    xInertia = newInertia.x;
                    yInertia = newInertia.y / 3;
                }
                break;
            case GUNPICKUPTYPE: {
                int pickupgunID = col.damage;
                for (int i = 0; i < guns.size(); i++) {
                    if (guns[i].gunID == pickupgunID) {
                        guns[i].unlocked = true;
                        //Kludge to make sure we select a locked gun if one is available
                        if (guns[gunSelect].unlocked == false) {
                            for (int i = 0; i < guns.size(); i++) {
                                gunSelect = (gunSelect + 1) % guns.size();
                                if (guns[gunSelect].unlocked) {
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }
                if (int(col.yVal) != (1 << 31)) {
                    collectedPickups.insert(int(col.yVal));
                }
                break;
            }
            case AMMOPICKUPTYPE: {
                int pickupgunID = col.damage;
                for (int i = 0; i < guns.size(); i++) {
                    if (guns[i].gunID == pickupgunID) {
                        guns[i].ammo = min(guns[i].maxAmmo, guns[i].ammo + int(col.xVal));
                        break;
                    }
                }
                if (int(col.yVal) != (1 << 31)) {
                    collectedPickups.insert(int(col.yVal));
                }
                break;
            }
            case OPPICKUPTYPE: {
                for (int i = 0; i < ops.size(); i++) {
                    if (ops[i].opID == col.damage) {
                        ops[i].unlocked = true;
                    }
                }
                if (int(col.yVal) != (1 << 31)) {
                    collectedPickups.insert(int(col.yVal));
                }
                break;
            }
            case OUTFITPICKUPTYPE: {
                handlePickup(col);
                if (int(col.yVal) != (1 << 31)) {
                    collectedPickups.insert(int(col.yVal));
                }
                break;
            }
            case FORCEFIELDTYPE: {
                jumpControl = false;
                justJumped = false;
                xInertia += col.xVal;
                yInertia += col.yVal;
                break;
            }
            case WATERTYPE: {
                isUnderWater = true;
                xMoveWater += col.xVal;
                yMoveWater += col.yVal;
            }
        }

    }

    void player::handlePickup(collision& col) {
        if (col.message == "display") {
            display = col.magnitude;
        }
        else if (col.message == "health") {
            if (col.damage) {//Add
                health = min(maxHealth, health + (int)col.magnitude);
            }
            else {
                health = min(maxHealth, (int)col.magnitude);
            }
        }
        else if (col.message == "maxHealth") {
            if (col.damage) {//Add
                maxHealth += col.magnitude;
            }
            else {
                maxHealth = col.magnitude;
            }
        }
        else if (col.message == "air") {
            if (col.damage) {//Add
                air = min(maxAir, air + (int)col.magnitude);
            }
            else {
                air = min(maxAir, (int)col.magnitude);
            }
        }
        else if (col.message == "maxAir") {
            if (col.damage) {//Add
                maxAir += col.magnitude;
            }
            else {
                maxAir = col.magnitude;
            }
        }
        else if (col.message == "elasticity") {
            if (col.damage) {//Add
                elasticity += col.magnitude;
            }
            else {
                elasticity = col.magnitude;
            }
        }
        else if (col.message == "acceleration") {
            if (col.damage) {//Add
                acceleration += col.magnitude;
            }
            else {
                acceleration = col.magnitude;
            }
        }
        else if (col.message == "speed") {
            if (col.damage) {//Add
                speed += col.magnitude;
            }
            else {
                speed = col.magnitude;
            }
        }
        else if (col.message == "friction") {
            if (col.damage) {//Add
                playerFriction += col.magnitude;
            }
            else {
                playerFriction = col.magnitude;
            }
        }
        else if (col.message == "waterAcceleration") {
            if (col.damage) {//Add
                waterAcceleration += col.magnitude;
            }
            else {
                waterAcceleration = col.magnitude;
            }
        }
        else if (col.message == "waterFriction") {
            if (col.damage) {//Add
                waterFriction += col.magnitude;
            }
            else {
                waterFriction = col.magnitude;
            }
        }
        else if (col.message == "jumpSpeed") {
            if (col.damage) {//Add
                jumpSpeed += col.magnitude;
            }
            else {
                jumpSpeed = col.magnitude;
            }
        }
        else if (col.message == "jumpCount") {
            if (col.damage) {//Add
                jumpCount += col.magnitude;
            }
            else {
                jumpCount = col.magnitude;
            }
        }
        else if (col.message == "autoRejump") {
            if (col.damage) {//Add
                autoRejump += col.magnitude;
            }
            else {
                autoRejump = col.magnitude;
            }
        }
        else if (col.message == "walljump") {
            if (col.damage) {//Add
                walljump += col.magnitude;
            }
            else {
                walljump = col.magnitude;
            }
        }
        else {
            cerr << "Unknown outfit pickup type";
        }
    }
