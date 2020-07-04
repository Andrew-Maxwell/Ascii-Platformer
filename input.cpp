#include "input.hpp"

/******************************************************************************/
//myGetKeyPressed: returns 0 unless exactly 1 key is pressed, then, returns that key
/******************************************************************************/

int myGetKeyPressed() {
    int toReturn = 0;
    for (int i = KEY_SPACE; i <= KEY_KB_MENU; i++) {
        if (IsKeyPressed(i)) {
            if (toReturn == 0) {
                toReturn = i;
            }
            else {
                return 0;
            }
        }
    }
    return toReturn;
}

int getAnyGamepadInput() {
    int toReturn = 0;
    int padID = 0;
    while (IsGamepadAvailable(padID)) {
        for (int i = 1; i < 17; i++) {
            if (IsGamepadButtonDown(padID, i)) {
                if (toReturn == 0) {
                    toReturn = i;
                }
                else {
                    return 0;
                }
            }
        }
        for (int i = 0; i < 4; i++) {
            if (GetGamepadAxisMovement(padID, i) != 0) {
                if (toReturn == 0) {
                    toReturn = -1 * i;
                }
                else {
                    return 0;
                }
            }
        }
        padID++;
    }
    return toReturn;
}


/******************************************************************************/
//input: Represents one channel of input (one key, gamepad button or axis)
/******************************************************************************/

    input::input() {
        id = -1;
    }

    input::input(bool newIsAxis, bool newPositive, int newID) {
        isAxis = newIsAxis;
        positive = newPositive;
        id = newID;
    }

    bool input::isDefined() {
        return id != -1;
    }

    string input::name(int device) {
        if (id == -1) {
            return "No Binding";
        }
        else if (device < 0) {
            return "Key " + keyName(id);
        }
        else if (isAxis) {
            return "Axis " + to_string(id) + (positive ? " +" : " -");
        }
        else {
            return "Button " + to_string(id);
        }
    }

    void input::update(int device) {
        lastValue = value;
        if (id == -1) {
            value = 0;
        }
        else if (device < 0) {   //keyboard
            if (IsKeyDown(id)) {
                value = 1;
            }
            else {
                value = 0;
            }
        }
        else if (isAxis) {
            float newValue = GetGamepadAxisMovement(device, id);
            if (positive && newValue > 0) {
                value = newValue;
            }
            else if (!positive && newValue < 0) {
                value = -1 * newValue;
            }
            else {
                value = 0;
            }
        }
        else {
            if (IsGamepadButtonDown(0, id)) {
                value = 1;
            }
            else {
                value = 0;
            }
        }
        firstCallPressed = firstCallReleased = true;
    }

    bool input::isDown() {
        return (value > 0);
    }

    bool input::isPressed() {
        return (value > 0 && lastValue == 0);
    }

    bool input::isPressedOnce() {
        if (isPressed() && firstCallPressed) {
            firstCallPressed = false;
            return true;
        }
        return false;
    }

    bool input::isReleased() {
        return (value == 0 && lastValue > 0);
    }

    bool input::isReleasedOnce() {
        if (isReleased() && firstCallReleased) {
            firstCallReleased = false;
            return true;
        }
        return false;
    }

    bool input::isUp() {
        return (value == 0);
    }

/******************************************************************************/
//inputMap
//maps keys/axes/buttons (inputs) to actions (e.g. jump, move, inventory, aim...)
/******************************************************************************/

    inputMap::inputMap(int newDevice) {
        device = newDevice;
        keyboard = (device == -1);
        if (keyboard) {   //keyboard
            useMouseAim = true;
            up = input(false, false, KEY_W);
            down = input(false, false, KEY_S);
            left = input(false, false, KEY_A);
            right = input(false, false, KEY_D);
            jump = input(false, false, KEY_W);
            interact = input(false, false, KEY_S);
            inventory = input(false, false, KEY_TAB);
            nextWeapon = input(false, false, KEY_E);
            previousWeapon = input(false, false, KEY_Q);
            explode = input(false, false, KEY_R);
            lastCode = input(false, false, KEY_LEFT_SHIFT);
            nextCode = input();
            previousCode = input();
            aimUp = input(false, false, KEY_I);
            aimDown = input(false, false, KEY_K);
            aimLeft = input(false, false, KEY_J);
            aimRight = input(false, false, KEY_L);
            fire = input(false, false, KEY_SPACE);
            for (int i = 0; i < 10; i++) {
                code[i] = input(false, false, KEY_ZERO + i);
            }
        }
        else {
            useMouseAim = false;
            up = input(true, false, GAMEPAD_AXIS_LEFT_Y);
            down = input(true, true, GAMEPAD_AXIS_LEFT_Y);
            left = input(true, false, GAMEPAD_AXIS_LEFT_X);
            right = input(true, true, GAMEPAD_AXIS_LEFT_X);
            jump = input(false, false, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
            interact = input(false, false, GAMEPAD_BUTTON_LEFT_TRIGGER_2);
            inventory = input(false, false, GAMEPAD_BUTTON_RIGHT_FACE_UP);
            nextWeapon = input(false, false, GAMEPAD_BUTTON_LEFT_FACE_RIGHT);
            previousWeapon = input(false, false, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
            explode = input(false, false, GAMEPAD_BUTTON_RIGHT_TRIGGER_2);
            lastCode = input(false, false, GAMEPAD_BUTTON_LEFT_TRIGGER_1);
            nextCode = input(false, false, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT);
            previousCode = input(false, false, GAMEPAD_BUTTON_RIGHT_FACE_LEFT);
            aimUp = input(true, false, GAMEPAD_AXIS_RIGHT_Y);
            aimDown = input(true, true, GAMEPAD_AXIS_RIGHT_Y);
            aimLeft = input(true, false, GAMEPAD_AXIS_RIGHT_X);
            aimRight = input(true, true, GAMEPAD_AXIS_RIGHT_X);
            fire = input(false, false, GAMEPAD_BUTTON_RIGHT_TRIGGER_1);
            for (int i = 0; i < 10; i++) {
                code[i] = input(false, false, 0);
            }
        }
    }

    void inputMap::update() {
        for (int i = 0; i < count(); i++) {
            operator[](i).update(device);
        }
    }

    input& inputMap::operator[](int index) {
        if (index >= 0 && index < 18) {
            switch(index) {
                case(0): return up;
                case(1): return down;
                case(2): return left;
                case(3): return right;
                case(4): return jump;
                case(5): return interact;
                case(6): return inventory;
                case(7): return nextWeapon;
                case(8): return previousWeapon;
                case(9): return explode;
                case(10): return lastCode;
                case(11): return nextCode;
                case(12): return previousCode;
                case(13): return aimUp;
                case(14): return aimDown;
                case(15): return aimLeft;
                case(16): return aimRight;
                case(17): return fire;
                default: return error;
            }
        }
        else if (index < 28) {
            return code[index - 17];
        }
        else {
            return error;
        }
    }

    string inputMap::name(int index) {
        return operator[](index).name(device);
    }

    void inputMap::change(int index) {
        input newInput;
        if (device == -1) {
            for (int i = 32; i < 349; i++) {
                if (IsKeyPressed(i)) {
                    if (newInput.isDefined()) { //Multiple keys pressed
                        return;
                    }
                    else {
                        newInput = input(false, false, i);
                    }
                }
            }
        }
        else {
            for (int i = 1; i <= 17; i++) {
                if (IsGamepadButtonPressed(device, i)) {
                    if (newInput.isDefined()) { //Multiple buttons pressed
                        return;
                    }
                    else {
                        newInput = input(false, false, i);
                    }
                }
            }
            for (int i = 0; i < 4; i++) {   //TODO: Change to GetGamepadAxisCount (need workaround for broken fifth axis on older gamepads)
                float value = GetGamepadAxisMovement(device, i);
                if (value != 0) {
                    if (newInput.isDefined()) { //Multiple inputs
                        return;
                    }
                    else {
                        newInput = input(true, (value > 0), i);
                    }
                }
            }
        }
        if (newInput.isDefined()) {
            operator[](index) = newInput;
        }
    }

    int inputMap::count() {
        return 28;
    }

    Vector2 inputMap::getAim() {
        Vector2 aim = (Vector2){aimRight.value - aimLeft.value, aimDown.value - aimUp.value};
        if (aim.x == 0 && aim.y == 0) {
            return (Vector2){0, -1};
        }
        else {
            return Vector2Normalize(aim);
        }
    }

string keyName(int key) {
    switch(key) {
        case 39: return "APOSTROPHE";
        case 44: return "COMMA";
        case 45: return "MINUS";
        case 46: return "PERIOD";
        case 47: return "SLASH";
        case 48: return "ZERO";
        case 49: return "ONE";
        case 50: return "TWO";
        case 51: return "THREE";
        case 52: return "FOUR";
        case 53: return "FIVE";
        case 54: return "SIX";
        case 55: return "SEVEN";
        case 56: return "EIGHT";
        case 57: return "NINE";
        case 59: return "SEMICOLON";
        case 61: return "EQUAL";
        case 65: return "A";
        case 66: return "B";
        case 67: return "C";
        case 68: return "D";
        case 69: return "E";
        case 70: return "F";
        case 71: return "G";
        case 72: return "H";
        case 73: return "I";
        case 74: return "J";
        case 75: return "K";
        case 76: return "L";
        case 77: return "M";
        case 78: return "N";
        case 79: return "O";
        case 80: return "P";
        case 81: return "Q";
        case 82: return "R";
        case 83: return "S";
        case 84: return "T";
        case 85: return "U";
        case 86: return "V";
        case 87: return "W";
        case 88: return "X";
        case 89: return "Y";
        case 90: return "Z";
        case 32: return "SPACE";
        case 256: return "ESCAPE";
        case 257: return "ENTER";
        case 258: return "TAB";
        case 259: return "BACKSPACE";
        case 260: return "INSERT";
        case 261: return "DELETE";
        case 262: return "RIGHT";
        case 263: return "LEFT";
        case 264: return "DOWN";
        case 265: return "UP";
        case 266: return "PAGE UP";
        case 267: return "PAGE DOWN";
        case 268: return "HOME";
        case 269: return "END";
        case 280: return "CAPS LOCK";
        case 281: return "SCROLL LOCK";
        case 282: return "NUM LOCK";
        case 283: return "PRINT SCREEN";
        case 284: return "PAUSE";
        case 290: return "F1";
        case 291: return "F2";
        case 292: return "F3";
        case 293: return "F4";
        case 294: return "F5";
        case 295: return "F6";
        case 296: return "F7";
        case 297: return "F8";
        case 298: return "F9";
        case 299: return "F10";
        case 300: return "F11";
        case 301: return "F12";
        case 340: return "LEFT SHIFT";
        case 341: return "LEFT CONTROL";
        case 342: return "LEFT ALT";
        case 343: return "LEFT SUPER";
        case 344: return "RIGHT SHIFT";
        case 345: return "RIGHT CONTROL";
        case 346: return "RIGHT ALT";
        case 347: return "RIGHT SUPER";
        case 348: return "KB MENU";
        case 91: return "LEFT BRACKET";
        case 92: return "BACKSLASH";
        case 93: return "RIGHT BRACKET";
        case 96: return "GRAVE";
        case 320: return "KEYPAD 0";
        case 321: return "KEYPAD 1";
        case 322: return "KEYPAD 2";
        case 323: return "KEYPAD 3";
        case 324: return "KEYPAD 4";
        case 325: return "KEYPAD 5";
        case 326: return "KEYPAD 6";
        case 327: return "KEYPAD 7";
        case 328: return "KEYPAD 8";
        case 329: return "KEYPAD 9";
        case 330: return "KEYPAD DECIMAL";
        case 331: return "KEYPAD DIVIDE";
        case 332: return "KEYPAD MULTIPLY";
        case 333: return "KEYPAD SUBTRACT";
        case 334: return "KEYPAD ADD";
        case 335: return "KEYPAD ENTER";
        case 336: return "KEYPAD EQUAL";
        default: return "??????";
    }
}
