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
        ySelect = 0;
        scroll = 0;
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
            if (oldMouse.x != mouse.x || oldMouse.y != mouse.y || IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || GetMouseWheelMove()) {
                mouseMode = true;
            }
            oldMouse = mouse;
        }
        if (!mouseMode) {
            input dummy;
            handleKey (KEY_UP,    KEY_W, GAMEPAD_BUTTON_LEFT_FACE_UP, GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_RIGHT_Y, -1, dummy, upDown, ySelect, yCount, -1);
            handleKey (KEY_DOWN,  KEY_S, GAMEPAD_BUTTON_LEFT_FACE_DOWN, GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_RIGHT_Y, 1, dummy, downDown,  ySelect, yCount, 1);
            handleKey (KEY_LEFT,  KEY_A, GAMEPAD_BUTTON_LEFT_FACE_LEFT, GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_RIGHT_X, -1, dummy, leftDown,  xSelect, xCount, -1);
            handleKey (KEY_RIGHT, KEY_D, GAMEPAD_BUTTON_LEFT_FACE_RIGHT, GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_RIGHT_X, 1, dummy, rightDown, xSelect, xCount, 1);
        }
        tickNo--;
        scrollBar();
        firstCallToReleased = true;
    }

    void gameMenu::handleInput(inputMap& in) {
        if (myGetKeyPressed() || getAnyGamepadInput()) {
            mouseMode = false;
        }
        else {
            Vector2 mouse = GetMousePosition();
            if (oldMouse.x != mouse.x || oldMouse.y != mouse.y || IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || GetMouseWheelMove()) {
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
        scrollBar();
        firstCallToReleased = true;
    }

    bool gameMenu::button(string text, int buttonCol, int buttonRow, string inBrackets, int x, int y) {
        if (y == -1) {
            y = 2 * (buttonRow + 1) - scroll;
        }
        if (y <= 0) {
            return false;
        }
        int hudFontSize = theScreen -> getHudFontSize();
        Vector2 mouse = GetMousePosition();
        bool toReturn = false;
        if (mouseMode) {
            if (mouse.x > x * hudFontSize && mouse.x < (x + text.size() + (inBrackets == "" ? 0 : 3 + inBrackets.size())) * hudFontSize && mouse.y > y * hudFontSize && mouse.y < (y + 1) * hudFontSize) {
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    theScreen -> drawHudWithBackground(x, y, DARKFOREGROUND, DARKBACKGROUND, text);
                }
                else {
                    theScreen -> drawHudWithBackground(x, y, LIGHTFOREGROUND, LIGHTBACKGROUND, text);
                }
                if (inBrackets != "") {
                    theScreen -> drawHudWithBrackets(x + text.size() + 1, y, inBrackets, true);
                }
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && firstCallToReleased) {
                    firstCallToReleased = false;
                    toReturn = true;
                }
            }
            else {
                theScreen -> drawHud(x, y, UIFOREGROUND, text);
                if (inBrackets != "") {
                    theScreen -> drawHudWithBrackets(x + text.size() + 1, y, inBrackets, false);
                }
            }
        }
        else {
            if (xSelect == buttonCol && ySelect == buttonRow) {
                if (IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_SPACE) || onAnyGamepad(IsGamepadButtonDown, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || onAnyGamepad(IsGamepadButtonDown, GAMEPAD_BUTTON_LEFT_TRIGGER_1) || onAnyGamepad(IsGamepadButtonDown, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) {
                    theScreen -> drawHudWithBackground(x, y, DARKFOREGROUND, DARKBACKGROUND, text);
                }
                else {
                    theScreen -> drawHudWithBackground(x, y, LIGHTFOREGROUND, LIGHTBACKGROUND, text);
                }
                if (inBrackets != "") {
                    theScreen -> drawHudWithBrackets(x + text.size() + 1, y, inBrackets, true);
                }
                if (tickNo < 0 && (IsKeyReleased(KEY_ENTER) || IsKeyReleased(KEY_SPACE) || onAnyGamepad(IsGamepadButtonReleased, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || onAnyGamepad(IsGamepadButtonReleased, GAMEPAD_BUTTON_LEFT_TRIGGER_1) || onAnyGamepad(IsGamepadButtonReleased, GAMEPAD_BUTTON_RIGHT_TRIGGER_1))) {
                    toReturn = true;
                }
            }
            else {
                theScreen -> drawHud(x, y, UIFOREGROUND, text);
                if (inBrackets != "") {
                    theScreen -> drawHudWithBrackets(x + text.size() + 1, y, inBrackets, false);
                }
            }
        }
        if (tickNo > 0) {
            return false;
        }
        return toReturn;
    }

    void gameMenu::scrollBar() {
        int fontSize = theScreen -> getHudFontSize();
        int pageRows = 2 * yCount + 1;
        int screenRows = theScreen -> getHudRows() - 1;
        if (screenRows < pageRows) {
            //Determine scroll value
            float handleLength = (screenRows - 2) * (screenRows / float(pageRows));
            float handlePosition = 2 + scroll / float(pageRows) * (screenRows - 2);
            Vector2 newMouse = GetMousePosition();
            bool mouseOnBar = newMouse.x > fontSize * (theScreen -> getHudCols() - 1);
            bool mouseOnHandle = mouseOnBar && newMouse.y > handlePosition * fontSize && newMouse.y < (handlePosition + handleLength) * fontSize;
            if (mouseMode) {
                scroll -= 2 * GetMouseWheelMove();
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (mouseOnHandle) {
                        scrollClicked = true;
                        scrollMouse = GetMousePosition();
                        oldScroll = scroll;     //Save where the handle was when we started
                    }
                    else if (mouseOnBar) {  //Jump to position clicked on bar
                        scroll = (((newMouse.y / float(fontSize)) - 2) / float(theScreen -> getHudRows() - 3)) * pageRows;
                        scrollMouse = GetMousePosition();
                        oldScroll = scroll;
                    }
                }
                //Move to new scroll relative to old scroll
                else if (scrollClicked && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    scroll = oldScroll + (newMouse.y - scrollMouse.y) / float(fontSize * theScreen -> getHudRows()) * pageRows;
                }
                else {
                    scrollClicked = false;
                }
                //While scrolling mouse mode, move keyboard mode selection to stay on-screen
                if (2 * ySelect + 1 - scroll > screenRows) {
                    ySelect = (scroll - 1 + screenRows) / 2;
                }
                if (2 * ySelect + 3 - scroll < 0) {
                    ySelect = (scroll + 1) / 2;
                }
            }
            //While in keyboard mode, scroll to keep up with keyboard mode selection.
            else {
                if (2 * ySelect + 3 - scroll > screenRows) {
                    scroll = 2 * ySelect + 3 - screenRows;
                }
                if (2 * ySelect + 1 - scroll < 0) {
                    scroll = 2 * ySelect + 1;
                }
            }
            scroll = min(scroll, pageRows - screenRows);
            scroll = max(0, scroll);
            if (scroll % 2 == 1) {
                scroll--;
            }

            //Draw bar background
            theScreen -> drawHud(theScreen -> getHudCols() - 1, 1, UIFOREGROUND, "\xE2\x95\xA5");
            for (int i = 2; i < theScreen -> getHudRows() - 1; i++) {
                theScreen -> drawHud(theScreen -> getHudCols() - 1, i, UIFOREGROUND,  "\xE2\x95\x91");
            }
            theScreen -> drawHud(theScreen -> getHudCols() - 1, theScreen -> getHudRows() - 1, UIFOREGROUND, "\xE2\x95\xA8");
            //Draw bar handle
            if (mouseMode && (scrollClicked || mouseOnHandle)) {
                if (scrollClicked) {
                    theScreen -> drawHudBarDown(theScreen -> getHudCols() - 1, handlePosition, DARKFOREGROUND, handleLength);
                }
                else if (mouseOnHandle) {
                    theScreen -> drawHudBarDown(theScreen -> getHudCols() - 1, handlePosition, LIGHTFOREGROUND, handleLength);
                }
            }
            else {
                theScreen -> drawHudBarDown(theScreen -> getHudCols() - 1, handlePosition, UIFOREGROUND, handleLength);
            }
        }
    }

    void gameMenu::inputBox(string title, string& input) {
        const int width = 48, height = 7, x = (theScreen -> getHudCols() - width) / 2, y = (theScreen -> getHudRows() - height) / 2;
        char nextChar = GetKeyPressed();
        while (nextChar && input.size() < width - 4) {
            input += nextChar;
            nextChar = GetKeyPressed();
        }
        drawHudBox(x, y, width, height, UIFOREGROUND);
        theScreen -> drawHud((theScreen -> getHudCols() - title.size()) / 2, y + 1, UIBACKGROUND, title);
        theScreen -> drawHudWithBrackets((
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

    Color gameMenu::editColor(Color oldColor) {
        return oldColor;
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
            //Draw available ops as buttons
            string listNum = "0: ";
            for (int i = 0; i < 10; i++) {
                listNum[0] = '0' + i;
                theScreen -> drawHud(1, 10 + 2 * i, UIFOREGROUND, listNum);
                theScreen -> drawHud(4, 10 + 2 * i, o.channelColors[i], o.channels[i].to_string());
                for (int j = 0; j < o.ops.size(); j++) {;
                    if (o.ops[j].unlocked) {
                        if (button(o.ops[j].display, j, i, "", j * 3 + 15, 10 + 2 * i)) {
                            for (int k = 0; k < o.ops[j].operations.size(); k++) {
                                apply(&o.channels[i], o.ops[j].operations[k], o.ops[j].operands[k]);
                            }
                            o.channelColors[i].r = 255 - (o.channels[i] & bitset<8>(192)).to_ulong();
                            o.channelColors[i].g = 255 - ((o.channels[i] & bitset<8>(56)).to_ulong() << 2);
                            o.channelColors[i].b = 255 - ((o.channels[i] & bitset<8>(7)).to_ulong() << 5);
                        }
                    }
                    else {
                        button("?", j, i, "", j * 3 + 15, 10 + 2 * i);
                    }
                }
            }
            if (goBack() || (in.inventory.isPressed() && !firstTick) || button("Back to Game", 0, 10, "", 1, 32)) { status = runStatus;
            }
            firstTick = false;
            drawTitle("Inventory");
            theScreen -> end();
        }
    }

    void gameMenu::pause(int& status) {
        init(1, 4);
        while (status == pauseStatus) {
            handleInput();
            theScreen -> start(true);
            drawTitle("Game Paused");
            if (button("\xE2\x96\xBA Game Options", 0, 0)) {
                status = optionsStatus;
            }
            else if (button("\xE2\x96\xBA Main Menu (lose unsaved progress)", 0, 1)) {
                status = menuStatus;
            }
            else if (button("\xE2\x96\xBA Quit (lose unsaved progress)", 0, 2)) {
                status = quitStatus;
            }
            else if (goBack() || button("\xE2\x96\xBA Resume", 0, 3)) {
                status = runStatus;
            }
            theScreen -> end();
        }
    }

    void gameMenu::main(int& status) {
        const int rows = 3, cols = 1;
        init(cols, rows);
        while (status == menuStatus) {
            handleInput();
            theScreen -> start(true);
            if (button("\xE2\x96\xBA Load Game", 0, 0)) {
                status = deadStatus;
            }
            else if (button("\xE2\x96\xBA Options", 0, 1)) {
                status = optionsStatus;
                init(cols, rows);
            }
            else if (button( "\xE2\x96\xBA Quit", 0, 2)) {
                status = quitStatus;
            }
            drawTitle("ASCII Platformer");
            theScreen -> end();
        }
    }

    string gameMenu::chooseSave(listData saves) {
        vector<string> options = saves.getList();
        init(1, options.size() + 2);
        string toReturn = "";
        bool enterName = false;
        while (status == loadStatus) {
            handleInput();
            theScreen -> start(true);
            for (int i = 0; i < options.size(); i++) {
                if (button("\xE2\x96\xBA Load " + options[i], 0, i)) {
                    status = deadStatus;
                    toReturn = options[i];
                }
            }
            if (button("\xE2\x96\xBA New game", 0, options.size())) {
                enterName = true;
            }
            if (enterName && (goBack() || IsKeyPressed(KEY_ENTER) || onAnyGamepad(IsGamepadButtonPressed, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || onAnyGamepad(IsGamepadButtonPressed, GAMEPAD_BUTTON_LEFT_TRIGGER_1) || onAnyGamepad(IsGamepadButtonPressed, GAMEPAD_BUTTON_RIGHT_TRIGGER_1))) {
                enterName = false;
            }
            if (button("\xE2\x96\xBA Back to Menu", 0, options.size() + 1)) {
                status = menuStatus;
            }
            if (enterName) {
                                                            
            }
            drawTitle ("Load Game");
        }
        return toReturn;
    }


    void gameMenu::options(int& status, configData& configFile) {
        const int rows = 12, cols = 2;
        vector<conflict> conflicts = configFile.getConflicts();
        init(cols, rows + conflicts.size());
        while (status == optionsStatus) {
            handleInput();
            if (ySelect % yCount < 3 || ySelect % yCount >= 11 + conflicts.size()) {
                xSelect = 0;
            }
            theScreen -> start(true);
            //Display settings
            if (button("Toggle fullscreen", 0, 0)) {
                if (IsWindowFullscreen()) {
                    ToggleFullscreen();
                    SetWindowSize(MINWINDOWCOLS * theScreen -> getHudFontSize(), MINWINDOWROWS * theScreen -> getHudFontSize());
                    SetWindowMinSize(MINWINDOWCOLS * theScreen -> getHudFontSize(), MINWINDOWROWS * theScreen -> getHudFontSize());
                    configFile.setFullScreen(false);
                }
                else {
                    int width = GetMonitorWidth(0);
                    int height = GetMonitorHeight(0);
                    ToggleFullscreen();
                    SetWindowSize(width, height);
                    configFile.setFullScreen(true);
                }
                configFile.save();
            }
            if (button("Tweak interface scale", 0, 1)) {
                theScreen -> tweakHudScale();
                configFile.setHudFontSize(theScreen -> getHudFontSize());
                configFile.save();
            }
            string gameScaleLabel = "Tweak game scale";
            if (button(gameScaleLabel, 0, 2)) {
                theScreen -> tweakGameScale();
                configFile.setGameFontSize(theScreen -> getFontSize());
                configFile.save();
            }
            string gameScaleTest = "Game Scale Text";
            theScreen -> drawScaleTest(4 + gameScaleLabel.size(), 6 - scroll, RED, gameScaleTest);
            //Player config settings
            for (int i = 0; i < 8; i++) {
                theScreen -> drawHud(1, 2 * (4 + i) - scroll, configFile.getConfigColor(i), "Player " + to_string(i));
                if (button("\xE2\x96\xBA Edit Input", 0, 3 + i, "", 10)) {
                    playerConfig c = configFile.getConfig(i);
                    status = inputOptionsStatus;
                    editInputMap(status, c.in);
                    configFile.setConfig(i, c);
                    configFile.save();
                    conflicts = configFile.getConflicts();
                    init(cols, rows + conflicts.size());
                }
                if (button("\xE2\x96\xBA Edit Color", 1, 3 + i, "", 24)) {
                    playerConfig c = configFile.getConfig(i);
                    c.tint = editColor(c.tint);
                    configFile.setConfig(i, c);
                    configFile.save();
                    conflicts = configFile.getConflicts();
                    init(cols, rows + conflicts.size());
                }
            }
            for (int i = 0; i < conflicts.size(); i++) {
                theScreen -> drawHud(1, 2 * (12 + i) - scroll, RED, "Conflict:            and           " + conflicts[i].error);
                if (button("\xE2\x96\xBA Player " + to_string(conflicts[i].player1), 0, 11 + i, "", 11)) {
                    playerConfig c = configFile.getConfig(conflicts[i].player1);
                    status = inputOptionsStatus;
                    editInputMap(status, c.in);
                    configFile.setConfig(conflicts[i].player1, c);
                    configFile.save();
                    conflicts = configFile.getConflicts();
                    init(cols, rows + conflicts.size());
                }
                else if (button("\xE2\x96\xBA Player " + to_string(conflicts[i].player2), 1, 11 + i, "", 26)) {
                    playerConfig c = configFile.getConfig(conflicts[i].player2);
                    status = inputOptionsStatus;
                    editInputMap(status, c.in);
                    configFile.setConfig(conflicts[i].player2, c);
                    configFile.save();
                    conflicts = configFile.getConflicts();
                    init(cols, rows + conflicts.size());
                }
            }
            if (button("Reset all defaults", 0, 11 + conflicts.size())) {
                string fileName = configFile.getFileName();
                configFile = configData(fileName, true);
                conflicts.clear();
                configFile.save();
            }
            if (goBack() || button("\xE2\x96\xBA Back to Menu", 0, 12 + conflicts.size())) {
                status = -1;    //Previous status will be restored in calling function
            }
            drawTitle("Options");
            theScreen -> end();
        }
    }

    void gameMenu::editInputMap(int& status, inputMap& toReplace) {
        init(1, toReplace.count() + 4);
        int selectionToChange = -1;
        //Keep track of separate values for gamepad and keyboard
        inputMap keyboardMap(-1);
        inputMap gamepadMap(0);
        inputMap* toEdit;
        if (toReplace.keyboard) {
            keyboardMap = toReplace;
            toEdit = &keyboardMap;
        }
        else {
            gamepadMap = toReplace;
            toEdit = &gamepadMap;
        }
        while (status == inputOptionsStatus) {
            if (selectionToChange == -1) {
                handleInput();
            }
            theScreen -> start(true);
            bool clickedNoButton = true;
            for (int i = 0; i < toEdit -> count(); i++) {
                if (selectionToChange == i) {
                    string prompt;
                    if (toEdit -> keyboard) {
                        prompt = "PRESS KEY";
                    }
                    else {
                        prompt = "PRESS BUTTON OR MOVE JOYSTICK";
                    }
                    theScreen -> drawHud(1, 2 * (i + 1), RED, prompt);
                    if (IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE)) {
                        (*toEdit)[i] = input(); //unbound input
                        selectionToChange = -1;
                    }
                    else if (goBack()) {
                        selectionToChange = -1;
                    }
                    else {
                        if (toEdit -> change(i)) {
                            selectionToChange = -1;
                        }
                    }
                }
                else {
                    if (button(toEdit -> name(i), 0, i, (*toEdit)[i].name(toEdit -> device))) {
                        selectionToChange = i;
                        clickedNoButton = false;
                    }
                }
                if (selectionToChange > -1 && IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && firstCallToReleased && clickedNoButton) {
                    selectionToChange = -1;
                    firstCallToReleased = false;
                }
            }
            if (toEdit -> keyboard) {
                if (button("Device type", 0, toEdit -> count(), "keyboard")) {
                    toEdit = &gamepadMap;
                }
                else if (button("Use mouse aim", 0, toEdit -> count() + 1, (toEdit -> useMouseAim ? "true" : "false"))) {
                    toEdit -> useMouseAim = !toEdit -> useMouseAim;
                }
            }
            else {
                if (button("Device type", 0, toEdit -> count(), "gamepad")) {
                    toEdit = &keyboardMap;
                }
                else {
                    if (selectionToChange == toEdit -> count()) {
                        theScreen -> drawHud(1, 2 * (toEdit -> count() + 2 - scroll), RED, "PRESS GAMEPAD BUTTON OR MOVE JOYSTICK");
                        int newDevice = selectGamepad();
                        if (newDevice >= 0) {
                            toEdit -> device = newDevice;
                            selectionToChange = -1;
                            firstCallToReleased = false;
                        }
                        if (goBack()) {
                            selectionToChange = -1;
                        }
                    }
                    else if (button("Gamepad number", 0, toEdit -> count() + 1, to_string(toEdit -> device))) {
                        selectionToChange = toEdit -> count();
                    }
                }
            }
            if (button("Reset to default", 0, toEdit -> count() + 2)) {
                int device = toEdit -> device;
                *toEdit = inputMap(device);
            }
            if (button("\xE2\x96\xBA Back to Options", 0, toEdit -> count() + 3)) {
                toReplace = *toEdit;
                status = optionsStatus;
            }
            drawTitle("Edit Input Map");
            theScreen -> end();
        }
    }

