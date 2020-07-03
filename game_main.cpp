#include "gameleveldata.hpp"
#include "savedata.hpp"
#include "game_menu.hpp"
#include "configdata.hpp"
#include "meta.hpp"

using namespace rapidjson;

int main(int argc, char** argv) {

//Initialize raylib

    configData config("config.json");
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
    vector<player*> players;
    string roomName;
/*    players.push_back(new player(0, 0, WHITE, 1.0));
    players.push_back(new player(0, 0, WHITE, 1.0));
    inputMap in(true);
    players[0] -> setInputMap(in);
    inputMap gamepad(false);
    players[1] -> setInputMap(gamepad);*/
    gameLevelData level;
    saveData save;
    bool loadedSave = false;
    bool argRoom = argc > 1;    //If room is defined by argument
    theScreen = new screen();
    theScreen -> setParams(0, 0, config.getGameFontSize(), config.getHudFontSize(), 1.0, 0);
    gameMenu menu;
    int transition = 7, transitionDirection = -1;

    while (!(status == quitStatus || WindowShouldClose())) {      //While the game is running

        //Menu logic

        if (status == menuStatus) {
            menu.main(status);
        }
        else if (status == optionsStatus) {
            menu.options(status, config);
            status = menuStatus; //Return to main menu after options complete.
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
            for (int i = 0; i < players.size(); i++) {
                players[i] -> breakDead = false;
                players[i] -> breakDoor = false;
            }
            loadedSave = save.load("save.json");

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
                level.load(roomName);
                level.generateLayerCache();
            }

            //Reload world
            world = new collider(0.0, 0.0, level.getWorldFileName());

            //Set the player position from save or level as appropriate
            if (loadedSave && !argRoom) {
                for (int i = 0; i < players.size(); i++) {
                    players[i] -> moveTo(save.getPosition());
                }
            }

            //Initialize the player outfit
            outfit defaultOutfit = level.getOutfit("defaultOutfit");
            for (int i = 0; i < players.size(); i++) {
                string outfitName = defaultOutfit.name + to_string(i);
                if (loadedSave && save.hasOutfit(outfitName)) {
                    outfit newOutfit = save.getOutfit(outfitName);
                    newOutfit.merge(defaultOutfit);
                    players[i] -> setOutfit(newOutfit);
                }
                else {
                    players[i] -> setOutfit(defaultOutfit);
                }
            }

            //Read in entities
            level.readEntitiesGame(players, (!loadedSave || argRoom));
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

            //Read in entities
            level.readEntitiesGame(players, false);
            theScreen -> setParams(world -> getRows(), world -> getCols(),
               config.getGameFontSize(), config.getHudFontSize(), players[0] -> getSizeFactor(), level.getDayLength());
            level.initializeColors(theScreen);

        }

        else if (status == saveStatus) {
            status = runStatus;
            argRoom = false;
            int whoSaved = 0;
            for (int i = 0; i < players.size(); i++) {
                if (players[i] -> breakSave) {
                    whoSaved = i;
                    players[i] -> breakSave = false;
                }
                save.writeOutfit(players[i] -> getCurrentOutfit(), i);
            }
            save.save(players[whoSaved] -> getPosition(), players[whoSaved] -> nextRoom);
        }

        else {
            while ((status == runStatus || status == pauseStatus || status == optionsStatus || status == inventoryStatus) && !WindowShouldClose()) {

                //Buttons modifying status

                int whoInventory = 0;
                for (int i = 0; i < players.size(); i++) {
                    if (players[i] -> breakInventory && status == runStatus) {
                        status = inventoryStatus;
                        whoInventory = i;
                        players[i] -> breakInventory = false;
                    }
                }
                if (menu.goBack() && status == runStatus) {
                    status = pauseStatus;
                }
                if (IsWindowMinimized() || IsWindowHidden()) {
                    status = pauseStatus;
                }

                //take appropriate action for status

                if (status == pauseStatus) {
                    menu.pause(status);
                }
                else if (status == optionsStatus) {
                    menu.options(status, config);
                    status = pauseStatus; //Return to pause menu after user is done w/ options.
                }
                else if (status == inventoryStatus) {
                    outfit o = players[whoInventory] -> getCurrentOutfit();
                    menu.inventory(status, o, in);
                    players[whoInventory] -> setOutfit(o);
                }
                else {  //status is run

                    //start tick timer

                    tickStart = chrono::steady_clock::now();
                    tickCounter++;

                    //Update entities

                    world -> tickSet();
                    world -> tickGet();
                    world -> finalize();

                    //Display the world

                    Vector2 playerPos = players[0] -> getPosition();
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

                    bool allDead = true;
                    bool allDoor = true;
                    bool anySave = false;
                    for (int i = 0; i < players.size(); i++) {
                        if (!players[i] -> breakDead) {
                            allDead = false;
                        }
                        if (!players[i] -> breakDead && !players[i] -> breakDoor) { //only wait for alive players before going thru door
                            allDoor = false;
                        }
                        if (players[i] -> breakSave) {
                            anySave = true;
                        }
                    }
                    if (allDead) {
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
                        status = saveStatus;
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

