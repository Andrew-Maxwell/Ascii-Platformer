#include "player.hpp"

/******************************************************************************/
//player
//an entity controlled by the user
//Also responsible for storing save data and drawing inventory screen
/******************************************************************************/

//Constructor

    player::player(  float newX, float newY, Color newTint, float newSizeFactor) :
                            entity(newX, newY, newTint, newSizeFactor),
                            physicalEntity(newX, newY, newTint, newSizeFactor, 1.0, 0.0, 0.0)
    {
        yMomentum = 0;
        elasticity = 0;
        health = maxHealth = 8;
        air = maxAir = 600;

        for (int i = 0; i < 10; i++) {
            channels[i] = bitset<8>("00000000");
        }
    }

    unsigned int player::type() {
        return PLAYERTYPE;
    }

//Outfit handling functions

    outfit player::getCurrentOutfit() {
        outfit toReturn;
        toReturn.name = outfitName;
        toReturn.health = health;
        toReturn.maxHealth = maxHealth;
        toReturn.air = air;
        toReturn.maxAir = maxAir;
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
        health = newOutfit.health;
        maxHealth = newOutfit.maxHealth;
        hurtTimer = -10000000;
        air = newOutfit.air;
        maxAir = newOutfit.maxAir;
        guns = newOutfit.guns;
        gunSelect = 0;
        ops = newOutfit.ops;
        for (int i = 0; i < 10; i++) {
            channels[i] = newOutfit.channels[i];
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

    Vector2 player::getPos() {
        return (Vector2){x, y};
    }

//Collision functions

    bool player::doesCollide(float otherX, float otherY, int otherType) {
        return (otherX >= x && otherX <= x + 1 && otherY >= y && otherY <= y + 1) || physicalEntity::doesCollide(otherX, otherY, otherType);
    }

    collision player::getCollision(float otherX, float otherY, int otherType) {
        if (otherType == WATERTYPE) {
            return physicalEntity::getCollision(otherX, otherY, otherType);
        }
        collision c(1);
        return c;
    }

    bool player::stopColliding() {
        return false;
    }

//Fire a gun

    void player::fire(weapon& gun) {
        if (gun.ammo > 0 && gun.unlocked) {
            switch(gun.gunType) {
                case 1:
                    if (gun.lastFired + gun.cooldown < tickCounter) {
                        Vector2 aim = theCanvas -> getMouseRelativeTo(x, y, sizeFactor);
                        aim = Vector2Scale(Vector2Normalize(aim), 0.5);
                        bullet* b = new bullet(x + 3 * aim.x, y + 3 * aim.y, tint, sizeFactor, aim.x, aim.y, 0, 120, 0, 10, GRAVITY, 0, -10, -0.5, 20);
                        world -> addCollideable(b);
                        gun.lastFired = tickCounter;
                        gun.ammo--;
                    }
                    else {
                        //play cooldown noise
                    }
                    break;
                default:
                    cout << "Bad gun.\n";
                    break;
            }
        }
        else {
            //play out of ammo noise
        }
    }

//Tick functions

    void player::tickSet() {

        //Reset break cases

        breakDoor = breakSave = breakDead = false;

        //Explosions

        if (IsKeyPressed(KEY_R)) {
            explode (60, x, y, tint, sizeFactor, 1, 0, 600, 0.5);
        }

        //Movement in air


        if (IsKeyPressed(KEY_W)) {
            if (world -> isSolid(x + (1 - width) / 2, y + 1) || world -> isSolid(x + (1 + width) / 2, y + 1))  {
                yMomentum -= JUMPSPEED;
            }
        }


        if (!isUnderWater) {
            if (IsKeyDown(KEY_D)) {
                if (xMomentum < 0) {
                    xMomentum /= 2;
                    xMomentum += PLAYERSPEED;
                }
                else if (xMomentum < PLAYERMAXSPEED) {
                    xMomentum = min(xMomentum + PLAYERSPEED, PLAYERMAXSPEED);
                }
            }
            if (IsKeyDown(KEY_A)) {
                if (xMomentum > 0) {
                    xMomentum /= 2;
                    xMomentum -= PLAYERSPEED;
                }
                else if (xMomentum > 0 - PLAYERMAXSPEED) {
                    xMomentum = max(xMomentum - PLAYERSPEED, 0 - PLAYERMAXSPEED);
                }
            }
            else if (IsKeyUp(KEY_A) && IsKeyUp(KEY_D)) {
                xMomentum *= 0.85;
            }

          	//Short jumps

            if (IsKeyReleased(KEY_W)) {
                yMomentum = max(0.0f, yMomentum);
            }

            //Air recovery

            air = min(maxAir, air + AIRRECOVERYRATE);

        }
        else {

            //Moving underwater

            if (IsKeyDown(KEY_W)) {
                yMomentum -= SWIMSPEED;
            }
            if (IsKeyDown(KEY_S)) {
                yMomentum += SWIMSPEED;
            }
            if (IsKeyDown(KEY_A)) {
                xMomentum -= SWIMSPEED;
            }
            if (IsKeyDown(KEY_D)) {
                xMomentum += SWIMSPEED;
            }

            //Drowning

            if (air == 0) {
                health--;
            }
            else {
                air--;
            }
        }

        //Channels

        if (IsKeyDown(KEY_ONE)) {
            world -> setChannel(channels[0].to_ulong());
        }
        if (IsKeyDown(KEY_TWO)) {
            world -> setChannel(channels[1].to_ulong());
        }
        if (IsKeyDown(KEY_THREE)) {
            world -> setChannel(channels[2].to_ulong());
        }
        if (IsKeyDown(KEY_FOUR)) {
            world -> setChannel(channels[3].to_ulong());
        }
        if (IsKeyDown(KEY_FIVE)) {
            world -> setChannel(channels[4].to_ulong());
        }
        if (IsKeyDown(KEY_SIX)) {
            world -> setChannel(channels[5].to_ulong());
        }
        if (IsKeyDown(KEY_SEVEN)) {
            world -> setChannel(channels[6].to_ulong());
        }
        if (IsKeyDown(KEY_EIGHT)) {
            world -> setChannel(channels[7].to_ulong());
        }
        if (IsKeyDown(KEY_NINE)) {
            world -> setChannel(channels[8].to_ulong());
        }
        if (IsKeyDown(KEY_ZERO)) {
            world -> setChannel(channels[9].to_ulong());
        }
        //Boollets

        //Switching guns

        if (IsKeyPressed(KEY_Q)) {
            gunSelect = (gunSelect - 1) % guns.size();
        }
        if (IsKeyPressed(KEY_E)) {
            gunSelect = (gunSelect + 1) % guns.size();
        }

        //Shootin'

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && guns.size() > 0) {
            fire(guns[gunSelect]);
        }

        //Spikes, falling, and death
        
        hurtTimer--;
        if (y > world -> getRows() + 25) {
            health = 0;
        }
        if (health <= 0) {
            breakDead = true;
            xMomentum = 0;
            yMomentum = 0;
            collisions.clear();
        }

        //spikes
        
        int spikeDamage = world -> getPlayerDamage((int)(x + 0.5), (int)(y + 0.5));
        if (hurtTimer < 0 && spikeDamage) {
            health += spikeDamage;
            hurtTimer = 60;
            Vector2 newMomentum = Vector2Scale(Vector2Negate(Vector2Normalize({xMomentum, yMomentum})), 0.7);
            xMomentum = newMomentum.x;
            yMomentum = newMomentum.y / 3;
            damageIndicator(spikeDamage, x, y, HURTCOLOR, sizeFactor);
        }
        physicalEntity::tickSet();;
    }

    void player::tickGet() {
        list<collision>::iterator colIter = collisions.begin();
        while (colIter != collisions.end()) {
            switch(colIter -> type) {
                case ENDINGGATETYPE:
                    won = 1;
                    colIter = collisions.erase(colIter);
                    break;
                case DOORTYPE:
                    if (IsKeyPressed(KEY_S)) {
                        x = colIter -> xVal;
                        y = colIter -> yVal;
                        nextRoom = colIter -> message;
                        breakDoor = true;
                    }
                    colIter = collisions.erase(colIter);
                    break;
                case SAVEPOINTTYPE: {
                    breakSave = true;
                    colIter = collisions.erase(colIter);
                    break;
                }
                case BULLETTYPE:
                    if ((hurtTimer < 0)) {
                        yMomentum += colIter -> yVal * 0.3;
                        xMomentum += colIter -> xVal * 0.3;
                        health += (colIter -> damage);
                        hurtTimer = 60;
                        damageIndicator(colIter -> damage, x, y, HURTCOLOR, sizeFactor);
                    }
                    colIter = collisions.erase(colIter);
                    break;
                case GUNPICKUPTYPE: {
                    int pickupgunID = colIter -> damage;
                    for (int i = 0; i < guns.size(); i++) {
                        if (guns[i].gunID == pickupgunID) {
                            guns[i].unlocked = true;
                            break;
                        }
                    }
                    if (int(colIter -> yVal)) {
                        collectedPickups.insert(int(colIter -> yVal));
                    }
                    colIter = collisions.erase(colIter);
                    break;
                }
                case AMMOPICKUPTYPE: {
                    int pickupgunID = colIter -> damage;
                    for (int i = 0; i < guns.size(); i++) {
                        if (guns[i].gunID == pickupgunID) {
                            guns[i].ammo = min(guns[i].maxAmmo, guns[i].ammo + int(colIter -> xVal));
                            break;
                        }
                    }
                    if (int(colIter -> yVal)) {
                        collectedPickups.insert(int(colIter -> yVal));
                    }
                    colIter = collisions.erase(colIter);
                    break;
                }
                case HEALTHPICKUPTYPE:
                    damageIndicator(colIter -> damage, x, y, HEALTHCOLOR, sizeFactor);
                    health = min(maxHealth, health + colIter -> damage);
                    if (int(colIter -> yVal)) {
                        collectedPickups.insert(int(colIter -> yVal));
                    }
                    colIter = collisions.erase(colIter);
                    break;
                case MAXHEALTHPICKUPTYPE:
                    health += (colIter -> damage);
                    damageIndicator(colIter -> damage, x, y, HEALTHCOLOR, sizeFactor);
                    maxHealth += (colIter -> damage);
                    if (int(colIter -> yVal)) {
                        collectedPickups.insert(int(colIter -> yVal));
                    }
                    colIter = collisions.erase(colIter);
                    break;
                case AIRPICKUPTYPE:
                    air = min(maxAir, air + colIter -> damage);
                    damageIndicator(colIter -> damage, x, y, AIRCOLOR, sizeFactor);
                    if (int(colIter -> yVal)) {
                        collectedPickups.insert(int(colIter -> yVal));
                    }
                    colIter = collisions.erase(colIter);
                    break;
                case MAXAIRPICKUPTYPE:
                    air += (colIter -> damage);
                    damageIndicator(colIter -> damage, x, y, AIRCOLOR, sizeFactor);
                    maxAir += (colIter -> damage);
                    if (int(colIter -> yVal)) {
                        collectedPickups.insert(int(colIter -> yVal));
                    }
                    colIter = collisions.erase(colIter);
                    break;
                case OPPICKUPTYPE: {
                    for (int i = 0; i < ops.size(); i++) {
                        if (ops[i].opID == colIter -> damage) {
                            ops[i].unlocked = true;
                        }
                    }
                    if (int(colIter -> yVal)) {
                        collectedPickups.insert(int(colIter -> yVal));
                    }
                    colIter = collisions.erase(colIter);
                    break;
                }
                default:
                    colIter++;
            }
        }
        physicalEntity::tickGet();
    }

    bool player::finalize() {
        return false;
    }

    void player::print() {
        if (!breakDoor) {   //Fix teleporting for one frame after going through door
            if (hurtTimer > 0 && (hurtTimer / 4) % 2 == 0) {    //Flash if recently taken damage
                theCanvas -> draw(x, y, HURTCOLOR, sizeFactor, "@");
            }
            else {
                theCanvas -> draw(x, y, tint, sizeFactor, "@");
            }
        }
        drawHud();
    }

    void player::drawHud() {

        //Print gun info

        int rowCount = 0;
        for (int i = 0; i < guns.size(); i++) {
            if (guns[i].unlocked) {
                if (tickCounter < guns[i].lastFired + guns[i].cooldown || guns[i].ammo < 1) {
                    theCanvas -> drawHud(1, ++rowCount + 3, guns[i].tintFaded, guns[i].display);
                    theCanvas -> drawHud(2, rowCount + 3, guns[i].tintFaded, to_string(guns[i].ammo));
                }
                else {
                    theCanvas -> drawHud(1, ++rowCount + 3, guns[i].tint, guns[i].display);
                    theCanvas -> drawHud(2, rowCount + 3, guns[i].tint, to_string(guns[i].ammo));
                }
            }
        }

        //Health background bar

        theCanvas -> drawHudBarRight(1, 1, UIBACKGROUND, maxHealth / 8.0f);

        //Health bar

        if (hurtTimer > 0 && (hurtTimer / 4) % 2 == 0) {    //If hurt, flash
            theCanvas -> drawHudBarRight(1, 1, HURTCOLOR, health / 8.0f);
        }
        else {
            theCanvas -> drawHudBarRight(1, 1, HEALTHCOLOR, health / 8.0f);
        }

        //Air bar

        if (air < maxAir) {
            theCanvas -> drawHudBarRight(1, 2, AIRCOLOR, air / 64.0);
        }
    }

    //Used in bitset handling

    void apply(bitset<8>* current, int op, int arg) {
        switch(op) {
            case 0: {   //do nothing
                break;
            }
            case 6: {   //Load
                bitset<8> toReturn(arg);
                *current = toReturn;
                break;
            }
            case 1: {   //Rotate
                bitset<8> toReturn;
                for (int i = 0; i < 8; i++) {
                    toReturn[i] = (*current)[(i + arg)&7];
                }
                for (int i = 0; i < 8; i++) {
                    (*current)[i] = toReturn[i];
                }
                break;
            }
            case 2: {   //Shift
                if (arg > 0) {
                    *current >>= arg;
                }
                else {
                    *current <<= (-1 * arg);
                }
                break;
            }
            case 3: {   //Set (OR 1)
                bitset<8> bs(arg);
                *current |= bs;
                break;
            }
            case 4: {   //Reset (AND 0)
                bitset<8> bs(arg);
                *current &= bs;
                break;
            }
            case 5: {   //Flip (XOR 1);
                bitset<8> bs(arg);
                *current ^= bs;
                break;
            }
            default: {
                cerr << "Not a valid choice!";
            }
        }
    }


    void player::drawTabScreen() {

        int icons[] = {' ', 0xab, '<', 0x25a3, 0x2610, 'X', 'L'};

        drawHud();
        string listNum = "0: ";

        //Print out all of the channel bitsets + all available ops, one per row.

        for (int i = 0; i < 10; i++) {
            int key = (i + 1) % 10;   //Print out the channels labeled like keyboard keys, 0 at the end
            listNum[0] = '0' + key;
            theCanvas -> drawHud(1, 10 + 2 * i, UIFOREGROUND, listNum);
            theCanvas -> drawHud(4, 10 + 2 * i, UIFOREGROUND, channels[i].to_string());
            for (int j = 0; j < ops.size(); j++) {;
                if (ops[j].unlocked) {
                    theCanvas -> drawHud(j * 3 + 15, 10 + 2 * i, UIFOREGROUND, ops[j].display);
                }
                else {
                    theCanvas -> drawHud(j * 3 + 15, 10 + 2 * i, UIFOREGROUND, "?");
                }
            }
        }

        //Click on an op to apply it to the bitset.

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            int lineSelect = (mouse.y / theCanvas -> getFontSize() - 10) / 2;
            if (0 <= lineSelect && lineSelect <= 9) {
                int opSelect = (mouse.x / theCanvas -> getFontSize() - 15) / 3;
                if (0 <= opSelect && opSelect < ops.size()) {
                    for (int i = 0; i < ops[opSelect].operations.size(); i++) {
                        apply(&channels[lineSelect], ops[opSelect].operations[i], ops[opSelect].operands[i]);
                    }
                }
            }
        }
    }

