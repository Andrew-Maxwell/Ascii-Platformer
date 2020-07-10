#include "gamemenu.hpp"


/*****************************************************************************/
//Pre-game menus
/*****************************************************************************/

    void gameMenu::main(int& status) {
        const int rows = 5, cols = 1;
        init(cols, rows);
        while (status == menuStatus) {
            handleInput();
            theScreen -> start(true);
            if (button("\xE2\x96\xBA Singleplayer", 0, 0)) {
                status = singleLoadStatus;
            }
            else if (button("\xE2\x96\xBA Co-op", 0, 1)) {
                status = coopLoadStatus;
            }
            else if (button("\xE2\x96\xBA Arena", 0, 2)) {
                
            }
            else if (button("\xE2\x96\xBA Options", 0, 3)) {
                status = optionsStatus;
                init(cols, rows);
            }
            else if (button( "\xE2\x96\xBA Quit", 0, 4)) {
                status = quitStatus;
            }
            drawTitle("ASCII Platformer");
            theScreen -> end();
        }
    }

    string gameMenu::chooseSave(int& status, listData& saves) {
        vector<string> options = saves.getList();
        init(1, options.size() + 2);
        string toReturn = "";
        string newName = "";
        bool enterName = false;
        inputBox nameBox;
        while (status == coopLoadStatus || status == singleLoadStatus) {
            handleInput();
            theScreen -> start(true);
            for (int i = 0; i < options.size(); i++) {
                if (button("\xE2\x96\xBA Load " + options[i], 0, i)) {
                    status = deadStatus;
                    toReturn = options[i] + ".save";
                }
            }
            if (button("\xE2\x96\xBA New game", 0, options.size())) {
                enterName = true;
                locked = true;
            }
            if (button("\xE2\x96\xBA Back to Menu", 0, options.size() + 1)) {
                status = menuStatus;
            }
            if (enterName) {
                nameBox.display("New save name: ", newName);
                if (nameBox.okay()) {
                    firstCallToReleased = false;
                    enterName = false;
                    locked = false;
                }
                if (nameBox.cancel()) {
                    newName = "";
                    firstCallToReleased = false;
                    enterName = false;
                    locked = false;
                }
            }
            else if (newName != "") {
                options.push_back(newName);
                newName = "";
                saves.setList(options);
                saves.save();
            }
            drawTitle ("Load Game");
            theScreen -> end();
        }
        return toReturn;
    }

/*****************************************************************************/
//In-game menus
/*****************************************************************************/

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
            if (goBack() || (in.inventory.isPressed() && !firstTick) || button("Back to Game", 0, 10, "", 1, 32)) {
                status = runStatus;
            }
            firstTick = false;
            drawTitle("Inventory");
            theScreen -> end();
        }
    }


/*****************************************************************************/
//Config menus
/*****************************************************************************/

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

    //Helper function

    Color getColor(int x, int y) {
        Color tint;
        tint.r = max(0, min(255, 16 * (y + max(min(x - 16, 16 - x), min(x - 64, 64 - x)))));
        tint.g = max(0, min(255, 16 * (y + max(min(x - 32, 32 - x), min(x + 16, -16 - x)))));
        tint.b = max(0, min(255, 16 * (y + max(min(x - 48, 48 - x), min(x - 0, 0 - x)))));
        tint.a = 255;
        return tint;
    }

    Color gameMenu::editColor(Color oldColor) {
        const int width = 48, height = 40;
        init(width, height + 1);
        RenderTexture palette = LoadRenderTexture(width, height);
        Color toReturn;
        bool returnNow = false;
        BeginTextureMode(palette);
        ClearBackground(RED);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                Color tint = getColor(j, height - i - 1);
                DrawRectangle(j, i, 1, 1, tint);
            }
        }
        EndTextureMode();
        while (!returnNow) {
            theScreen -> start(true);
            handleInput(false);
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    if (button(" ", j, i, "", j, i + 1)) {
                        toReturn = getColor(j, i);
                        returnNow = true;
                    }
                }
            }
            DrawTextureEx(palette.texture, (Vector2){0, theScreen -> getHudFontSize()}, 0, theScreen -> getHudFontSize(), WHITE);
            if (!mouseMode && ySelect != 40) {
                Color c1 = getColor(xSelect, ySelect);
                Color c2 = {255 - c1.r, 255 - c1.g, 255 - c1.b, 255};
                theScreen -> drawHud(xSelect, ySelect, c2, "\xE2\x98\x90");
            }
            if (ySelect == 40) {
                xSelect = 0;
            }
            if (button("Cancel", 0, 40, "", 1, 42)) {
                toReturn = oldColor;
                returnNow = true;
            }
            drawTitle("Pick a color... Any color...");
            theScreen -> end();
        }
        UnloadRenderTexture(palette);
        return toReturn;
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

