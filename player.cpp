#include "player.hpp"

/******************************************************************************/
//player
//an entity controlled by the user
//Also responsible for storing save data and drawing inventory screen
/******************************************************************************/

//Constructor

    player::player(float newX, float newY, Color newTint, float newSizeFactor) :
                       entity(newX, newY, newTint, newSizeFactor) {
        yInertia = 0;
        elasticity = 0;
        friction = 1.0f;
        health = maxHealth = 8;
        air = maxAir = 600;

        for (int i = 0; i < 10; i++) {
            channels[i] = bitset<8>("00000000");
            channelColors[i] = (Color){255, 255, 255, 255};
        }
    }



//Collision functions

    bool player::doesCollide(float otherX, float otherY, int otherType) {
        return ((otherType == WATERTYPE && lastTickUnderWater != isUnderWater) || (otherX >= x && otherX <= x + 1 && otherY >= y && otherY <= y + 1));
    }

    collision player::getCollision(float otherX, float otherY, int otherType) {
        if (otherType == WATERTYPE) {
            return collision(PHYSICALENTITYTYPE, 0, x, yInertia);
        }
        return collision(PLAYERTYPE);
    }

    bool player::stopColliding() {
        return false;
    }

//Fire a gun

    void player::fire(weapon& gun) {
        if (gun.unlocked) {
            if (gun.ammo != 0) {
                if (gun.lastFired + gun.cooldown < tickCounter) {
                    if (gun.automatic || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        Vector2 aim = theCanvas -> getMouseRelativeTo(x, y, sizeFactor);
                        aim = Vector2Normalize(aim);
                        bullet* b = new bullet(
                            //x, y, zPosition, color, sizeFactor
                            x + 1.5 * aim.x, y + 1.5 * aim.y, tint, sizeFactor,
                            //xInertia, yInertia, character, particleCount, lifetime
                            aim.x * gun.speed, aim.y * gun.speed, gun.bulletDisplay, gun.particleCount, gun.lifetime,
                            //Elasticity, max speed, gravity, friction
                            gun.elasticity, 10, gun.gravity, 0.5,
                            //Bullet damage, explosion power, explosion range
                            gun.damage, gun.explosionPower, gun.explosionRange,
                            //Does bullet explode upon wall? water? any entities? 
                            gun.hitWall, gun.hitWater, gun.hitEntity, gun.sticky);
                        world -> addCollideable(b, zPosition);
                        b -> setZPosition (zPosition);
                        gun.lastFired = tickCounter;
                        gun.ammo--;
                    }
                }
                else if (!gun.automatic) {
                    //gun cooldown indicator (noise?)
                }
            }
            else {
                //Gun ammo indicator (noise?)
            }
        }
    }

//Tick functions

    void player::tickSet() {

        //Reset break cases
        breakDoor = breakSave = breakDead = false;

        //Detect solid blocks
        bool onGround =
            world -> isSolid(x + (1 - width) / 2, y + 1) ||
            world -> isSolid(x + (1 + width) / 2, y + 1);
        bool onWall =
            world -> isSolid(x + (1 - width) / 2 - 0.1, y) ||
            world -> isSolid(x + (1 + width) / 2 + 0.1, y);

        //Keyboard movement handling
        if (!isUnderWater) {
            //Gravity
            yInertia = min(yInertia + gravity, 0.3f);
            yMovement = 0;
            //Jumping
            if ((IsKeyPressed(KEY_W) || IsKeyDown(KEY_W) && autoRejump)) {  //If the jump key is pressed
                if (onGround) {                                         //If we're on a surface we can jump from
                    yInertia  = -1 * jumpSpeed;                                 //then jump, and also reset jump counter
                    jumpsUsed = jumpCount;
                    justJumped = true;
                }
                else if (walljump && onWall) {
                    yInertia = -1 * jumpSpeed;
                    jumpsUsed = jumpCount;
                    justJumped = true;
                }
                else if (jumpsUsed != 0) {                                //Else, if jumps remaining,
                    jumpsUsed--;
                    yInertia = -1 * jumpSpeed;                                 //jump and decrement jump counter
                    if (IsKeyPressed(KEY_W)) {
                        justJumped = true;
                    }
                }
            }
            if (IsKeyReleased(KEY_W) && yInertia < 0 && justJumped) {
                yInertia *= 0.3;
                justJumped = false;
            }

            //Lateral movement
            if (IsKeyDown(KEY_D)) {
                xMovement = speed;
                if (xInertia < 0) {
                    xInertia *= 0.9;
                }
            }
            else if (IsKeyDown(KEY_A)) {
                xMovement = -1 * speed;
                if (xInertia > 0) {
                    xInertia *= 0.9;
                }
            }
            else {
                xMovement = 0;
                xInertia *= 0.9;
            }

            //Air recovery
            air = min(maxAir, air + AIRRECOVERYRATE);
        }
        else {  //Is underwater
            jumpsUsed = jumpCount;

            //Moving underwater
            if (IsKeyDown(KEY_W)) {
                yMovement = -1 * speed;
                if (yInertia > 0) {
                    yInertia *= 0.9;
                }
            }
            else if (IsKeyDown(KEY_S)) {
                yMovement = speed;
                if (yInertia < 0) {
                    yInertia *= 0.9;
                }
            }
            else {
                yMovement = 0;
                yInertia *= 0.9;
            }

            if (IsKeyDown(KEY_A)) {
                xMovement = -1 * speed;
                if (xInertia > 0) {
                    xInertia *= 0.9;
                }
            }
            else if (IsKeyDown(KEY_D)) {
                xMovement = speed;
                if (xInertia < 0) {
                    xInertia *= 0.9;
                }
            }
            else {
                xMovement = 0;
                xInertia *= 0.9;
            }

/*
            xMovement = 0;
            yMovement = 0;
            if (IsKeyDown(KEY_W)) {
                yInertia = max(-1 * waterSpeed, yInertia - waterAcceleration);
            }
            if (IsKeyDown(KEY_A)) {
                xInertia = max(-1 * waterSpeed, xInertia - waterAcceleration);
            }
            if (IsKeyDown(KEY_S)) {
                yInertia = min(waterSpeed, yInertia + waterAcceleration);
            }
            if (IsKeyDown(KEY_D)) {
                xInertia = min(waterSpeed, xInertia + waterAcceleration);
            }
*/

            //Drowning
            if (air == 0) {
                health--;
            }
            else {
                air--;
            }
        }

        //Channels
        for (int i = 0; i < 10; i++) {
            if (IsKeyDown(KEY_ZERO + i)) {
                world -> setChannel(channels[i].to_ulong());
                lastChannel = i;
                if (tickCounter % 8 == 0) {
                    broadcast(10, x, y, channelColors[i], sizeFactor, channels[i][(tickCounter % 64) / 8]);
                }
            }
        }
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            world -> setChannel(channels[lastChannel].to_ulong());
            if (tickCounter % 8 == 0) {
                broadcast(5, x, y, channelColors[lastChannel], sizeFactor, channels[lastChannel][(tickCounter % 64) / 8]);
            }
        }

        //Switching guns
        if (IsKeyPressed(KEY_Q)) {
            for (int i = 0; i < guns.size(); i++) {
                gunSelect = (gunSelect - 1) % guns.size();
                if (guns[gunSelect].unlocked) {
                    break;
                }
            }
        }
        if (IsKeyPressed(KEY_E)) {
            for (int i = 0; i < guns.size(); i++) {
                gunSelect = (gunSelect + 1) % guns.size();
                if (guns[gunSelect].unlocked) {
                    break;
                }
            }
        }

        //Shootin'
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && guns.size() > 0) {
            fire(guns[gunSelect]);
        }

        //Explosions
        if (IsKeyPressed(KEY_R)) {
            explode (60, x, y, tint, sizeFactor, 0.4, 0, 600, 0.5, zPosition);
        }

        //Death
        hurtTimer--;
        if (y > world -> getRows() + 25) {
            health = 0;
        }
        if (health <= 0) {
            breakDead = true;
            xInertia = 0;
            yInertia = 0;
            collisions.clear();
        }
    }

    void player::tickGet() {
        hit = false;
        lastTickUnderWater = isUnderWater;
        isUnderWater = false;
        xMoveWater = 0;
        yMoveWater = 0;

        //spikes
        int spikeDamage = world -> getPlayerDamage((int)(x + 0.5), (int)(y + 0.5));
        if (hurtTimer < 0 && spikeDamage) {
            health += spikeDamage;
            hurtTimer = 60;
            Vector2 newInertia = Vector2Scale(Vector2Negate(Vector2Normalize({xInertia, yInertia})), 0.7);
            xInertia = newInertia.x;
            yInertia = newInertia.y / 3;
            damageIndicator(spikeDamage, x, y, HURTCOLOR, sizeFactor);
        }

        //Handling collisions
        list<collision>::iterator colIter = collisions.begin();
        while (colIter != collisions.end()) {
            handleCollision(*colIter);
            colIter++;
        }
        collisions.clear();

        //Allow for jumping out of the water

        if (lastTickUnderWater && !isUnderWater) {
            yInertia = yMovement + yMoveWater;
            yMovement = 0;
        }

        //Update X and Y position based on 3 movement variables
        float numSteps = max(int(abs(xInertia + xMoveWater + xMovement)) + 1, int(abs(yInertia + yMoveWater + yMovement)) + 1);
        float xStep = (xInertia + xMoveWater + xMovement) / numSteps;
        float yStep = (yInertia + yMoveWater + yMovement) / numSteps;
        for (int i = 0; i < numSteps; i++) {
            if (world -> isSolid(int(x + xStep + 0.05 + 0.9 * (xStep > 0)), int(y + 0.05))
            || world -> isSolid(int(x + xStep + 0.05 + 0.9 * (xStep > 0)), int(y + 0.95))) {
                x = floor(x + xStep) + (xStep < 0);
                xInertia *= (-1 * elasticity);
                hit = true;
                xStep = 0;
            }
            else {
                x += xStep;
            }
            if (world -> isSolid(int(x + 0.05), int(y + yStep + 0.05 + 0.9 * (yStep > 0)))
            || world -> isSolid(int(x + 0.95), int(y + yStep + 0.05 + 0.9 * (yStep > 0)))) {
                y = floor(y + yStep) + (yStep < 0);
                yInertia *= (-1 * elasticity);
                xInertia *= friction;
                hit = true;
                yStep = 0;
            }
            else {
                y += yStep;
            }
        }
    }

    bool player::finalize() {
        return false;
    }

    void player::print() {
        if (!breakDoor) {   //Fix teleporting for one frame after going through door
            if (hurtTimer > 0 && (hurtTimer / 4) % 2 == 0) {    //Flash if recently taken damage
                theCanvas -> draw(x, y, HURTCOLOR, sizeFactor, displayStr, false);
            }
            else {
                theCanvas -> draw(x, y, tint, sizeFactor, displayStr, doLighting);
            }
        }
    }



