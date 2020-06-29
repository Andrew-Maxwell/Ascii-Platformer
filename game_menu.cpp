#include "game_menu.hpp"
#include "screen.hpp"
#include "meta.hpp"


    string keyName(int key);

    void gameMenu::handleInput() {
        if (myGetKeyPressed()) {
            mouseMode = false;
        }
        else {
            Vector2 mouse = GetMousePosition();
            if (oldMouse.x != mouse.x || oldMouse.y != mouse.y || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                mouseMode = true;
            }
            oldMouse = mouse;
        }
        if (!mouseMode) {
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(keys.up) || (upDown > 18 && upDown % 6 == 0)) {
                select = (itemCount + select - 1) % itemCount;
            }
            if (IsKeyDown(KEY_UP) || IsKeyDown(keys.up)) {
                upDown++;
            }
            else {
                upDown = 0;
            }
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(keys.down) || (downDown > 18 && downDown % 6 == 0)) {
                select = (select + 1) % itemCount;
            }
            if (IsKeyDown(KEY_DOWN) || IsKeyDown(keys.down)) {
                downDown++;
            }
            else {
                downDown = 0;
            }
        }
    }

    bool gameMenu::button(int position, string text) {
        int hudFontSize = theScreen -> getHudFontSize();
        Vector2 mouse = GetMousePosition();
        int triangleInt[2] = {0x25ba, ' '};
        char* triangle = TextToUtf8(triangleInt, 2);
        text = triangle + text;
        free(triangle);
        int y = 2 * (position + 1);
//        int x = (getHudCols() - text.size()) / 2;
        int x = 1;
        if (mouseMode) {
            if (mouse.x > x * hudFontSize && mouse.x < (x + text.size()) * hudFontSize && mouse.y > y * hudFontSize && mouse.y < (y + 1) * hudFontSize) {
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    theScreen -> drawHudBarRight(x, y, DARKBACKGROUND, text.length());
                    theScreen -> drawHud(x, y, DARKFOREGROUND, text);
                }
                else {
                    theScreen -> drawHudBarRight(x, y, LIGHTBACKGROUND, text.length());
                    theScreen -> drawHud(x, y, LIGHTFOREGROUND, text);
                }
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    return true;
                }
            }
            else {
                theScreen -> drawHud(x, y, UIFOREGROUND, text);
            }
        }
        else {
            if (select == position) {
                if (IsKeyDown(KEY_ENTER) || IsKeyDown(keys.lastCode)) {
                    theScreen -> drawHudBarRight(x, y, DARKBACKGROUND, text.length());
                    theScreen -> drawHud(x, y, DARKFOREGROUND, text);
                }
                else {
                    theScreen -> drawHudBarRight(x, y, LIGHTBACKGROUND, text.length());
                    theScreen -> drawHud(x, y, LIGHTFOREGROUND, text);
                }
                if (IsKeyReleased(KEY_ENTER) || IsKeyReleased(keys.lastCode)) {
                    return true;
                }
            }
            else {
                theScreen -> drawHud(x, y, UIFOREGROUND, text);
            }
        }
        return false;
    }

    void gameMenu::pause(int& status) {
        itemCount = 4;
        while (status == pauseStatus) {
            handleInput();
            theScreen -> start(true);
            string pausedLabel = "Game Paused";
            string optionsLabel = "Game Options";
            string menuLabel = "Main Menu (lose unsaved progress)";
            string quitLabel = "Quit (lose unsaved progress)";
            string resumeLabel = "Resume";
            theScreen -> drawHudBarRight(0, 0, UIFOREGROUND, theScreen -> getHudCols() + 1);
            theScreen -> drawHud((theScreen -> getHudCols() - pausedLabel.size()) / 2, 0, LIGHTFOREGROUND, pausedLabel);
            if (button(0, optionsLabel)) {
                status = optionsStatus;
            }
            else if (button(1, menuLabel)) {
                status = menuStatus;
            }
            else if (button(2, quitLabel)) {
                status = quitStatus;
            }
            else if (button(3, resumeLabel)) {
                status = runStatus;
            }
            theScreen -> end();
        }
    }

    void gameMenu::main(int& status) {
        itemCount = 3;
        while (status == menuStatus) {
            handleInput();
            theScreen -> start(true);
            string title = "ASCII Platformer";
            string startLabel = "Load Game";
            string optionsLabel = "Options";
            string quitLabel = "Quit";
            theScreen -> drawHudBarRight(0, 0, UIFOREGROUND, theScreen -> getHudCols() + 1);
            theScreen -> drawHud((theScreen -> getHudCols() - title.size()) / 2, 0, LIGHTFOREGROUND, title);
            if (button(0, startLabel)) {
                status = deadStatus;
            }
            else if (button(1, optionsLabel)) {
                status = optionsStatus;
            }
            else if (button(2, quitLabel)) {
                status = quitStatus;
            }
            theScreen -> end();
        }
    }

    void gameMenu::options(int& status, configData& config) {
        itemCount = 6;
        while (status == optionsStatus) {
            handleInput();
            theScreen -> start(true);
            string title = "Options";
            theScreen -> drawHudBarRight(0, 0, UIFOREGROUND, theScreen -> getHudCols() + 1);
            theScreen -> drawHud((theScreen -> getHudCols() - title.size()) / 2, 0, LIGHTFOREGROUND, title);
            //Display settings
            string fullscreenLabel = "Toggle Fullscreen";
            if (button(0, fullscreenLabel)) {
                if (IsWindowFullscreen()) {
                    ToggleFullscreen();
                    SetWindowSize(1024, 768);
                    SetWindowMinSize(1024, 768);
                    config.setFullScreen(false);
                }
                else {
                    int width = GetMonitorWidth(0);
                    int height = GetMonitorHeight(0);
                    ToggleFullscreen();
                    SetWindowSize(width, height);
                    config.setFullScreen(true);
                }
                config.save();
            }
            string hudScaleLabel = "Tweak Interface Scale";
            if (button(1, hudScaleLabel)) {
                theScreen -> tweakHudScale();
                config.setHudFontSize(theScreen -> getHudFontSize());
                config.save();
            }
            string gameScaleLabel = "Tweak Game Scale";
            if (button(2, gameScaleLabel)) {
                theScreen -> tweakGameScale();
                config.setGameFontSize(theScreen -> getFontSize());
                config.save();
            }
            string gameScaleTest = "Game Scale Text";
            theScreen -> drawScaleTest(4 + gameScaleLabel.size(), 6, RED, gameScaleTest);
            //Keybind settings
            string keyLabel = "Key Bindings";
            if (button(3, keyLabel)) {
                status = keybindStatus;
                theScreen -> end();
                keyOptions(status, config);
            }
            string backLabel = "Back to Menu";
            if (button(4, backLabel)) {
                status = -1;    //Previous status will be restored in calling function
            }
            theScreen -> end();
        }
    }

    void gameMenu::keyOptions(int& status, configData& config) {
        itemCount = keys.count() + 2;
        int keyToChange = -1;
        while (status == keybindStatus) {
            if (keyToChange == -1) {
                handleInput();
            }
            theScreen -> start(true);
            string title = "Keyboard Bindings";
            theScreen -> drawHudBarRight(0, 0, UIFOREGROUND, theScreen -> getHudCols() + 1);
            theScreen -> drawHud((theScreen -> getHudCols() - title.size()) / 2, 0, LIGHTFOREGROUND, title);
            bool clickedNoButton = true;
            for (int i = 0; i < keys.count(); i++) {
                if (keyToChange == i) {
                    string pressKey = "PRESS KEY";
                    theScreen -> drawHud((theScreen -> getHudCols() - pressKey.size()) / 2, 2 * (i + 1), RED, pressKey);
                    int lastKey = myGetKeyPressed();
                    if (lastKey) {
                        if (lastKey != KEY_ESCAPE) {
                            keys[keyToChange] = lastKey;
                            config.setKeys(keys);
                            config.save();
                        }
                        keyToChange = -1;
                    }
                }
                else {
                    if (button(i, keys.name(i) + ": " + keyName(keys[i]))) {
                        keyToChange = i;
                        clickedNoButton = false;
                    }
                }
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && clickedNoButton) {
                    keyToChange = -1;
                }
            }
            string resetLabel = "Reset to Default";
            if (button(keys.count(), resetLabel)) {
                keys = keyMapping();
                config.setKeys(keys);
                config.save();
            }
            string backLabel = "Back to Options";
            if (button(keys.count() + 1, backLabel)) {
                status = optionsStatus;
            }
            theScreen -> end();
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
