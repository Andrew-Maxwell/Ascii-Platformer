#include "gameleveldata.hpp"
#include "savedata.hpp"

using namespace rapidjson;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


int main(int argc, char** argv) {

//Initialize raylib

    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "ASCII Platformer");
    SetTargetFPS(60);

//misc initializations

    chrono::steady_clock::time_point tickStart = chrono::steady_clock::now(), tickEnd;
    long total = 0, totalSecond = 0;
    int maxLoad = 0;

    bool breakDead = true;
    bool breakDoor = false;
    bool breakSave = false;
    bool quit = false;
    bool showInventory = false;
    player thePlayer(0, 0, (Color){255, 255, 255, 255}, 1.0);
    gameLevelData level;
    saveData save;
    bool loadedSave;
    theCanvas = new canvas();

//testing

    while (!(quit || WindowShouldClose())) {      //While the game is running

        /*
        Three possible reasons to break from game loop:
        Saving, death, or changing rooms (doors)
        Also, initializing the game is treated as a death.
        */

        //If we broke to save the game, then do so

        if (breakSave) {
            breakSave = false;
            save.writeOutfit(thePlayer.getCurrentOutfit());
            save.save(thePlayer.getPosition(), thePlayer.nextRoom);
        }

        //If we broke because we died (or first time), reload from save
        //or reload default position if save doesn't exist

        else if (breakDead) {
            breakDead = false;
            loadedSave = save.load("save.json");

            //Determine which room we're going to be in next
            if (argc > 1) {
                thePlayer.nextRoom = argv[1];
            }
            else if (loadedSave) {
                thePlayer.nextRoom = save.getRoom();
            }
            else {
                thePlayer.nextRoom = "test.txt";
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
            level.readEntitiesGame(thePlayer.getCollectedPickups(), &thePlayer);
            theCanvas -> setParams(world -> getRows(), world -> getCols(),
               level.getBackgroundColor(), level.getFontSize(), thePlayer.getSizeFactor());

            //Set the player position from save or level as appropriate
            if (loadedSave) {
                thePlayer.moveTo(save.getPosition());
            }
            else {
                thePlayer.moveTo(level.getPlayerPosition());
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

        else if (breakDoor) {
            breakDoor = false;
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
            level.readEntitiesGame(thePlayer.getCollectedPickups(), &thePlayer);
            theCanvas -> setParams(world -> getRows(), world -> getCols(),
               level.getBackgroundColor(), level.getFontSize(), thePlayer.getSizeFactor());

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
        else {
            cerr << "WTF!!??";
        }

        //Main game loop

        while (!(breakSave || breakDoor || breakDead || quit || WindowShouldClose())) {

            //Inventory screen?

            if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_I)) {
                showInventory = !showInventory;
            }

            if (showInventory) {
                theCanvas -> start(true);
                thePlayer.drawTabScreen();
                theCanvas -> end();
            }
            else {

                //start tick timer

                tickStart = chrono::steady_clock::now();
                tickCounter++;

                //Update entities

                world -> tickSet();
                world -> tickGet();
                world -> finalize();

                //Display the world

                Vector2 playerPos = thePlayer.getPos();
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

                breakSave = thePlayer.breakSave;
                breakDoor = thePlayer.breakDoor;
                breakDead = thePlayer.breakDead;
            }
        }
    }

//final clean up

    CloseWindow();
    cout << "Average load: " << total * 100 / 1000000 / (tickCounter / 60) << "%\nMax load: " << maxLoad << "%\n";
}

