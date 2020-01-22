#include "meta.hpp"
#include "game_meta_classes.hpp"
#include "game_classes.hpp"

#define DRAWFPS true

using namespace std;

/******************************************************************************/
//Read in a list of entities from a file
/******************************************************************************/

void readEntities(entityList& el, collider*& col, Color& background, playerEntity* player, string fileName) {
    ifstream entityFile;
    entityFile.open(fileName);
    if (!entityFile) {
        cerr << "Error opening entity file. Attempting to open" << fileName;
        exit(EXIT_FAILURE);
    }
    char c;
    float x, y, sizeFactor;
    int R, G, B, A;
    string colliderFileName;
    entityFile >> colliderFileName >> R >> G >> B >> A;
    background = {R, G, B, A};
    col = new collider(0.0, 0.0, colliderFileName);
    while (entityFile >> c >> x >> y >> R >> G >> B >> A >> sizeFactor) {
        cout << "Read in a " << c << endl;
        switch(c) {
            case 'L': {     //Layer
                string fileName;
                entityFile >> fileName;
                layer * L = new layer(x, y, R, G, B, A, sizeFactor, fileName);
                el.addEntity(L);
                break;
            }
            case 'E': {     //EndingGate
                int width, height;
                entityFile >> width >> height;
                endingGate * E = new endingGate(x, y, R, G, B, A, sizeFactor, width, height);
                el.addEntity(E);
                col -> addCollideable(E);
                break;
            }
            case 'D': {     //Door
                string nextRoom;
                float destinationX, destinationY;
                entityFile >> nextRoom >> destinationX >> destinationY;
                door * D = new door(x, y, R, G, B, A, sizeFactor, nextRoom, destinationX, destinationY);
                el.addEntity(D);
                col -> addCollideable(D);
                break;
            }
            case 'S': {     //Save point
                savePoint * S = new savePoint(x, y, R, G, B, A, sizeFactor);
                el.addEntity(S);
                col -> addCollideable(S);
                break;
            }
            case '@': {     //Player
                player -> setColor(R, G, B, A);
                player -> setSizeFactor(sizeFactor);
                el.addEntity(player);
                col -> addCollideable(player);
                break;
            }
            case 'F': {     //Forcefield
                int channel;
                float power, range;
                entityFile >> channel >> power >> range;
                forceField * F = new forceField(x, y, R, G, B, A, sizeFactor, channel, power, range);
                el.addEntity(F);
                col -> addCollideable(F);
                break;
            }
            case 'R': {     //Weather (rain, or snow)
                float dropsPerTick, xMomentum;
                char snow;
                entityFile >> dropsPerTick >> xMomentum >> snow;
                bool isSnow = snow == 's';
                rain * newRain = new rain(x, y, R, G, B, A, sizeFactor, dropsPerTick, xMomentum, isSnow);
                el.addEntity(newRain);
                break;
            }
            case 'G': {     //gun pickup
                int displayChar, lifetime, gunID;
                entityFile >> displayChar >> lifetime >> gunID;
                gunPickUp * newGunPickUp = new gunPickUp(x, y, R, G, B, A, sizeFactor, displayChar, lifetime, gunID);
                el.addEntity(newGunPickUp);
                col -> addCollideable(newGunPickUp);
                break;
            }
            case 'A': {      //ammo pickup
                int displayChar, lifetime, gunID, ammoCount;
                entityFile >> displayChar >> lifetime >> gunID >> ammoCount;
                ammoPickUp * newAmmoPickUp = new ammoPickUp(x, y, R, G, B, A, sizeFactor, displayChar, lifetime, gunID, ammoCount);
                el.addEntity(newAmmoPickUp);
                col -> addCollideable(newAmmoPickUp);
                break;
            }
            case 'H': {      //health pickup
                int displayChar, lifetime, healthCount;
                entityFile >> displayChar >> lifetime >> healthCount;
                healthPickUp * newHealthPickUp = new healthPickUp(x, y, R, G, B, A, sizeFactor, displayChar, lifetime, healthCount);
                el.addEntity(newHealthPickUp);
                col -> addCollideable(newHealthPickUp);
                break;
            }
            case 'M': {      //max health pickup
                int displayChar, lifetime, healthCount;
                entityFile >> displayChar >> lifetime >> healthCount;
                maxHealthPickUp * newMaxHealthPickUp = new maxHealthPickUp(x, y, R, G, B, A, sizeFactor, displayChar, lifetime, healthCount);
                el.addEntity(newMaxHealthPickUp);
                col -> addCollideable(newMaxHealthPickUp);
                break;
            }
            case 'O': {     //op pickup
                int displayChar, lifetime, count, op, arg;
                string message = "";
                entityFile >> displayChar >> lifetime >> count;
                for (int i = 0; i < count; i++) {
                    entityFile >> op >> arg;
                    message.append(1, (char)op);
                    message.append(1, (char)arg);
                }
                opPickUp * newOpPickUp = new opPickUp(x, y, R, G, B, A, sizeFactor, displayChar, lifetime, message);
                el.addEntity(newOpPickUp);
                col -> addCollideable(newOpPickUp);
                break;
            }
            default: {
                cerr << "Error: Bad entity when reading entity list.";
                entityFile.ignore(1000, '\n');
                break;
            }
        }
    }
    entityFile.close();
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


int main() {

//Initialize raylib

    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "ASCII Platformer");
    SetTargetFPS(60);

    Font displayFont = LoadFontEx(FONTNAME, 8, FONTCHARS, NUMCHARS);


//misc initializations

    chrono::steady_clock::time_point tickStart, tickEnd;
    long total = 0, tickCount = 0;

    int won = 0;
    playerEntity* player;
    string savedNextRoom;
    float savedX, savedY;
    bool shouldChangeRooms = false;
    bool showInventory = false;

    while (!(won > 0 || WindowShouldClose())) {      //While we don't want to stop playing, and haven't won

        //Initialize

        player = new playerEntity(0.0, 0.0, 255, 255, 255, 255, 1, "test.txt");
        if (player -> load("save")) {
            cout << "Loaded save.";
        }

        while (!(won || WindowShouldClose())) {         //While we're still alive

            //Read in entities, including colllider

            entityList entities;
            collider* col;
            Color background;

            readEntities(entities, col, background, player, player -> nextRoom);

            //Camera initializations

            float cameraX = col -> getCols() / 2;
            float cameraY = col -> getRows() / 2;
            bool moveCameraX = (col -> getCols() > SCREENCOLS * player -> getSizeFactor());
            bool moveCameraY = (col -> getRows() > SCREENROWS * player -> getSizeFactor());

            shouldChangeRooms = false;

            while (!(won || WindowShouldClose() || shouldChangeRooms)) {        //While we're in the same room

                //Begin the tick

                won = player -> won;
                tickStart = chrono::steady_clock::now();

                //Inventory screen?

                if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_I)) {
                    showInventory = !showInventory;
                }

                if (showInventory) {
                    BeginDrawing();
                    ClearBackground(UIBACKGROUND);
                    player -> drawTabScreen(displayFont);
                    EndDrawing();
                }

                //Otherwise, just show normally

                else {

                    //Update camera

                    if (moveCameraX) {
                        if (player -> x > cameraX + CAMERALAGX) {
                            cameraX = min(player -> x - CAMERALAGX, col -> getCols() - SCREENCOLS / 2.0f);
                        }
                        else if (player -> x < cameraX - CAMERALAGX) {
                            cameraX = max(player -> x + CAMERALAGX, SCREENCOLS / 2.0f);
                        }
                    }
                    if (moveCameraY) {
                        if (player -> y > cameraY + CAMERALAGY) {
                            cameraY = min(player -> y - CAMERALAGY, col -> getRows() - SCREENROWS / 2.0f);
                        }
                        else if (player -> y < cameraY - CAMERALAGY) {
                            cameraY = max(player -> y + CAMERALAGY, SCREENROWS / 2.0f);
                        }
                    }

                    //Update entities

                    entities.tickSet(*col);
                    col -> tickSet(*col);
                    entities.tickGet(*col);
                    col -> tickGet(*col);
                    entities.finalize();
                    col -> finalize();

                    //Display the world

                    BeginDrawing();
                    ClearBackground(background);
                    entities.print(cameraX, cameraY, displayFont);
                    if (DRAWFPS) {
                        myDrawText(displayFont, to_string(GetFPS()).c_str(), (Vector2){10, 10}, 16, 0, WHITE);
                    }
                    EndDrawing();

                    //Handle room changing

                    if (player -> shouldChangeRooms) {
                        shouldChangeRooms = true;
                        player -> shouldChangeRooms = false;
                        delete col;
                    }
                }

                //End the tick

                tickEnd = chrono::steady_clock::now();
                int tickLength = chrono::duration_cast<chrono::microseconds>(tickEnd - tickStart).count();
                total += tickLength;
                tickCount++;

                //Sleep until the next frame

//                this_thread::sleep_for(16666000ns - chrono::duration_cast<chrono::nanoseconds>(tickEnd - tickStart));

            }
        }
        delete player;
        won = max(won, 0);
    }

//final clean up

    CloseWindow();
    cout << "Average tick length: " << total / tickCount << endl;
    if (won == -1) {
        cout << "You lose.";
    }
    else {
        cout << "You win! your score is: " << won;
    }
}

