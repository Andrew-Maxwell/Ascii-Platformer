#include "player.hpp"

    void player::printHud() {

        //Print gun info

        int rowCount = 0;
        for (int i = 0; i < guns.size(); i++) {
            if (guns[i].unlocked) {
                if (tickCounter < guns[i].lastFired + guns[i].cooldown || guns[i].ammo < 1) {
                    theScreen -> drawHud(1, ++rowCount + 3, guns[i].tintFaded, guns[i].display);
                    if (guns[i].ammo >= 0) {
                        theScreen -> drawHud(2, rowCount + 3, guns[i].tintFaded, to_string(guns[i].ammo));
                    }
                }
                else {
                    theScreen -> drawHud(1, ++rowCount + 3, guns[i].tint, guns[i].display);
                    if (guns[i].ammo >= 0) {
                        theScreen -> drawHud(2, rowCount + 3, guns[i].tint, to_string(guns[i].ammo));
                    }
                }
            }
        }

        //Health background bar

        theScreen -> drawHudBarRight(1, 1, BLACK, maxHealth / 8.0f);

        //Health bar

        if (hurtTimer > 0 && (hurtTimer / 4) % 2 == 0) {    //If hurt, flash
            theScreen -> drawHudBarRight(1, 1, HURTCOLOR, health / 8.0f);
        }
        else {
            theScreen -> drawHudBarRight(1, 1, HEALTHCOLOR, health / 8.0f);
        }

        //Air bar

        if (air < maxAir) {
            theScreen -> drawHudBarRight(1, 2, AIRCOLOR, air / 64.0);
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

        printHud();

        string listNum = "0: ";

        //Print out all of the channel bitsets + all available ops, one per row.

        for (int i = 0; i < 10; i++) {
            listNum[0] = '0' + i;
            theScreen -> drawHud(1, 10 + 2 * i, UIFOREGROUND, listNum);
            theScreen -> drawHud(4, 10 + 2 * i, channelColors[i], channels[i].to_string());
            for (int j = 0; j < ops.size(); j++) {;
                if (ops[j].unlocked) {
                    theScreen -> drawHud(j * 3 + 15, 10 + 2 * i, UIFOREGROUND, ops[j].display);
                }
                else {
                    theScreen -> drawHud(j * 3 + 15, 10 + 2 * i, UIFOREGROUND, "?");
                }
            }
        }

        //Click on an op to apply it to the bitset.

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            int lineSelect = (mouse.y / theScreen -> getFontSize() - 10) / 2;
            if (0 <= lineSelect && lineSelect <= 9) {
                int opSelect = (mouse.x / theScreen -> getFontSize() - 15) / 3;
                if (0 <= opSelect && opSelect < ops.size() && ops[opSelect].unlocked) {
                    for (int i = 0; i < ops[opSelect].operations.size(); i++) {
                        apply(&channels[lineSelect], ops[opSelect].operations[i], ops[opSelect].operands[i]);
                        channelColors[lineSelect].r = 255 - (channels[lineSelect] & bitset<8>(192)).to_ulong();
                        channelColors[lineSelect].g = 255 - ((channels[lineSelect] & bitset<8>(56)).to_ulong() << 2);
                        channelColors[lineSelect].b = 255 - ((channels[lineSelect] & bitset<8>(7)).to_ulong() << 5);
                    }
                }
            }
        }
    }
