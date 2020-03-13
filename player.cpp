#include "player.hpp"

/******************************************************************************/
//saveData
//Used as a hack for saving player data.
/******************************************************************************/

struct saveData {
    float x, y;
    int health, maxHealth;
    int air, maxAir;
    bool gunUnlocked[16] = {false};
    int gunAmmos[16] {0};
    int gunMaxAmmos[16] = {0};
    int gunDisplays[16];
    int ops[16][4] = {{0}};
    int args[16][4] = {{0}};
    int opCount = 0;
    bitset<8> channels[10];
    char nextRoom[64] = "test.txt";
    bool pickUpsCollected[512] = {false};
};


/******************************************************************************/
//player
//an entity controlled by the user
//Also responsible for storing save data and drawing inventory screen
/******************************************************************************/

//Constructor

    player::player(  float newX, float newY, Color newTint, float newSizeFactor, string newNextRoom) :
                            entity(newX, newY, newTint, newSizeFactor),
                            realPhysicalEntity(newX, newY, newTint, newSizeFactor, 1.0, 0.0, 0.0)
    {
        shouldChangeRooms = false;
        nextRoom = newNextRoom;
        yMomentum = 0;
        elasticity = 0;
        type = PLAYERTYPE;
        health = maxHealth = 8;
        air = maxAir = 600;

        for (int i = 0; i < 10; i++) {
            channels[i] = bitset<8>("00000000");
        }

        for (int i = 0; i < 16; i++) {
            gunDisplayChars[i] = TextToUtf8(&gunDisplays[i], 1);
        }
    }

    player::~player() {
        for (int i = 0; i < 16; i++) {
            free(gunDisplayChars[i]);
        }
    }

    bool player::save(string fileName) {
        saveData s;
        s.x = x;
        s.y = y;
        s.health = health;
        s.maxHealth = maxHealth;
        s.air = air;
        s.maxAir = maxAir;
        copy(gunUnlocked, gunUnlocked + 16, s.gunUnlocked);
        copy(gunAmmos, gunAmmos + 16, s.gunAmmos);
        copy(gunMaxAmmos, gunMaxAmmos + 16, s.gunMaxAmmos);
        copy(&ops[0][0], &ops[0][0] + 64, &s.ops[0][0]);
        copy(&args[0][0], &args[0][0] + 64, &s.args[0][0]);
        s.opCount = opCount;
        copy(pickUpsCollected, pickUpsCollected + 512, s.pickUpsCollected);
        copy(channels, channels + 10, s.channels);
        copy(nextRoom.begin(), nextRoom.end(), s.nextRoom);

        fstream saveOut;
        saveOut.open(fileName, ios::out|ios::binary);
        if (!saveOut) {
            cerr << "Error writing save file.\n";
            return false;
        }
        else {
            saveOut.write((char*)&s, sizeof(s));
            saveOut.close();
            return true;
        }
    }

    bool player::load(string fileName) {
        saveData s;
        fstream saveIn;
        saveIn.open(fileName, ios::in|ios::binary);
        if (!saveIn) {
            cerr << "No save file found.\n";
            return false;
        }
        else {
            saveIn.read((char*)&s, sizeof(s));
            saveIn.close();
            x = s.x;
            y = s.y;
            health = s.health;
            maxHealth = s.maxHealth;
            air = s.air;
            maxAir = s.maxAir;
            copy(s.gunUnlocked, s.gunUnlocked + 16, gunUnlocked);
            copy(s.gunAmmos, s.gunAmmos + 16, gunAmmos);
            copy(s.gunMaxAmmos, s.gunMaxAmmos + 16, gunMaxAmmos);
            copy(&s.ops[0][0], &s.ops[0][0] + 64, &ops[0][0]);
            copy(&s.args[0][0], &s.args[0][0] + 64, &args[0][0]);
            copy(s.pickUpsCollected, s.pickUpsCollected + 512, pickUpsCollected);
            opCount = s.opCount;
            copy(s.channels, s.channels + 10, channels);
            nextRoom = s.nextRoom;
            return true;
        }

    }

//Accessors

    bool player::isCollected(int pickUpID) {
        if (pickUpID >= 0 && pickUpID < 512) {
            return pickUpsCollected[pickUpID];
        }
        return false;
    }

    void player::setColor(Color newTint) {
        tint = newTint;
    }

    void player::setSizeFactor(float newSizeFactor) {
        sizeFactor = newSizeFactor;
    }

    float player::getSizeFactor() {
        return sizeFactor;
    }

    void player::setEList(entityList* newEList) {
        eList = newEList;
    }

//Collision functions

    bool player::doesCollide(float otherX, float otherY, int type) {
        lastCollisionType = type;
        return (otherX >= x && otherX <= x + 1 && otherY >= y && otherY <= y + 1) || realPhysicalEntity::doesCollide(otherX, otherY, type);
    }

    collision player::getCollision(float otherX, float otherY, int otherType) {
        if (lastCollisionType == WATERTYPE) {
            return realPhysicalEntity::getCollision(otherX, otherY, otherType);
        }
        collision c(1);
        return c;
    }

    bool player::stopColliding() {
        return false;
    }

//Tick functions

    void player::tickSet(collider& col) {

        //Explosions

        if (IsKeyPressed(KEY_R)) {
            explode (col, eList, 60, x, y, tint, sizeFactor, 1, 0, 600, 0.5);
        }

        //Movement in air

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

          	//Jomping

            if (IsKeyPressed(KEY_W)) {
                if (col.isSolid(x + (1 - width) / 2, y + 1) || col.isSolid(x + (1 + width) / 2, y + 1))  {
                    yMomentum -= JUMPSPEED;
                }
            }
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
            col.setChannel(channels[1].to_ulong());
        }
        if (IsKeyDown(KEY_TWO)) {
            col.setChannel(channels[2].to_ulong());
        }
        if (IsKeyDown(KEY_THREE)) {
            col.setChannel(channels[3].to_ulong());
        }
        if (IsKeyDown(KEY_FOUR)) {
            col.setChannel(channels[4].to_ulong());
        }
        if (IsKeyDown(KEY_FIVE)) {
            col.setChannel(channels[5].to_ulong());
        }
        if (IsKeyDown(KEY_SIX)) {
            col.setChannel(channels[6].to_ulong());
        }
        if (IsKeyDown(KEY_SEVEN)) {
            col.setChannel(channels[7].to_ulong());
        }
        if (IsKeyDown(KEY_EIGHT)) {
            col.setChannel(channels[8].to_ulong());
        }
        if (IsKeyDown(KEY_NINE)) {
            col.setChannel(channels[9].to_ulong());
        }
        if (IsKeyDown(KEY_ZERO)) {
            col.setChannel(channels[0].to_ulong());
        }
        //Boollets

        //Switching guns

        if (IsKeyPressed(KEY_Q)) {
            for (int i = 0; i < 16; i++) {
                gunSelect--;
                if (gunUnlocked[gunSelect % 16]) {
                    break;
                }
            }
        }
        if (IsKeyPressed(KEY_E)) {
            for (int i = 0; i < 16; i++) {
                gunSelect++;
                if (gunUnlocked[gunSelect % 16]) {
                    break;
                }
            }
        }

        //Cooldown

        for (int i = 0; i < 16; i++) {
            gunCoolDowns[i]--;
        }

        //Shootin'

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && gunUnlocked[gunSelect]) {
            gunSelect = gunSelect % 16;
            if (gunAmmos[gunSelect] > 0 && gunCoolDowns[gunSelect] < 0) {
                Vector2 aim = Vector2Subtract(GetMousePosition(), positionOnScreen);
                bullet* b;
                switch(gunSelect) {
                    case 0:
                        aim = Vector2Scale(Vector2Normalize(aim), 0.5);
                        b = new bullet(x + 3 * aim.x, y + 3 * aim.y, tint, sizeFactor, eList, aim.x, aim.y, 0, 120, 0, 10, GRAVITY, 0, -10, -0.5, 20);
                        gunCoolDowns[0] = 60;
                        gunAmmos[0]--;
                        break;
                    default:
                        cerr << "Fired bullet with invalid gun\n";
                        break;
                }
                b -> tickSet(col);
                b -> tickSet(col);
                eList -> addEntity(b);
                col.addCollideable(b);
            }
        }

        //Spikes, falling, and death
        
        hurtTimer--;
        if (y > col.getRows() + 25) {
            health = 0;
        }
        if (health <= 0) {
            won = -1;
        }

        //spikes
        
        int spikeDamage = col.getPlayerDamage((int)(x + 0.5), (int)(y + 0.5));
        if (hurtTimer < 0 && spikeDamage) {
            health += spikeDamage;
            hurtTimer = 60;
            Vector2 newMomentum = Vector2Scale(Vector2Negate(Vector2Normalize({xMomentum, yMomentum})), 0.7);
            xMomentum = newMomentum.x;
            yMomentum = newMomentum.y / 3;
            damageIndicator(eList, spikeDamage, x, y, HURTCOLOR, sizeFactor);
        }
        realPhysicalEntity::tickSet(col);;
    }

    void player::tickGet(collider& col) {
        list<collision>::iterator colIter = collisions.begin();
        while (colIter != collisions.end()) {
            switch(colIter -> type) {
                case ENDINGGATETYPE:
                    won = 1;
                    colIter = collisions.erase(colIter);
                    break;
                case DOORTYPE:
                    x = colIter -> xVal;
                    y = colIter -> yVal;
                    nextRoom = colIter -> message;
                    shouldChangeRooms = true;
                    colIter = collisions.erase(colIter);
                    break;
                case SAVEPOINTTYPE: {
                    save("save");
                    colIter = collisions.erase(colIter);
                    break;
                }
                case BULLETTYPE:
                    if ((hurtTimer < 0)) {
                        yMomentum += colIter -> yVal * 0.3;
                        xMomentum += colIter -> xVal * 0.3;
                        health += (colIter -> damage);
                        hurtTimer = 60;
                        damageIndicator(eList, colIter -> damage, x, y, HURTCOLOR, sizeFactor);
                    }
                    colIter = collisions.erase(colIter);
                    break;
                case GUNPICKUPTYPE: {
                    int gunID = colIter -> damage;
                    gunUnlocked[gunID] = true;  // damage is actually the gunID
                    switch(gunID) {
                        case 0:
                            gunAmmos[gunID] = 3;
                            gunMaxAmmos[gunID] = 6;
                            break;
                        default:
                            cerr << "Error: Gun Pickup contains invalid gunID.";
                            break;
                    }
                    if (0 < colIter -> yVal && colIter -> yVal < 512) {
                        pickUpsCollected[(int)(colIter -> yVal)] = true;
                    }
                    colIter = collisions.erase(colIter);
                    break;
                    }
                case AMMOPICKUPTYPE:
                    gunAmmos[colIter -> damage] = min(gunMaxAmmos[colIter -> damage], (int)(gunAmmos[colIter -> damage] + colIter -> xVal));
                    if (0 < colIter -> yVal && colIter -> yVal < 512) {
                        pickUpsCollected[(int)(colIter -> yVal)] = true;
                    }
                    colIter = collisions.erase(colIter);
                    break;
                case HEALTHPICKUPTYPE:
                    damageIndicator(eList, colIter -> damage, x, y, HEALTHCOLOR, sizeFactor);
                    health = min(maxHealth, health + colIter -> damage);
                    if (0 < colIter -> yVal && colIter -> yVal < 512) {
                        pickUpsCollected[(int)(colIter -> yVal)] = true;
                    }
                    colIter = collisions.erase(colIter);
                    break;
                case MAXHEALTHPICKUPTYPE:
                    health += (colIter -> damage);
                    damageIndicator(eList, colIter -> damage, x, y, HEALTHCOLOR, sizeFactor);
                    maxHealth += (colIter -> damage);
                    if (0 < colIter -> yVal && colIter -> yVal < 512) {
                        pickUpsCollected[(int)(colIter -> yVal)] = true;
                    }
                    colIter = collisions.erase(colIter);
                    break;
                case AIRPICKUPTYPE:
                    air = min(maxAir, air + colIter -> damage);
                    damageIndicator(eList, colIter -> damage, x, y, AIRCOLOR, sizeFactor);
                    if (0 < colIter -> yVal && colIter -> yVal < 512) {
                        pickUpsCollected[(int)(colIter -> yVal)] = true;
                    }
                    colIter = collisions.erase(colIter);
                    break;
                case MAXAIRPICKUPTYPE:
                    air += (colIter -> damage);
                    damageIndicator(eList, colIter -> damage, x, y, AIRCOLOR, sizeFactor);
                    maxAir += (colIter -> damage);
                    if (0 < colIter -> yVal && colIter -> yVal < 512) {
                        pickUpsCollected[(int)(colIter -> yVal)] = true;
                    }
                    colIter = collisions.erase(colIter);
                    break;
                case OPPICKUPTYPE: {
                    string message = colIter -> message;
                    for (int i = 0; i < 8; i+= 2) {
                        ops[opCount][i / 2] = message[i];
                        args[opCount][i / 2] = message[i + 1];
                    }
                    opCount++;
                    if (0 < colIter -> yVal && colIter -> yVal < 512) {
                        pickUpsCollected[(int)(colIter -> yVal)] = true;
                    }
                    colIter = collisions.erase(colIter);
                    break;
                }
                default:
                    colIter++;
            }
        }
        realPhysicalEntity::tickGet(col);
    }

    bool player::finalize() {
        return false;
    }

    void player::print(float cameraX, float cameraY, Font displayFont) {

        positionOnScreen = (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor };   //Used for mouse aiming
        if (hurtTimer > 0 && (hurtTimer / 4) % 2 == 0) {    //Flash if recently taken damage
            myDrawText(displayFont, "@", positionOnScreen, FONTSIZE * sizeFactor, 1, HURTCOLOR);
        }
        else {
            myDrawText(displayFont, "@", positionOnScreen, FONTSIZE * sizeFactor, 1, tint);
        }
        drawHUD(displayFont);
    }

    void player::drawHUD(Font displayFont) {

        //Print gun info

        int rowCount = 0;
        for (int i = 0; i < 16; i++) {
            if (gunUnlocked[i]) {
                if (gunCoolDowns[i] > 0 || gunAmmos[i] < 1) {
                    myDrawText(displayFont, gunDisplayChars[i], { FONTSIZE, (++rowCount + 3) * FONTSIZE}, FONTSIZE, 0, gunColorsFaded[i]);
                    myDrawText(displayFont, to_string(gunAmmos[i]).c_str(), { 2 * FONTSIZE, (rowCount + 3) * FONTSIZE}, FONTSIZE, 0, gunColorsFaded[i]);
                }
                else {
                    myDrawText(displayFont, gunDisplayChars[i], { FONTSIZE, (++rowCount + 3) * FONTSIZE}, FONTSIZE, 0, gunColors[i]);
                    myDrawText(displayFont, to_string(gunAmmos[i]).c_str(), { 2 * FONTSIZE, (rowCount + 3) * FONTSIZE}, FONTSIZE, 0, gunColors[i]);
                }
            }
        }

        //Health background bar

        drawHudBarRight(displayFont, FONTSIZE, FONTSIZE, UIBACKGROUND, maxHealth);

        //Health bar

        if (hurtTimer > 0 && (hurtTimer / 4) % 2 == 0) {    //If hurt, flash
            drawHudBarRight(displayFont, FONTSIZE, FONTSIZE, HURTCOLOR, health);
        }
        else {
            drawHudBarRight(displayFont, FONTSIZE, FONTSIZE, HEALTHCOLOR, health);
        }

        //Air bar

        if (air < maxAir) {
            drawHudBarRight(displayFont, FONTSIZE, FONTSIZE * 2, AIRCOLOR, air / 7);
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


    void player::drawTabScreen(Font displayFont) {

        int keys[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
        int icons[] = {' ', 0xab, '<', 0x25a3, 0x2610, 'X', 'L'};

        drawHUD(displayFont);
        string listNum = "0: ";

        //Print out all of the channel bitsets + all available ops, one per row.

        for (int i = 0; i < 10; i++) {
            listNum[0] = '0' + keys[i];
            myDrawText(displayFont, listNum.c_str(), { FONTSIZE, FONTSIZE * (10 + 2 * i) }, FONTSIZE, 0, UIFOREGROUND);
            for (int j = 0; j < 8; j++) {
                if (channels[keys[i]][j]) {
                    myDrawText(displayFont, "1", { FONTSIZE * (j + 4), FONTSIZE * (10 + 2 * i) }, FONTSIZE, 0, UIFOREGROUND);
                }
                else {
                    myDrawText(displayFont, "0", { FONTSIZE * (j + 4), FONTSIZE * (10 + 2 * i) }, FONTSIZE, 0, UIFOREGROUND);
                }
            }
            for (int k = 0; k < 16; k++) {
                char* temp = TextToUtf8(&icons[ops[k][0]], 1);
                myDrawText(displayFont, temp, { FONTSIZE * (k * 3 + 15), FONTSIZE * (10 + 2 * i) }, FONTSIZE, 0, UIFOREGROUND);
                free(temp);
                if (ops[k][1] != 0) {
                    myDrawText(displayFont, "+", {FONTSIZE * (k * 3 + 16), FONTSIZE * (10 + 2 * i) }, FONTSIZE, 0, UIFOREGROUND);
                }
            }
        }

        //Click on an op to apply it to the bitset.

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            int lineSelect = (mouse.y / FONTSIZE - 10) / 2;
            if (0 <= lineSelect && lineSelect <= 9) {
                int opSelect = (mouse.x / FONTSIZE - 15) / 3;
                if (0 <= opSelect && opSelect < 16) {
                    for (int i = 0; i < 4; i++) {
                        apply(&channels[keys[lineSelect]], ops[opSelect][i], args[opSelect][i]);
                    }
                }
            }
        }
    }

