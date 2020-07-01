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
    player thePlayer(0, 0, (Color){255, 255, 255, 255}, 1.0);
    inputMap in(false);
    thePlayer.setInputMap(in);
    gameLevelData level;
    saveData save;
    bool loadedSave;
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

        else if (status == deadStatus) {
            transition = 7, transitionDirection = -1;
            status = runStatus;
            thePlayer.breakDead = false;
            loadedSave = save.load("save.json");

            //Determine which room we're going to be in next
            if (argRoom) {
                thePlayer.nextRoom = argv[1];
            }
            else if (loadedSave) {
                thePlayer.nextRoom = save.getRoom();
            }
            else {
                thePlayer.nextRoom = "test.json";
            }

            //Reload world

            delete world;
            //If necessary, reload from file & recreate layer cache
            if (thePlayer.nextRoom != level.getFileName()) {
                level.load(thePlayer.nextRoom);
                level.generateLayerCache();
            }
            //Initialize global handlers
            world = new collider(0.0, 0.0, level.getWorldFileName());

            //Set the player position from save or level as appropriate
            if (loadedSave && !argRoom) {
                thePlayer.moveTo(save.getPosition());
            }

            //Initialize the player outfit
            outfit defaultOutfit = level.getOutfit("defaultOutfit");
            if (loadedSave && save.hasOutfit(defaultOutfit.name)) {
                outfit newOutfit = save.getOutfit(defaultOutfit.name);
                newOutfit.merge(defaultOutfit);
                thePlayer.setOutfit(newOutfit);
            }
            else {
                thePlayer.setOutfit(defaultOutfit);
            }

            //Read in entities
            level.readEntitiesGame(thePlayer.getCollectedPickups(), &thePlayer, true);
            theScreen -> setParams(world -> getRows(), world -> getCols(), config.getGameFontSize(), config.getHudFontSize(), thePlayer.getSizeFactor(), level.getDayLength());
            level.initializeColors(theScreen);
        }

        //If we're moving to a different level, perform a subset
        //of the actions for death

        else if (status == doorStatus) {
            transition = 7, transitionDirection = -1;
            status = runStatus;
            thePlayer.breakDoor = false;
            thePlayer.goToDoor();
            save.writeOutfit(thePlayer.getCurrentOutfit());

            //Reload world

            delete world;
            //If necessary, reload from file & recreate layer cache
            if (thePlayer.nextRoom != level.getFileName()) {
                level.load(thePlayer.nextRoom);
                level.generateLayerCache();
            }
            //Initialize global handlers
            world = new collider(0.0, 0.0, level.getWorldFileName());

            //Initialize the player outfit
            outfit defaultOutfit = level.getOutfit("defaultOutfit");
            if (save.hasOutfit(defaultOutfit.name)) {
                outfit newOutfit = save.getOutfit(defaultOutfit.name);
                newOutfit.merge(defaultOutfit);
                thePlayer.setOutfit(newOutfit);
            }
            else {
                thePlayer.setOutfit(defaultOutfit);
            }

            //Read in entities
            level.readEntitiesGame(thePlayer.getCollectedPickups(), &thePlayer, false);
            theScreen -> setParams(world -> getRows(), world -> getCols(),
               config.getGameFontSize(), config.getHudFontSize(), thePlayer.getSizeFactor(), level.getDayLength());
            level.initializeColors(theScreen);

        }

        else if (status == saveStatus) {
            status = runStatus;
            argRoom = false;
            thePlayer.breakSave = false;
            save.writeOutfit(thePlayer.getCurrentOutfit());
            save.save(thePlayer.getPosition(), thePlayer.nextRoom);
        }

        else {
            while ((status == runStatus || status == pauseStatus || status == optionsStatus || status == inventoryStatus) && !WindowShouldClose()) {

                //Buttons modifying status

                if (thePlayer.breakInventory && status == runStatus) {
                    status = inventoryStatus;
                    thePlayer.breakInventory = false;
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
                    outfit o = thePlayer.getCurrentOutfit();
                    menu.inventory(status, o, in);
                    thePlayer.setOutfit(o);
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

                    Vector2 playerPos = thePlayer.getPosition();
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

                    if (thePlayer.breakDead) {
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
                    else if (thePlayer.breakSave) {
                        status = saveStatus;
                    }
                    else if (thePlayer.breakDoor) {
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

