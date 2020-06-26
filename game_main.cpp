#include "gameleveldata.hpp"
#include "savedata.hpp"
#include "game_menu.hpp"

using namespace rapidjson;

int main(int argc, char** argv) {

//Initialize raylib

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "ASCII Platformer");    //TODO: allow selecting monitor
    SetTargetFPS(60);
    SetExitKey(-1);

//misc initializations

    chrono::steady_clock::time_point tickStart = chrono::steady_clock::now(), tickEnd;
    long total = 0, totalSecond = 0;
    int maxLoad = 0;

    int status = breakMenu;
    player thePlayer(0, 0, (Color){255, 255, 255, 255}, 1.0);
    gameLevelData level;
    saveData save;
    bool loadedSave;
    bool argRoom = argc > 1;    //If room is defined by argument
    theCanvas = new canvas();

    while (!(status == breakQuit || WindowShouldClose())) {      //While the game is running

        //Menu logic

        if (status == breakMenu) {
            mainMenu(status);
        }
        else if (status == options) {
            optionsMenu(status, breakMenu);
        }

        //If we broke because we died (or first time), reload from save
        //or reload default position if save doesn't exist

        else if (status == breakDead) {
            status = run;
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
            //Read in entities
            level.readEntitiesGame(thePlayer.getCollectedPickups(), &thePlayer, true);
            theCanvas -> setParams(world -> getRows(), world -> getCols(), level.getFontSize(), thePlayer.getSizeFactor(), level.getDayLength());
            level.initializeColors(theCanvas);

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
        }

        //If we're moving to a different level, perform a subset
        //of the actions for death

        else if (status == breakDoor) {
            status = run;
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
            //Read in entities
            level.readEntitiesGame(thePlayer.getCollectedPickups(), &thePlayer, false);
            theCanvas -> setParams(world -> getRows(), world -> getCols(),
               level.getFontSize(), thePlayer.getSizeFactor(), level.getDayLength());
            level.initializeColors(theCanvas);

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
        }

        else if (status == breakSave) {
            status = run;
            argRoom = false;
            save.writeOutfit(thePlayer.getCurrentOutfit());
            save.save(thePlayer.getPosition(), thePlayer.nextRoom);
        }

        else {
            while ((status == run || status == pause || status == options) && !WindowShouldClose()) {

                //Buttons modifying status

                if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_I)) {
                    if (status == showInventory) {
                        status = run;
                    }
                    else if (status == run) {
                        status = showInventory;
                    }
                }
                if (IsKeyPressed(KEY_ESCAPE)) {
                    if (status == pause || status == showInventory) {
                        status = run;
                    }
                    if (status == run) {
                        status = pause;
                    }
                }
                if (IsWindowMinimized() || IsWindowHidden()) {
                    status = pause;
                }

                //take appropriate action for status

                if (status == pause) {
                    pauseMenu(status);
                }
                else if (status == options) {
                    optionsMenu(status, pause);
                }
                else if (status == showInventory) {
                    theCanvas -> start(true);
                    thePlayer.drawTabScreen();
                    theCanvas -> end();
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

                    Vector2 playerPos = thePlayer.getPos();
                    theCanvas -> calculateLighting();
                    theCanvas -> start(playerPos.x, playerPos.y, false);
                    world -> print();

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

                    theCanvas -> end();

                    //Loop break cases

                    if (thePlayer.breakDead) {
                        status = breakDead;
                    }
                    else if (thePlayer.breakSave) {
                        status = breakSave;
                    }
                    else if (thePlayer.breakDoor) {
                        status = breakDoor;
                    }
                }
            }
        }
    }

//final clean up

    CloseWindow();
    cout << "Average load: " << total * 100 / 1000000 / (tickCounter / 60) << "%\nMax load: " << maxLoad << "%\n";
}

