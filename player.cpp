#include "player.hpp"

/******************************************************************************/
//player
//an entity controlled by the user
//Also responsible for storing save data and drawing inventory screen
/******************************************************************************/

//Constructor

    player::player(float newX, float newY, Color newTint, float newScale) :
                       entity(newX, newY, newTint, newScale) {
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

    bool player::doesCollide(float otherX, float otherY, int otherType, unsigned int otherID) {
        return ((otherType == TRIGGERTYPE) || (otherType == WATERTYPE && lastTickUnderWater != isUnderWater) || (otherX >= x - 1 && otherX <= x + 1 && otherY >= y - 1 && otherY <= y + 1));
    }

    collision player::getCollision(float otherX, float otherY, int otherType, unsigned int otherID) {
        if (otherType == WATERTYPE) {
            return collision(PHYSICALENTITYTYPE, id, 0, x, yInertia);
        }
        else if (in.interact.isPressed()) {
            return collision(PLAYERTYPE, id, 2, x, y);
        }
        else if (in.interact.isDown()) {
            return collision(PLAYERTYPE, id, 1, x, y);
        }
        else {
            return collision(PLAYERTYPE, id, 0, x, y);
        }
    }

    bool player::stopColliding() {
        return breakDead;
    }

//Fire a gun

    void player::fire(weapon& gun) {
        if (gun.unlocked) {
            if (gun.ammo != 0) {
                if (gun.lastFired + gun.cooldown < tickCounter) {
                    if (gun.automatic || IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || in.fire.isPressed()) {
                        Vector2 aim;
                        if (in.useMouseAim) {
                            aim = theScreen -> getMouseRelativeTo(x, y, scale);
                            aim = Vector2Normalize(aim);
                        }
                        else {
                            aim = in.getAim();
                        }
                        bullet* b = new bullet(
                            //x, y, zPosition, color, scale
                            x + 1.5 * aim.x, y + 1.5 * aim.y,  tint, scale,
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
        in.update();    //Update input data

        if (!breakDoor && !breakDead) {

            focusCamera = in.focusCamera.isPressed();

            if (in.inventory.isPressedOnce()) {
                breakInventory = true;
            }

            //Channels
            for (int i = 0; i < 10; i++) {
                if (in.code[i].isDown()) {
                    world -> setChannel(channels[i].to_ulong(), false);
                    lastChannel = i;
                    if (tickCounter % 8 == 0) {
                        broadcast(2, x, y, channelColors[i], scale, channels[i][(tickCounter % 64) / 8]);
                    }
                }
            }
            if (in.lastCode.isDown()) {
                world -> setChannel(channels[lastChannel].to_ulong(), false);
                if (tickCounter % 8 == 0) {
                    broadcast(2, x, y, channelColors[lastChannel], scale, channels[lastChannel][(tickCounter % 64) / 8]);
                }
            }

            //Switching channels
            if (in.previousCode.isPressed()) {
                lastChannel = (lastChannel - 1 + 10) % 10;
            }
            else if (in.nextCode.isPressed()) {
                lastChannel = (lastChannel + 1) % 10;
            }

            //Switching guns
            if (in.previousWeapon.isPressed()) {
                for (int i = 0; i < guns.size(); i++) {
                    gunSelect = (gunSelect - 1) % guns.size();
                    if (guns[gunSelect].unlocked) {
                        break;
                    }
                }
            }
            if (in.nextWeapon.isPressed()) {
                for (int i = 0; i < guns.size(); i++) {
                    gunSelect = (gunSelect + 1) % guns.size();
                    if (guns[gunSelect].unlocked) {
                        break;
                    }
                }
            }

            //Shootin'
            if (((in.useMouseAim && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) || (!in.useMouseAim && in.fire.isDown())) && guns.size() > 0) {
                fire(guns[gunSelect]);
            }

            //Explosions
            if (IsKeyPressed(KEY_T)) {
                physicalParticle* p = new physicalParticle(x, y, tint, scale, 'A', 0, 0, 0, 100, gravity, friction, 1000);
                world -> addParticle(p, zPosition);
            }

            if (in.explode.isPressed()) {
                explode (60, x, y, tint, scale, 0.4, 0x2022, 600, 0.3, zPosition);
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
        else if (breakDoor) {
            if (in.interact.isPressedOnce()) {
                breakDoor = false;
            }
        }
    }

    void player::tickGet() {

        if (!breakDoor && !breakDead) {

            lastTickUnderWater = isUnderWater;
            isUnderWater = false;
            pushedX = 0;
            pushedY = 0;

            //Handling collisions
            list<collision>::iterator colIter = collisions.begin();
            while (colIter != collisions.end()) {
                handleCollision(*colIter);
                colIter++;
            }
            collisions.clear();

            //Allow for jumping out of the water

            if (lastTickUnderWater && !isUnderWater) {
                yInertia = yMovement + pushedY;
                yMovement = 0;
            }

            //Keyboard movement handling
            if (!isUnderWater) {
                //Gravity
                yInertia = min(yInertia + gravity, maxFallSpeed);
                yMovement = 0;
                //Jumping
                if (in.jump.isPressed() || (in.jump.isDown() && autoRejump)) {  //If the jump key is pressed
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
                        if (in.up.isPressed()) {
                            justJumped = true;
                        }
                    }
                }
                if (in.jump.isReleased() && yInertia < 0 && justJumped) {
                    yInertia *= 0.3;
                    justJumped = false;
                }

                //Lateral movement
                if (in.right.isDown()) {
                    xMovement = speed * in.right.value;
                    if (xInertia < 0) {
                        xInertia *= 0.9;
                    }
                }
                else if (in.left.isDown()) {
                    xMovement = -1 * speed * in.left.value;
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
                if (in.up.isDown()) {
                    yMovement = -1 * speed * in.up.value;
                    if (yInertia > 0) {
                        yInertia *= 0.9;
                    }
                }
                else if (in.down.isDown()) {
                    yMovement = speed * in.down.value;
                    if (yInertia < 0) {
                        yInertia *= 0.9;
                    }
                }
                else {
                    yMovement = 0;
                    yInertia *= 0.9;
                }

                if (in.left.isDown()) {
                    xMovement = -1 * speed * in.left.value;
                    if (xInertia > 0) {
                        xInertia *= 0.9;
                    }
                }
                else if (in.right.isDown()) {
                    xMovement = speed * in.right.value;
                    if (xInertia < 0) {
                        xInertia *= 0.9;
                    }
                }
                else {
                    xMovement = 0;
                    xInertia *= 0.9;
                }
                //Drowning
                if (air == 0) {
                    health--;
                }
                else {
                    air--;
                }
            }


            //spikes
            int spikeDamage = world -> getPlayerDamage((int)(x + 0.5), (int)(y + 0.5));
            if (hurtTimer < 0 && spikeDamage) {
                health += spikeDamage;
                hurtTimer = 60;
                Vector2 newInertia = Vector2Scale(Vector2Negate(Vector2Normalize({xInertia, yInertia})), 0.7);
                xInertia = newInertia.x;
                yInertia = newInertia.y / 3;
                damageIndicator(spikeDamage, x, y, HURTCOLOR, scale);
            }

            hit = onGround = onWall = false;

            //Update X and Y position based on 3 movement variables
            float dX = xInertia + pushedX + xMovement;
            float dY = yInertia + pushedY + yMovement;
            bool hitX, hitY;
            Vector2 newPosition = world -> go((Vector2){x, y}, (Vector2){dX, dY}, width, height, hitX, hitY, xInertia, yInertia, elasticity, id);
            if (hitX) {
//                xInertia *= (-1 * elasticity);
                hit = true;
                onWall = true;
            }
            if (hitY) {
//                yInertia *= (-1 * elasticity);
                xInertia *= friction;
                hit = true;
                if (newPosition.y < y + dY) {
                    onGround = true;
                }
            }
            x = newPosition.x;
            y = newPosition.y;

            //Allow for being crushed
            if (world -> isSolid(x + width / 2, y + height / 2)) {
                cout << "Crushed at (" << x << ", " << y << ")\n";
                health -= 999;
                damageIndicator(-999, x, y, HURTCOLOR, scale);
            }


        }
    }

    bool player::finalize() {
        return breakDead;
    }

    void player::print() {
        if (!breakDoor && !breakDead) {
            if (hurtTimer > 0 && (hurtTimer / 4) % 2 == 0) {    //Flash if recently taken damage
                theScreen -> draw(x, y, HURTCOLOR, scale, displayStr, false, doHighlight);
            }
            else {
                theScreen -> draw(x, y, tint, scale, displayStr, doLighting, doHighlight);
            }
        }
    }

    void player::printHud() {

        //Print selected code

        theScreen -> drawHud(1 + 8 * playerNo, 3, channelColors[lastChannel], to_string(lastChannel));

        //Print gun info

        int rowCount = 0;
        int j = gunSelect;
        for (int i = 0; i < min(int(guns.size()), 4); i++) {
            if (guns[j].unlocked) {
                if (tickCounter < guns[j].lastFired + guns[j].cooldown || guns[j].ammo == 0) {
                    theScreen -> drawHud(1 + 8 * playerNo, ++rowCount + 3, guns[j].tintFaded, guns[j].display);
                    if (guns[j].ammo >= 0) {
                        theScreen -> drawHud(2 + 8 * playerNo, rowCount + 3, guns[j].tintFaded, to_string(guns[j].ammo));
                    }
                }
                else {
                    theScreen -> drawHud(1 + 8 * playerNo, ++rowCount + 3, guns[j].tint, guns[j].display);
                    if (guns[j].ammo >= 0) {
                        theScreen -> drawHud(2 + 8 * playerNo, rowCount + 3, guns[j].tint, to_string(guns[j].ammo));
                    }
                }
            }
            j = ((j + 1) % guns.size());
        }

        //Health background bar

        theScreen -> drawHudBarRight(1 + 8 * playerNo, 1, BLACK, min(maxHealth / 8.0f, 7.0f));

        //Health bar

        if (hurtTimer > 0 && (hurtTimer / 4) % 2 == 0) {    //If hurt, flash
            theScreen -> drawHudBarRight(1 + 8 * playerNo, 1, HURTCOLOR, min(health / 8.0f, 7.0f));
        }
        else {
            theScreen -> drawHudBarRight(1 + 8 * playerNo, 1, tint, min(health / 8.0f, 7.0f));
        }

        //Air bar

        if (air < maxAir) {
            theScreen -> drawHudBarRight(1 + 8 * playerNo, 2, AIRCOLOR, min(air / 480.0f, 7.0f));
        }
    }



