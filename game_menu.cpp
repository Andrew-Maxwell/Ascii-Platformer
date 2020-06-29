#include "game_menu.hpp"
#include "screen.hpp"
#include "meta.hpp"

    struct menuInput {

        Vector2 oldMouse = GetMousePosition();
        int select = 0;
        bool mouseMode = false;
        int itemCount;

        menuInput(int newItemCount) {
            itemCount = newItemCount;
        }

        void handle() {
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
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(keys.up)) {
                    select = (itemCount + select - 1) % itemCount;
                }
                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(keys.down)) {
                    select = (select + 1) % itemCount;
                }
            }
        }

    };

    void pauseMenu(int& status) {
        menuInput in(4);
        while (status == pause) {
            in.handle();
            theScreen -> start(true);
            string pausedLabel = "Game Paused";
            string optionsLabel = "Game Options";
            string menuLabel = "Main Menu (lose unsaved progress)";
            string quitLabel = "Quit (lose unsaved progress)";
            string resumeLabel = "Resume";
            theScreen -> drawHudBarRight(0, 0, UIFOREGROUND, theScreen -> getHudCols() + 1);
            theScreen -> drawHud((theScreen -> getHudCols() - pausedLabel.size()) / 2, 0, LIGHTFOREGROUND, pausedLabel);
            if (theScreen -> button(0, optionsLabel, in.mouseMode, in.select)) {
                status = options;
            }
            else if (theScreen -> button(1, menuLabel, in.mouseMode, in.select)) {
                status = breakMenu;
            }
            else if (theScreen -> button(2, quitLabel, in.mouseMode, in.select)) {
                status = breakQuit;
            }
            else if (theScreen -> button(3, resumeLabel, in.mouseMode, in.select)) {
                status = run;
            }
            theScreen -> end();
        }
    }

    void mainMenu(int& status) {
        menuInput in(3);
        while (status == breakMenu) {
            in.handle();
            theScreen -> start(true);
            string title = "ASCII Platformer";
            string startLabel = "Load Game";
            string optionsLabel = "Options";
            string quitLabel = "Quit";
            theScreen -> drawHudBarRight(0, 0, UIFOREGROUND, theScreen -> getHudCols() + 1);
            theScreen -> drawHud((theScreen -> getHudCols() - title.size()) / 2, 0, LIGHTFOREGROUND, title);
            if (theScreen -> button(0, startLabel, in.mouseMode, in.select)) {
                status = breakDead;
            }
            else if (theScreen -> button(1, optionsLabel, in.mouseMode, in.select)) {
                status = options;
            }
            else if (theScreen -> button(2, quitLabel, in.mouseMode, in.select)) {
                status = breakQuit;
            }
            theScreen -> end();
        }
    }

    void optionsMenu(int& status, configData& config) {
        menuInput in(6);
        while (status == options) {
            in.handle();
            theScreen -> start(true);
            string title = "Options";
            theScreen -> drawHudBarRight(0, 0, UIFOREGROUND, theScreen -> getHudCols() + 1);
            theScreen -> drawHud((theScreen -> getHudCols() - title.size()) / 2, 0, LIGHTFOREGROUND, title);
            //Display settings
            string fullscreenLabel = "Toggle Fullscreen";
            if (theScreen -> button(0, fullscreenLabel, in.mouseMode, in.select)) {
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
            if (theScreen -> button(1, hudScaleLabel, in.mouseMode, in.select)) {
                theScreen -> tweakHudScale();
                config.setHudFontSize(theScreen -> getHudFontSize());
                config.save();
            }
            string gameScaleLabel = "Tweak Game Scale";
            if (theScreen -> button(2, gameScaleLabel, in.mouseMode, in.select)) {
                theScreen -> tweakGameScale();
                config.setGameFontSize(theScreen -> getFontSize());
                config.save();
            }
            string gameScaleTest = "Game Scale Text";
            theScreen -> drawScaleTest(3, 8, RED, gameScaleTest);
            //Keybind settings
            string keyLabel = "Key Bindings";
            if (theScreen -> button(4, keyLabel, in.mouseMode, in.select)) {
                status = keyOptions;
                theScreen -> end();
                keyOptionsMenu(status, config);
            }
            string backLabel = "Back to Menu";
            if (theScreen -> button(5, backLabel, in.mouseMode, in.select)) {
                status = -1;    //Previous status will be restored in calling function
            }
            theScreen -> end();
        }
    }

    string keyName(int key);

    void keyOptionsMenu(int& status, configData& config) {
        int selected = -1;
        menuInput in(keys.count() + 2);
        while (status == keyOptions) {
            in.handle();
            theScreen -> start(true);
            string title = "Keyboard Bindings";
            theScreen -> drawHudBarRight(0, 0, UIFOREGROUND, theScreen -> getHudCols() + 1);
            theScreen -> drawHud((theScreen -> getHudCols() - title.size()) / 2, 0, LIGHTFOREGROUND, title);
            bool clickedNoButton = true;
            for (int i = 0; i < keys.count(); i++) {
                if (selected == i) {
                    string pressKey = "PRESS KEY";
                    theScreen -> drawHud((theScreen -> getHudCols() - pressKey.size()) / 2, 2 * (i + 1), RED, pressKey);
                    int lastKey = myGetKeyPressed();
                    if (lastKey) {
                        if (lastKey != KEY_ESCAPE) {
                            keys[selected] = lastKey;
                            config.setKeys(keys);
                            config.save();
                        }
                        selected = -1;
                    }
                }
                else {
                    if (theScreen -> button(i, keys.name(i) + ": " + keyName(keys[i]), in.mouseMode, in.select)) {
                        selected = i;
                        clickedNoButton = false;
                    }
                }
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && clickedNoButton) {
                    selected = -1;
                }
            }
            string resetLabel = "Reset to Default";
            if (theScreen -> button(keys.count(), resetLabel, in.mouseMode, in.select)) {
                keys = keyMapping();
                config.setKeys(keys);
            }
            string backLabel = "Back to Options";
            if (theScreen -> button(keys.count() + 1, backLabel, in.mouseMode, in.select)) {
                status = options;
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
