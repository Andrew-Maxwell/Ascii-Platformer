#include "gamemenu.hpp"


/*****************************************************************************/
//Pre-game menus
/*****************************************************************************/

    void gameMenu::main(int& status, int& mode) {
        const int rows = 5, cols = 1;
        init(cols, rows);
        while (status == menuStatus) {
            handleInput();
            theScreen -> start(true);
            if (button("\xE2\x96\xBA Singleplayer", 0, 0)) {
                status = loadStatus;
                mode = singlePlayerMode;
            }
            else if (button("\xE2\x96\xBA Co-op", 0, 1)) {
                status = loadStatus;
                mode = coopMode;
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

    string gameMenu::chooseSave(int& status, int mode) {
        listData saves("");
        if (mode == singlePlayerMode) {
            saves.load("singleplayersaves.json");
        }
        else {
            saves.load("coopsaves.json");
        }
        list<string> options = saves.getList();
        init(1, options.size() + 2);
        string toReturn = "";
        string newName = "";
        bool enterName = false;
        inputBox nameBox;
        while (status == loadStatus) {
            handleInput();
            theScreen -> start(true);
            int buttonPos = 0;
            auto iter = options.begin();
            while (iter != options.end()) {
                if (button("\xE2\x96\xBA Load " + *iter, 0, buttonPos++)) {
                    status = deadStatus;
                    string atIter = *iter;
                    toReturn = atIter + (mode == coopMode ? "-coopsave.json" : "-singleplayersave.json");
                    //Move most recently selected save to front of list
                    iter = options.erase(iter);
                    options.push_front(atIter);
                    saves.setList(options);
                    saves.save();
                }
                else {
                    iter++;
                }
            }
            if (button("New game", 0, options.size())) {
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
                options.push_front(newName);
                newName = "";
                saves.setList(options);
                saves.save();
            }
            drawTitle ("Load Game");
            theScreen -> end();
        }
        firstCallToReleased = false;
        return toReturn;
    }

    vector<playerConfig> gameMenu::readyRoom(int& status, saveData& save, bool loadedSave, gameLevelData& level, configData& config) {
        status = readyRoomStatus;
        int maxPlayersForLevel = min(level.getPlayerCount(), 8);
        vector<player*> tempPlayers;
        vector<playerConfig> playerConfigs;
        outfit defaultOutfit = level.getOutfit("defaultOutfit");
        bool conflict = false;
        bool tooFew = false;
        //Initialize
        for (int i = 0; i < maxPlayersForLevel; i++) {
            tempPlayers.push_back(new player(0, 0, HEALTHCOLOR, 1));
            if (loadedSave && save.hasOutfit(defaultOutfit.name + to_string(i))) {
                outfit newOutfit = save.getOutfit(defaultOutfit.name + to_string(i));
                newOutfit.merge(defaultOutfit);
                tempPlayers[i] -> setOutfit(newOutfit);
            }
            else {
                tempPlayers[i] -> setOutfit(defaultOutfit);
            }
            tempPlayers[i] -> playerNo = i;
        }
        for (int i = 0; i < MAXPLAYERS; i++) {
            playerConfigs.push_back(config.getPlayerConfig(i));
        }
        //If the screen is wide enough, add a "reset" button for each player
        if (theScreen -> getHudCols() >= 8 * (maxPlayersForLevel + 1)) {
            init (1, 3 + MAXPLAYERS);
        }
        else {
            init (1, 3);
        }
        while (status == readyRoomStatus) {
            theScreen -> start(true);
            handleInput();
            for (int i = 0; i < maxPlayersForLevel; i++) {
                tempPlayers[i] -> printHud();
            }
            for (int i = 0; i < MAXPLAYERS; i++) {
                playerConfigs[i].in.update();
                if (playerConfigs[i].in.left.isPressed()) {
                    playerConfigs[i].playerNo = (playerConfigs[i].playerNo - 1 + maxPlayersForLevel) % maxPlayersForLevel;
                }
                if (playerConfigs[i].in.right.isPressed()) {
                    playerConfigs[i].playerNo = (playerConfigs[i].playerNo + 1) % maxPlayersForLevel;
                }
                if (yCount > MAXPLAYERS && button("Reset", 0, i, "", 8 * maxPlayersForLevel, 10 + 2 * i)) {
                    playerConfigs[i].playerNo = -1;
                }
                if (playerConfigs[i].playerNo == -1) {
                    theScreen -> drawHud(1, 10 + 2 * i, playerConfigs[i].tint, "[Not selected]");
                }
                else {
                    theScreen -> drawHud(playerConfigs[i].playerNo * 8 + 4, 10 + 2 * i, playerConfigs[i].tint, "\xE2\x98\xBB");
                }
            }
            if (button("Reset all", 0, yCount - 3, "", 1, 10 + MAXPLAYERS * 2)) {
                for (int i = 0; i < 8; i++) {
                    playerConfigs[i].playerNo = -1;
                }
            }
            if (button("\xE2\x96\xBA Continue", 0, yCount - 2, "", 1, 12 + MAXPLAYERS * 2)) {
                conflict = false;
                tooFew = true;
                for (int i = 0; i < MAXPLAYERS; i++) {
                    if (playerConfigs[i].playerNo != -1) {
                        tooFew = false;
                        for (int j = i + 1; j < MAXPLAYERS; j++) {
                            if (playerConfigs[i].playerNo == playerConfigs[j].playerNo) {
                                conflict = true;
                            }
                        }
                    }
                }
                if (!conflict && !tooFew) {
                    status = deadStatus;    //Keep loading the level
                }
            }
            if (button("\xE2\x96\xBA Exit to main menu", 0, yCount - 1, "", 1, 14 + MAXPLAYERS * 2)) {
                playerConfigs.clear();
                status = menuStatus;    //Return to menu
            }
            if (conflict) {
                theScreen -> drawHud(1, 16 + MAXPLAYERS * 2, RED, "Error: players must select different characters");
            }
            else if (tooFew) {
                theScreen -> drawHud(1, 16 + MAXPLAYERS * 2, RED, "Error: need at least 1 character selected");
            }
            else {
                theScreen -> drawHud(1, 16 + MAXPLAYERS * 2, UIFOREGROUND, "Press left or right to select an avatar");
            }
            drawTitle("Ready Room");
            theScreen -> end();
        }
        auto configIter = playerConfigs.begin();
        while (configIter != playerConfigs.end()) {
            if (configIter -> playerNo == -1) {
                configIter = playerConfigs.erase(configIter);
            }
            else {
                configIter++;
            }
        }
        return playerConfigs;
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
        firstCallToReleased = false;
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
        pageRows = 35 + 2 * (o.interceptedCodes.size() / (theScreen -> getHudCols() / 9));
        while (status == inventoryStatus) {
            in.update();
            handleInput(in);
            if (ySelect == 10) {
                xSelect = 0;
            }
            theScreen -> start(true);
            //Draw available ops as buttons
            string listNum = "0: ";
            for (int i = 0; i < 10; i++) {
                int reOrder = (i + 1) % 10;   //print in keyboard order 1234567890
                listNum[0] = '0' + reOrder;
                theScreen -> drawHud(1, 10 + 2 * i - scroll, UIFOREGROUND, listNum);
                theScreen -> drawHud(4, 10 + 2 * i - scroll, o.channelColors[reOrder], o.channels[reOrder].to_string());
                for (int j = 0; j < o.ops.size(); j++) {
                    if (o.ops[j].unlocked) {
                        if (button(o.ops[j].display, j, i, "", j * 3 + 15, 10 + 2 * i - scroll)) {
                            for (int k = 0; k < o.ops[j].operations.size(); k++) {
                                apply(&o.channels[reOrder], o.ops[j].operations[k], o.ops[j].operands[k]);
                            }
                            o.channelColors[reOrder].r = 255 - (o.channels[reOrder] & bitset<8>(192)).to_ulong();
                            o.channelColors[reOrder].g = 255 - ((o.channels[reOrder] & bitset<8>(56)).to_ulong() << 2);
                            o.channelColors[reOrder].b = 255 - ((o.channels[reOrder] & bitset<8>(7)).to_ulong() << 5);
                        }
                    }
                    else {
                        button("?", j, i, "", j * 3 + 15, 10 + 2 * i);
                    }
                }
            }
            if (goBack() || (in.inventory.isPressedOnce()) || button("Back to Game", 0, 10, "", 1, 32 - scroll)) {
                status = runStatus;
            }
            theScreen -> drawHud(1, 34 - scroll, UIFOREGROUND, "Intercepted codes: ");
            int chanIter = 0;
            for (uint8_t channel : o.interceptedCodes) {
                Color tint = {255, 255, 255, 255};
                tint.r = 255 - (channel & 192);
                tint.g = 255 - ((channel & 56) << 2);
                tint.b = 255 - ((channel & 7) << 5);
                tint.a = 255;
                theScreen -> drawHud(1 + (chanIter % (theScreen -> getHudCols() / 9)) * 9, 36 + chanIter / (theScreen -> getHudCols() / 9) * 2 - scroll, tint, bitset<8>(channel).to_string());
                chanIter++;
            }
            firstTick = false;
            drawTitle("Inventory");
            theScreen -> end();
        }
        firstCallToReleased = false;
    }


/*****************************************************************************/
//Config menus
/*****************************************************************************/

    void gameMenu::options(int& status, configData& configFile) {
        const int rows = 14, cols = 2;
        vector<conflict> conflicts = configFile.getConflicts();
        init(cols, rows + conflicts.size());
        while (status == optionsStatus) {
            handleInput();
            if (ySelect % yCount < 3 || ySelect % yCount >= 11 + conflicts.size()) {
                xSelect = 0;
            }
            theScreen -> start(true);
            //Display settings
            if (button("Fullscreen", 0, 0, (IsWindowFullscreen() ? "true" : "false"))) {
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
            if (button("Interface scale", 0, 1, to_string(theScreen -> getHudFontSize()))) {
                theScreen -> tweakHudScale();
                configFile.setHudFontSize(theScreen -> getHudFontSize());
                configFile.save();
            }
            string gameScaleLabel = "Game scale";   //Need size to draw size indicator below
            if (button(gameScaleLabel, 0, 2, to_string(theScreen -> getFontSize()))) {
                theScreen -> tweakGameScale();
                configFile.setGameFontSize(theScreen -> getFontSize());
                configFile.save();
            }
            string gameScaleTest = "Game Scale Text";
            theScreen -> drawScaleTest(9 + gameScaleLabel.size(), 6 - scroll, RED, gameScaleTest);
            if (button("Auto switch camera in multiplayer", 0, 3, (configFile.getAutoCamera() ? "true" : "false"))) {
                configFile.setAutoCamera(!configFile.getAutoCamera());
                configFile.save();
            }
            //Player config settings
            for (int i = 0; i < MAXPLAYERS; i++) {
                theScreen -> drawHud(1, 2 * (5 + i) - scroll, configFile.getConfigColor(i), "Player " + to_string(i));
                if (button("\xE2\x96\xBA Edit Input", 0, 4 + i, "", 10)) {
                    playerConfig c = configFile.getPlayerConfig(i);
                    status = inputOptionsStatus;
                    editInputMap(status, c.in);
                    configFile.setConfig(i, c);
                    configFile.save();
                    conflicts = configFile.getConflicts();
                    init(cols, rows + conflicts.size());
                }
                if (button("\xE2\x96\xBA Edit Color", 1, 4 + i, "", 24)) {
                    playerConfig c = configFile.getPlayerConfig(i);
                    c.tint = editColor(c.tint);
                    configFile.setConfig(i, c);
                    configFile.save();
                    conflicts = configFile.getConflicts();
                    init(cols, rows + conflicts.size());
                }
            }
            for (int i = 0; i < conflicts.size(); i++) {
                theScreen -> drawHud(1, 2 * (MAXPLAYERS + 5 + i) - scroll, RED, "Conflict:            and           " + conflicts[i].error);
                if (button("\xE2\x96\xBA Player " + to_string(conflicts[i].player1), 0, MAXPLAYERS + 4 + i, "", 11)) {
                    playerConfig c = configFile.getPlayerConfig(conflicts[i].player1);
                    status = inputOptionsStatus;
                    editInputMap(status, c.in);
                    configFile.setConfig(conflicts[i].player1, c);
                    configFile.save();
                    conflicts = configFile.getConflicts();
                    init(cols, rows + conflicts.size());
                }
                else if (button("\xE2\x96\xBA Player " + to_string(conflicts[i].player2), 1, MAXPLAYERS + 4 + i, "", 26)) {
                    playerConfig c = configFile.getPlayerConfig(conflicts[i].player2);
                    status = inputOptionsStatus;
                    editInputMap(status, c.in);
                    configFile.setConfig(conflicts[i].player2, c);
                    configFile.save();
                    conflicts = configFile.getConflicts();
                    init(cols, rows + conflicts.size());
                }
            }
            if (button("Reset all defaults", 0, MAXPLAYERS + 4 + conflicts.size())) {
                string fileName = configFile.getFileName();
                configFile = configData(fileName, true);
                conflicts.clear();
                configFile.save();
            }
            if (goBack() || button("\xE2\x96\xBA Back to Menu", 0, MAXPLAYERS + 5 + conflicts.size())) {
                status = -1;    //Previous status will be restored in calling function
            }
            drawTitle("Options");
            theScreen -> end();
        }
        firstCallToReleased = false;
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
                theScreen -> drawHud(xSelect, ySelect + 1, c2, "\xE2\x98\x90");
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
        firstCallToReleased = false;
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
            if (toEdit -> keyboard) {
                if (button("Device type", 0, 0, "keyboard")) {
                    toEdit = &gamepadMap;
                }
                else if (button("Use mouse aim", 0, 1, (toEdit -> useMouseAim ? "true" : "false"))) {
                    toEdit -> useMouseAim = !toEdit -> useMouseAim;
                }
            }
            else {
                if (button("Device type", 0, 0, "gamepad")) {
                    toEdit = &keyboardMap;
                }
                else {
                    if (selectionToChange == toEdit -> count()) {
                        theScreen -> drawHud(1, 4 - scroll, RED, "PRESS GAMEPAD BUTTON OR MOVE JOYSTICK");
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
                    else if (button("Gamepad number", 0, 1, to_string(toEdit -> device))) {
                        selectionToChange = toEdit -> count();
                    }
                }
            }
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
                    theScreen -> drawHud(1, 2 * (i + 3) - scroll, RED, prompt);
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
                    if (button(toEdit -> name(i), 0, i + 2, (*toEdit)[i].name(toEdit -> device))) {
                        selectionToChange = i;
                        clickedNoButton = false;
                    }
                }
                if (selectionToChange > -1 && IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && firstCallToReleased && clickedNoButton) {
                    selectionToChange = -1;
                    firstCallToReleased = false;
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
        firstCallToReleased = false;
    }

