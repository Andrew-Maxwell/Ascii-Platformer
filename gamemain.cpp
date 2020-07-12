#include "gameleveldata.hpp"
#include "savedata.hpp"
#include "gamemenu.hpp"
#include "configdata.hpp"
#include "meta.hpp"

using namespace rapidjson;

int main(int argc, char** argv) {

//Initialize raylib

    configData config("config.json", false);
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1024, 768, "ASCII Platformer");    //TODO: allow selecting monitor
    SetWindowMinSize(1024, 768);
    SetTargetFPS(60);
    SetExitKey(-1);
    if (config.getFullscreen()) {
        int width = GetMonitorWidth(0);
        int height = GetMonitorHeight(0);
        ToggleFullscreen();
        SetWindowSize(width, height);
    }


//misc initializations

    chrono::steady_clock::time_point tickStart = chrono::steady_clock::now(), tickEnd;
    long total = 0, totalSecond = 0;
    int maxLoad = 0;
    int status = menuStatus;
    int mode;
    vector<playerConfig> playerConfigs;
    vector<player*> players;
    string roomName;
    inputMap in(-1);
    gameLevelData level;
    string saveName;
    saveData save;
    set<int> collectedPickups;
    bool loadedSave = false;
    bool argRoom = argc > 1;    //If room is defined by argument
    theScreen = new screen();
    theScreen -> setParams(0, 0, config.getGameFontSize(), config.getHudFontSize(), 1.0, 0);
    gameMenu menu;
    int transition = 7, transitionDirection = -1;
    Vector2 playerPos;

    while (!(status == quitStatus || WindowShouldClose())) {      //While the game is running

        //Menu logic

        if (status == menuStatus) {
            players.clear();
            playerConfigs.clear();
            menu.main(status, mode);
        }
        else if (status == optionsStatus) {
            menu.options(status, config);
            status = menuStatus; //Return to main menu after options complete.
        }
        else if (status == loadStatus) {
            saveName = menu.chooseSave(status, mode);
            if (mode == singlePlayerMode) {
                playerConfigs.push_back(config.getPlayerConfig(0));
            }
        }

        //If we broke because we died (or first time), reload from save
        //or reload default position if save doesn't exist

        /*
        The ordering of the statements when loading a level has a lot of "gotchas."
        For example:
            generateLayerCache after delete world (delete world accesses layers that generateLayerCache() deletes)

        */

        else if (status == deadStatus) {    //All players are dead
            transition = 7, transitionDirection = -1;
            status = runStatus;
            assert(players.size() == 0);
            if (!argRoom) {
                cout << "Loading save\n";
                loadedSave = save.load(saveName);
                if (loadedSave) {
                    cout << "Save loaded\n";
                }
            }

            //Determine which room we're going to be in next
            if (argRoom) {
                roomName = argv[1];
            }
            else if (loadedSave) {
                roomName = save.getRoom();
            }
            else {
                roomName = "test.json";
            }

            //Delete world
            delete world;

            //If necessary, reload from file & recreate layer cache
            if (roomName != level.getFileName()) {
                cout << "Loading level\n";
                level.load(roomName);
                cout << "Level loaded\n";
                level.generateLayerCache();
            }

            //Configure players
            if (playerConfigs.size() == 0) {    //if loading a save (i.e. coming from menu, not just died)
                playerConfigs = menu.readyRoom(save, loadedSave, level, config);
            }
            for (int i = 0; i < playerConfigs.size(); i++) {
                players.push_back(new player(0, 0, WHITE, 1));
                players[i] -> outfitNo = playerConfigs[i].playerNumber;
                players[i] -> playerNo = i;
                players[i] -> setColor(playerConfigs[i].tint);
                players[i] -> setInputMap(playerConfigs[i].in);
            }

            //Reload world
            world = new collider(0.0, 0.0, level.getWorldFileName());

            //Read in entities
            collectedPickups.clear();
            if (loadedSave) {
                collectedPickups = save.getCollectedPickups();
            }
            level.readEntitiesGame(players, false, &collectedPickups);

            //Set the player position from save if appropriate
            if (loadedSave && !argRoom) {
                for (int i = 0; i < players.size(); i++) {
                    players[i] -> moveTo(save.getPosition());
                }
            }

            //Initialize the player outfit
            outfit defaultOutfit = level.getOutfit("defaultOutfit");
            for (int i = 0; i < players.size(); i++) {
                string outfitName = defaultOutfit.name + to_string(players[i] -> outfitNo);
                if (loadedSave && save.hasOutfit(outfitName)) {
                    outfit newOutfit = save.getOutfit(outfitName);
                    newOutfit.merge(defaultOutfit);
                    players[i] -> setOutfit(newOutfit);
                }
                else {
                    players[i] -> setOutfit(defaultOutfit);
                }
            }

            //Set display parameters
            theScreen -> setParams(world -> getRows(), world -> getCols(), config.getGameFontSize(), config.getHudFontSize(), players[0] -> getSizeFactor(), level.getDayLength());
            level.initializeColors(theScreen);
        }

        //If we're moving to a different level, perform a subset
        //of the actions for death

        else if (status == doorStatus) {
            transition = 7, transitionDirection = -1;
            status = runStatus;
            for (int i = 0; i < players.size(); i++) {
                players[i] -> breakDoor = false;
                players[i] -> goToDoor();
            }
            roomName = players[0] -> nextRoom;

            for (int i = 0; i < players.size(); i++) {
                save.writeOutfit(players[i] -> getCurrentOutfit(), i);
            }

            delete world;

            //If necessary, reload from file & recreate layer cache
            if (roomName != level.getFileName()) {
                level.load(roomName);
                level.generateLayerCache();
            }

            //Reload world
            world = new collider(0.0, 0.0, level.getWorldFileName());

            //Read in entities
            level.readEntitiesGame(players, false, &collectedPickups);

            //Initialize the player outfit
            outfit defaultOutfit = level.getOutfit("defaultOutfit");
            for (int i = 0; i < players.size(); i++) {
                string outfitName = defaultOutfit.name + to_string(i);
                if (save.hasOutfit(outfitName)) {   //NOTE: This is done regardless of if loadedSave is true from earlier
                    outfit newOutfit = save.getOutfit(outfitName);
                    newOutfit.merge(defaultOutfit);
                    players[i] -> setOutfit(newOutfit);
                }
                else {
                    players[i] -> setOutfit(defaultOutfit);
                }
            }

            //Set display parameters
            theScreen -> setParams(world -> getRows(), world -> getCols(),
               config.getGameFontSize(), config.getHudFontSize(), players[0] -> getSizeFactor(), level.getDayLength());
            level.initializeColors(theScreen);

        }

        else if (status == saveStatus) {
            status = deadStatus;    //Reset the level
            argRoom = false;
            int whoSaved = 0;
            for (int i = 0; i < players.size(); i++) {
                if (players[i] -> breakSave) {
                    whoSaved = i;
                    players[i] -> breakSave = false;
                }
                save.writeOutfit(players[i] -> getCurrentOutfit(), i);
            }
            save.setCollectedPickups(collectedPickups);
            save.save(players[whoSaved] -> getPosition(), roomName);
            players.clear();
        }

        else {
            while ((status == runStatus || status == pauseStatus || status == optionsStatus || status == inventoryStatus) && !WindowShouldClose()) {

                //modifying status

                int whoInventory = 0;
                for (int i = 0; i < players.size(); i++) {
                    if (players[i] -> breakInventory && status == runStatus) {
                        players[i] -> breakInventory = false;
                        status = inventoryStatus;
                        whoInventory = i;
                    }
                }
                if ((IsWindowMinimized() || IsWindowHidden() || menu.goBack()) && status == runStatus) {
                    status = pauseStatus;
                }

                //take appropriate action for status

                if (status == pauseStatus) {
                    menu.pause(status);
                }
                else if (status == optionsStatus) {
                    menu.options(status, config);
                    status = pauseStatus; //Return to pause menu after options menu
                }
                else if (status == inventoryStatus) {
                    outfit o = players[whoInventory] -> getCurrentOutfit();
                    menu.inventory(status, o, in);
                    players[whoInventory] -> setOutfit(o);
                }
                else if (status == runStatus) { 

                    //start tick timer

                    tickStart = chrono::steady_clock::now();
                    tickCounter++;

                    //Update entities

                    world -> tickSet();
                    world -> tickGet();
                    vector<player*>::iterator playerIter = players.begin();
                    while (playerIter != players.end()) {
                        if ((*playerIter) -> finalize()) {
                            playerIter = players.erase(playerIter);
                        }
                        else {
                            playerIter++;
                        }
                    }
                    world -> finalize();

                    //Display the world

                    if (players.size() > 0) {
                        playerPos = players[0] -> getPosition();
                    }
                    theScreen -> calculateLighting();
                    theScreen -> start(playerPos.x, playerPos.y, false);
                    world -> print();

                    //transition
                    if (transition >= 0 && transition < 8) {
                        theScreen -> checkerboardTransition(transition);
                        if (tickCounter % 3 == 0) {
                            transition += transitionDirection;
                        }
                    }

                    //end tick timer

                    tickEnd = chrono::steady_clock::now();
                    int tickLength = chrono::duration_cast<chrono::microseconds>(tickEnd - tickStart).count();
                    total += tickLength;
                    totalSecond += tickLength;
                    if (STATS && tickCounter % 60 == 0) {
                        cout << "Load: " << totalSecond / 10000 << "%\n";
                        if (totalSecond / 10000 > maxLoad) {
                            maxLoad = totalSecond / 10000;
                        }
                        totalSecond = 0;
                    }

                    theScreen -> end();

                    //Loop break cases

                    bool allDoor = true;
                    bool anySave = false;
                    for (int i = 0; i < players.size(); i++) {
                        if (!players[i] -> breakDoor || !(players[i] -> getDoorDestination() == players[0] -> getDoorDestination())) {
                            allDoor = false;
                        }
                        if (players[i] -> breakSave) {
                            anySave = true;
                        }
                    }
                    if (players.size() == 0) {
                        //theScreen -> dialog ("You are dead...");
                        if (menu.goBack()) {
                            status = pauseStatus;
                        }
                        else if ((myGetKeyPressed() || getAnyGamepadInput()) && transition == -1) {
                            transition = 0;
                            transitionDirection = 1;
                        }
                        if (transition == 8) {
                            status = deadStatus;
                        }
                    }
                    else if (anySave) {
                        if (transition == -1) {
                            transition = 0;
                            transitionDirection = 1;
                        }
                        if (transition == 8) {
                            status = saveStatus;
                        }
                    }
                    else if (allDoor) {
                        if (transition == -1) {
                            transition = 0;
                            transitionDirection = 1;
                        }
                        if (transition == 8) {
                            status = doorStatus;
                        }
                    }
                }
            }
        }
    }

//final clean up

    CloseWindow();
    cout << "Average load: " << total * 100 / 1000000 / (tickCounter / 60) << "%\nMax load: " << maxLoad << "%\n";
}


