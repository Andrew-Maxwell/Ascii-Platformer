#include "game_menu.hpp"
#include "screen.hpp"
#include "meta.hpp"

    //Misc helper functions

    void drawTitle(string title) {
        theScreen -> drawHudBarRight(0, 0, UIFOREGROUND, theScreen -> getHudCols() + 1);
        theScreen -> drawHud((theScreen -> getHudCols() - title.size()) / 2, 0, LIGHTFOREGROUND, title);
    }

    bool onAnyGamepad(bool (*gamepadProperty)(int, int), int button) {
        int padID = 0;
        while (IsGamepadAvailable(padID)) {
            if ((*gamepadProperty)(padID, button)) {
                return true;
            }
            padID++;
        }
        return false;
    }

    bool axisOnAnyGamepad(int axis1, int invert1, int axis2 = 0, int invert2 = 0) {
        int padID = 0;
        while (IsGamepadAvailable(padID)) {
            if (GetGamepadAxisMovement(padID, axis1) * invert1 > 0.7) {
                return true;
            }
            if (GetGamepadAxisMovement(padID, axis2) * invert2 > 0.7) {
                return true;
            }
            padID++;
        }
        return false;
    }

    void handleKey (int keyOne, int keyTwo, int button1, int axis1, int axis2, int invertAxes, input in, int& keyDown, int& select, int itemCount, int direction) {
        if (IsKeyPressed(keyOne) || IsKeyPressed(keyTwo) || onAnyGamepad(IsGamepadButtonPressed, button1) || (axisOnAnyGamepad(axis1, invertAxes, axis2, invertAxes) && keyDown == 0) || in.isPressed() || (keyDown > 18 && keyDown % 6 == 0)) {
            select = (itemCount + select + direction) % itemCount;
        }
        if (IsKeyDown(keyOne) || IsKeyDown(keyTwo) || onAnyGamepad(IsGamepadButtonPressed, button1) || axisOnAnyGamepad(axis1, invertAxes, axis2, invertAxes) || in.isDown()) {
            keyDown++;
        }
        else {
            keyDown = 0;
        }
    }

/*****************************************************************************/
//gameMenu functions
/*****************************************************************************/

    void gameMenu::init(int newXCount, int newYCount) {
        tickNo = 2;
        xCount = newXCount;
        yCount = newYCount;
    }

    bool gameMenu::goBack() {
        return (tickNo < 0 && (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_DELETE) || onAnyGamepad(IsGamepadButtonPressed, GAMEPAD_BUTTON_MIDDLE_LEFT) || onAnyGamepad(IsGamepadButtonPressed, GAMEPAD_BUTTON_MIDDLE) || onAnyGamepad(IsGamepadButtonPressed, GAMEPAD_BUTTON_MIDDLE_RIGHT)));
    }

    void gameMenu::handleInput() {
        if (myGetKeyPressed() || getAnyGamepadInput()) {
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
            handleKey (KEY_UP,    KEY_W, GAMEPAD_BUTTON_LEFT_FACE_UP, GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_RIGHT_Y, -1, input(), upDown, ySelect, yCount, -1);
            handleKey (KEY_DOWN,  KEY_S, GAMEPAD_BUTTON_LEFT_FACE_DOWN, GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_RIGHT_Y, 1, input(), downDown,  ySelect, yCount, 1);
            handleKey (KEY_LEFT,  KEY_A, GAMEPAD_BUTTON_LEFT_FACE_LEFT, GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_RIGHT_X, -1, input(), leftDown,  xSelect, xCount, -1);
            handleKey (KEY_RIGHT, KEY_D, GAMEPAD_BUTTON_LEFT_FACE_RIGHT, GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_RIGHT_X, 1, input(), rightDown, xSelect, xCount, 1);
        }
        tickNo--;
    }

    void gameMenu::handleInput(inputMap& in) {
        if (myGetKeyPressed() || getAnyGamepadInput()) {
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
            handleKey (KEY_UP,    KEY_W, GAMEPAD_BUTTON_LEFT_FACE_UP, GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_RIGHT_Y, -1, in.up, upDown, ySelect, yCount, -1);
            handleKey (KEY_DOWN,  KEY_S, GAMEPAD_BUTTON_LEFT_FACE_DOWN, GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_RIGHT_Y, 1, in.down, downDown,  ySelect, yCount, 1);
            handleKey (KEY_LEFT,  KEY_A, GAMEPAD_BUTTON_LEFT_FACE_LEFT, GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_RIGHT_X, -1, in.left, leftDown,  xSelect, xCount, -1);
            handleKey (KEY_RIGHT, KEY_D, GAMEPAD_BUTTON_LEFT_FACE_RIGHT, GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_RIGHT_X, 1, in.right, rightDown, xSelect, xCount, 1);
        }
        tickNo--;
    }

    bool gameMenu::button(string text, int buttonCol, int buttonRow, int x, int y) {
        if (y == -1) {
            y = 2 * (buttonRow + 1);
        }
        int hudFontSize = theScreen -> getHudFontSize();
        Vector2 mouse = GetMousePosition();
        bool toReturn = false;
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
                    toReturn = true;
                }
            }
            else {
                theScreen -> drawHud(x, y, UIFOREGROUND, text);
            }
        }
        else {
            if (xSelect == buttonCol && ySelect == buttonRow) {
                if (IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_SPACE) || onAnyGamepad(IsGamepadButtonDown, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || onAnyGamepad(IsGamepadButtonDown, GAMEPAD_BUTTON_LEFT_TRIGGER_1) || onAnyGamepad(IsGamepadButtonDown, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) {
                    theScreen -> drawHudBarRight(x, y, DARKBACKGROUND, text.length());
                    theScreen -> drawHud(x, y, DARKFOREGROUND, text);
                }
                else {
                    theScreen -> drawHudBarRight(x, y, LIGHTBACKGROUND, text.length());
                    theScreen -> drawHud(x, y, LIGHTFOREGROUND, text);
                }
                if (IsKeyReleased(KEY_ENTER) || IsKeyReleased(KEY_SPACE) || onAnyGamepad(IsGamepadButtonReleased, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || onAnyGamepad(IsGamepadButtonReleased, GAMEPAD_BUTTON_LEFT_TRIGGER_1) || onAnyGamepad(IsGamepadButtonReleased, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) {
                    toReturn = true;
                }
            }
            else {
                theScreen -> drawHud(x, y, UIFOREGROUND, text);
            }
        }
        x += (text.size() + 1);
        y += 2;
        return toReturn;
    }

    void apply(bitset<8>* current, int op, int arg) {
        switch(op) {
            case 0: {   //do nothing
                break;
            }
            case 1: {   //Rotate
                bitset<8> toReturn;
                for (int i = 0; i < 8; i++) {
                    toReturn[i] = (*current)[(i + arg)&7];
                }
                *current = toReturn;
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
            case 6: {   //Load
                bitset<8> toReturn(arg);
                *current = toReturn;
                break;
            }
            default: {
                cerr << "Not a valid choice!" << op << endl;
            }
        }
//        cout << op << " " << arg << current -> to_string() << endl;
    }

    void gameMenu::inventory(int& status, outfit& o, inputMap& in) {
        init(o.ops.size(), 11);
        bool firstTick = true;  //Prevents entering and exiting screen in first tick (because tab is pressed)
        while (status == inventoryStatus) {
            handleInput(in);
            if (ySelect == 10) {
                xSelect = 0;
            }
            theScreen -> start(true);

            string title = "Inventory";
            drawTitle(title);

            //Draw available ops as buttons
            string listNum = "0: ";
            for (int i = 0; i < 10; i++) {
                listNum[0] = '0' + i;
                theScreen -> drawHud(1, 10 + 2 * i, UIFOREGROUND, listNum);
                theScreen -> drawHud(4, 10 + 2 * i, o.channelColors[i], o.channels[i].to_string());
                for (int j = 0; j < o.ops.size(); j++) {;
                    if (o.ops[j].unlocked) {
                        if (button(o.ops[j].display, j, i, j * 3 + 15, 10 + 2 * i)) {
                            for (int k = 0; k < o.ops[j].operations.size(); k++) {
                                apply(&o.channels[i], o.ops[j].operations[k], o.ops[j].operands[k]);
                            }
                            o.channelColors[i].r = 255 - (o.channels[i] & bitset<8>(192)).to_ulong();
                            o.channelColors[i].g = 255 - ((o.channels[i] & bitset<8>(56)).to_ulong() << 2);
                            o.channelColors[i].b = 255 - ((o.channels[i] & bitset<8>(7)).to_ulong() << 5);
                        }
                    }
                    else {
                        button("?", j, i, j * 3 + 15, 10 + 2 * i);
                    }
                }
            }
            string exitLabel = "Back to Game";
            if (goBack() || (in.inventory.isPressed() && !firstTick) || button(exitLabel, 0, 10, 1, 32)) {
                status = runStatus;
            }
            firstTick = false;
            theScreen -> end();
        }
    }

    void gameMenu::pause(int& status) {
        init(1, 4);
        while (status == pauseStatus) {
            handleInput();
            theScreen -> start(true);
            string pausedLabel = "Game Paused";
            drawTitle(pausedLabel);
            string optionsLabel = "\xE2\x96\xBA Game Options";
            string menuLabel = "\xE2\x96\xBA Main Menu (lose unsaved progress)";
            string quitLabel = "\xE2\x96\xBA Quit (lose unsaved progress)";
            string resumeLabel = "\xE2\x96\xBA Resume";
            if (button(optionsLabel, 0, 0)) {
                status = optionsStatus;
            }
            else if (button(menuLabel, 0, 1)) {
                status = menuStatus;
            }
            else if (button(quitLabel, 0, 2)) {
                status = quitStatus;
            }
            else if (goBack() || button(resumeLabel, 0, 3)) {
                status = runStatus;
            }
            theScreen -> end();
        }
    }

    void gameMenu::main(int& status) {
        init(1, 3);
        while (status == menuStatus) {
            handleInput();
            theScreen -> start(true);
            string title = "ASCII Platformer";
            string startLabel = "\xE2\x96\xBA Load Game";
            string optionsLabel = "\xE2\x96\xBA Options";
            string quitLabel = "\xE2\x96\xBA Quit";
            drawTitle(title);
            if (button(startLabel, 0, 0)) {
                status = deadStatus;
            }
            else if (button(optionsLabel, 0, 1)) {
                status = optionsStatus;
            }
            else if (button(quitLabel, 0, 2)) {
                status = quitStatus;
            }
            theScreen -> end();
        }
    }

    void gameMenu::options(int& status, configData& config) {
        init(1, 6);
        while (status == optionsStatus) {
            handleInput();
            theScreen -> start(true);
            string title = "Options";
            drawTitle(title);
            //Display settings
            string fullscreenLabel = "Toggle Fullscreen";
            if (button(fullscreenLabel, 0, 0)) {
                if (IsWindowFullscreen()) {
                    ToggleFullscreen();
                    SetWindowSize(MINWINDOWCOLS * theScreen -> getHudFontSize(), MINWINDOWROWS * theScreen -> getHudFontSize());
                    SetWindowMinSize(MINWINDOWCOLS * theScreen -> getHudFontSize(), MINWINDOWROWS * theScreen -> getHudFontSize());
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
            if (button(hudScaleLabel, 0, 1)) {
                theScreen -> tweakHudScale();
                config.setHudFontSize(theScreen -> getHudFontSize());
                config.save();
            }
            string gameScaleLabel = "Tweak Game Scale";
            if (button(gameScaleLabel, 0, 2)) {
                theScreen -> tweakGameScale();
                config.setGameFontSize(theScreen -> getFontSize());
                config.save();
            }
            string gameScaleTest = "Game Scale Text";
            theScreen -> drawScaleTest(4 + gameScaleLabel.size(), 6, RED, gameScaleTest);
            //input settings
            string inputOptionsLabel = "\xE2\x96\xBA Input Settings";
            if (button(inputOptionsLabel, 0, 3)) {
                status = inputOptionsStatus;
                theScreen -> end();
                inputOptions(status, config);
            }
            string backLabel = "\xE2\x96\xBA Back to Menu";
            if (goBack() || button(backLabel, 0, 4)) {
                status = -1;    //Previous status will be restored in calling function
            }
            theScreen -> end();
        }
    }

    void gameMenu::inputOptions(int& status, configData& config) {
/*        yCount = inputMap.count() + 2;
        xCount = 1;
        int inputToChange = -1;
        while (status == inputOptionsStatus) {
            if (inputToChange == -1) {
                handleInput();
            }
            theScreen -> start(true);
            string title = "Input Options";
            drawTitle(title);
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
                    if (button(keys.name(i) + ": " + keyName(keys[i]), 0, i)) {
                        keyToChange = i;
                        clickedNoButton = false;
                    }
                }
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && clickedNoButton) {
                    keyToChange = -1;
                }
            }
            string resetLabel = "Reset to Default";
            if (button(resetLabel, 0, keys.count())) {
                keys = keyMapping();
                config.setKeys(keys);
                config.save();
            }
            string backLabel = "\xE2\x96\xBA Back to Options";
            if (button(backLabel, 0, keys.count() + 1)) {
                status = optionsStatus;
            }
            theScreen -> end();
        }*/
    }

