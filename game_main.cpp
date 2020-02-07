#include "bullet.hpp"
#include "col.hpp"
#include "door.hpp"
#include "effects.hpp"
#include "endinggate.hpp"
#include "forcefield.hpp"
#include "gamelayer.hpp"
#include "layer.hpp"
#include "meta.hpp"
#include "particles.hpp"
#include "pickups.hpp"
#include "player.hpp"
#include "rain.hpp"
#include "savepoint.hpp"

#define DRAWFPS false

using namespace rapidjson;

/******************************************************************************/
//Read in a list of entities from a file
/******************************************************************************/

void readEntities(entityList& el, collider*& col, Color& background, player* playerObj, string& fileName) {

    //Read into a json object

    Document json;
    FILE* entityFile = getLevelFileP(fileName);
    fseek (entityFile, 0, SEEK_END);
    char * buffer = new char[ftell (entityFile)];
    fseek (entityFile, 0, SEEK_SET);
    FileReadStream entityReadStream(entityFile, buffer, sizeof(buffer));
    if (json.ParseStream(entityReadStream).HasParseError()) {
        cerr << "Error parsing json.\n" << endl;
    }
    fclose(entityFile);

    //Read level meta information: background color and collider file name

    int backgroundR = json.HasMember("R") ? json["R"].GetInt() : 255;
    int backgroundG = json.HasMember("G") ? json["G"].GetInt() : 255;
    int backgroundB = json.HasMember("B") ? json["B"].GetInt() : 255;
    int backgroundA = json.HasMember("A") ? json["A"].GetInt() : 255;
    string colliderFileName = json.HasMember("collider") ? json["collider"].GetString() : "test_collider.txt";
    background = {backgroundR, backgroundG, backgroundB, backgroundA};
    col = new collider(0.0, 0.0, colliderFileName);

    //Get the list of entities

    const Value& entities = json["entities"];
    assert(entities.IsArray());

    for (SizeType i = 0; i < entities.Size(); i++) {
        const Value& entity = entities[i];
        assert(entity.IsObject());

        //Read in generic data values (almost all entities will have these)

        string type = entity.HasMember("type") ? entity["type"].GetString() : "unknown entity";
        cout << "Read in a " << type << endl;
        
        float x = entity.HasMember("x") ? entity["x"].GetFloat() : 0.0;
        float y = entity.HasMember("y") ? entity["y"].GetFloat() : 0.0;
        int R = entity.HasMember("R") ? entity["R"].GetInt() : 0;
        int G = entity.HasMember("G") ? entity["G"].GetInt() : 0;
        int B = entity.HasMember("B") ? entity["B"].GetInt() : 0;
        int A = entity.HasMember("A") ? entity["A"].GetInt() : 0;
        float sizeFactor = entity.HasMember("sizeFactor") ? entity["sizeFactor"].GetFloat() : 1.0;

        if (type == "layer") {
            string fileName = entity.HasMember("fileName") ? entity["fileName"].GetString() : "Error: No layer filename specified.";
            gameLayer * L = new gameLayer(x, y, R, G, B, A, sizeFactor, fileName);
            el.addEntity(L);
        }
        else if (type == "endingGate") {
            int width = entity.HasMember("width") ? entity["width"].GetInt() : 3;
            int height = entity.HasMember("height") ? entity["height"].GetInt() : 3;
            endingGate * E = new endingGate(x, y, R, G, B, A, sizeFactor, width, height);
            el.addEntity(E);
            col -> addCollideable(E);
        }
        else if (type == "door") {
            string nextRoom = entity.HasMember("nextRoom") ? entity["nextRoom"].GetString() : "Error: No door destination specified.";
            float destinationX = entity.HasMember("destinationX") ? entity["destinationX"].GetFloat() : 0.0;
            float destinationY = entity.HasMember("destinationY") ? entity["destinationY"].GetFloat() : 0.0;
            door * D = new door(x, y, R, G, B, A, sizeFactor, nextRoom, destinationX, destinationY);
            el.addEntity(D);
            col -> addCollideable(D);
        }
        else if (type == "savePoint") {
            savePoint * S = new savePoint(x, y, R, G, B, A, sizeFactor);
            el.addEntity(S);
            col -> addCollideable(S);
        }
        else if (type == "player") {
            playerObj -> setColor(R, G, B, A);
            playerObj -> setSizeFactor(sizeFactor);
            el.addEntity(playerObj);
            col -> addCollideable(playerObj);
        }
        else if (type == "forceField") {
            int channel = entity.HasMember("channel") ? entity["channel"].GetInt() : 0.0;
            float power = entity.HasMember("power") ? entity["power"].GetFloat() : 0;
            float range = entity.HasMember("range") ? entity["range"].GetFloat() : 0;
            forceField * F = new forceField(x, y, R, G, B, A, sizeFactor, channel, power, range);
            el.addEntity(F);
            col -> addCollideable(F);
        }
        else if (type == "rain") {
            float dropsPerTick = entity.HasMember("dropsPerTick") ? entity["dropsPerTick"].GetFloat() : 1.0;
            float xMomentum = entity.HasMember("xMomentum") ? entity["xMomentum"].GetFloat() : 0.0;
            bool isSnow = entity.HasMember("snow") ? entity["snow"].GetBool() : 0;
            rain * newRain = new rain(x, y, R, G, B, A, sizeFactor, dropsPerTick, xMomentum, isSnow);
            el.addEntity(newRain);
        }
        else if (type == "gunPickUp") {
            int pickUpID = entity.HasMember("pickUpID") ? entity["pickUpID"].GetInt() : -1;
            if (!playerObj -> isCollected(pickUpID)) {
                int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2511;
                int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                int gunID = entity.HasMember("gunID") ? entity["gunID"].GetInt() : 0;
                bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                gunPickUp * newGunPickUp = new gunPickUp(x, y, R, G, B, A, sizeFactor, displayChar, lifetime, pickUpID, touch, gunID);
                el.addEntity(newGunPickUp);
                col -> addCollideable(newGunPickUp);
            }
        }
        else if (type == "ammoPickUp") {
            int pickUpID = entity.HasMember("pickUpID") ? entity["pickUpID"].GetInt() : -1;
            if (!playerObj -> isCollected(pickUpID)) {
                int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x25a7;
                int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                int gunID = entity.HasMember("gunID") ? entity["gunID"].GetInt() : 0;
                int ammoCount = entity.HasMember("ammoCount") ? entity["ammoCount"].GetInt() : 1;
                bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                ammoPickUp * newAmmoPickUp = new ammoPickUp(x, y, R, G, B, A, sizeFactor, displayChar, lifetime, pickUpID, touch, gunID, ammoCount);
                el.addEntity(newAmmoPickUp);
                col -> addCollideable(newAmmoPickUp);
            }
        }
        else if (type == "healthPickUp") {
            int pickUpID = entity.HasMember("pickUpID") ? entity["pickUpID"].GetInt() : -1;
            if (!playerObj -> isCollected(pickUpID)) {
                int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2665;
                int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                int healthCount = entity.HasMember("healthCount") ? entity["healthCount"].GetInt() : 4;
                bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                healthPickUp * newHealthPickUp = new healthPickUp(x, y, R, G, B, A, sizeFactor, displayChar, lifetime, pickUpID, touch, healthCount);
                el.addEntity(newHealthPickUp);
                col -> addCollideable(newHealthPickUp);
            }
        }
        else if (type == "maxHealthPickUp") {
            int pickUpID = entity.HasMember("pickUpID") ? entity["pickUpID"].GetInt() : -1;
            if (!playerObj -> isCollected(pickUpID)) {
                int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2665;
                int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                int healthCount = entity.HasMember("healthCount") ? entity["healthCount"].GetInt() : 4;
                bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                maxHealthPickUp * newMaxHealthPickUp = new maxHealthPickUp(x, y, R, G, B, A, sizeFactor, displayChar, lifetime, pickUpID, touch, healthCount);
                el.addEntity(newMaxHealthPickUp);
                col -> addCollideable(newMaxHealthPickUp);
            }
        }
        else if (type == "opPickUp") {
            int pickUpID = entity.HasMember("pickUpID") ? entity["pickUpID"].GetInt() : -1;
            if (!playerObj -> isCollected(pickUpID)) {
                int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0;
                int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;

                //Really hacky: Pass bitwise op and argument data via the "message" field in the collision

                const Value& ops = entity["ops"];
                const Value& args = entity["args"];
                assert(ops.IsArray());
                assert(args.IsArray());
                string message;
                for (SizeType i = 0; i < ops.Size(); i++) {
                    string op = ops[i].GetString();
                    if (op == "rotate") {
                        message.append(1, 1);
                        if (!displayChar) {
                            if (args[i].GetInt() > 0) {
                                displayChar = 0xab;
                            }
                            else {
                                displayChar = 0xbb;
                            }
                        }
                    }
                    else if (op == "shift") {
                        message.append(1, 2);
                        if (!displayChar) {
                            if (args[i].GetInt() > 0) {
                                displayChar = '<';
                            }
                            else {
                                displayChar = '>';
                            }
                        }
                    }
                    else if (op == "set") {
                        message.append(1, 3);
                        if (!displayChar) {
                            displayChar = 0x25a3;
                        }
                    }
                    else if (op == "reset") {
                        message.append(1, 4);
                        if (!displayChar) {
                            displayChar = 0x2610;
                        }
                    }
                    else if (op == "xor") {
                        message.append(1, 5);
                        if (!displayChar) {
                            displayChar = 'X';
                        }
                    }
                    else if (op == "load") {
                        message.append(1, 6);
                        if (!displayChar) {
                            displayChar = 'L';
                        }
                    }
                    else {
                        message.append(1, 0);
                        if (!displayChar) {
                            displayChar = '?';
                        }
                    }
                    message.append(1, args[i].GetInt());
                }
                opPickUp * newOpPickUp = new opPickUp(x, y, R, G, B, A, sizeFactor, displayChar, lifetime, pickUpID, touch, message);
                el.addEntity(newOpPickUp);
                col -> addCollideable(newOpPickUp);
            }
        }
        else {
            cerr << "Bad entity type: " << type << endl;
            break;
        }
    }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


int main(int argc, char** argv) {

//Initialize raylib

    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "ASCII Platformer");
    SetTargetFPS(60);

    Font displayFont = LoadFontEx(FONTNAME, 8, FONTCHARS, NUMCHARS);

//misc initializations

    chrono::steady_clock::time_point tickStart, tickEnd;
    long total = 0, tickCount = 0;

    int won = 0;
    player* playerObj;
    string savedNextRoom;
    bool shouldChangeRooms = false;
    bool showInventory = false;

    while (!(won > 0 || WindowShouldClose())) {      //While we don't want to stop playing, and haven't won

        //Initialize

        playerObj = new player(0.0, 0.0, 255, 255, 255, 255, 1, "test.txt");
        if (argc == 1) {
            if (playerObj -> load("save")) {
                cout << "Loaded save.\n";
            }
            else {
                playerObj -> nextRoom = "test.txt";
            }
        }
        else {
            playerObj -> nextRoom = argv[1];
        }

        while (!(won || WindowShouldClose())) {         //While we're still alive

            //Read in entities, including colllider

            entityList entities;
            collider* col;
            Color background;

            readEntities(entities, col, background, playerObj, playerObj -> nextRoom);
            cout << "Finished reading in entities\n";

            //Camera initializations

            float cameraX = col -> getCols() / 2;
            float cameraY = col -> getRows() / 2;
            bool moveCameraX = (col -> getCols() > SCREENCOLS * playerObj -> getSizeFactor());
            bool moveCameraY = (col -> getRows() > SCREENROWS * playerObj -> getSizeFactor());

            shouldChangeRooms = false;

            while (!(won || WindowShouldClose() || shouldChangeRooms)) {        //While we're in the same room

                //Begin the tick

                won = playerObj -> won;
                tickStart = chrono::steady_clock::now();

                //Inventory screen?

                if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_I)) {
                    showInventory = !showInventory;
                }

                if (showInventory) {
                    BeginDrawing();
                    ClearBackground(UIBACKGROUND);
                    playerObj -> drawTabScreen(displayFont);
                    EndDrawing();
                }

                //Otherwise, just show normally

                else {

                    //Update camera

                    if (moveCameraX) {
                        if (playerObj -> x > cameraX + CAMERALAGX) {
                            cameraX = min(playerObj -> x - CAMERALAGX, col -> getCols() - SCREENCOLS / 2.0f);
                        }
                        else if (playerObj -> x < cameraX - CAMERALAGX) {
                            cameraX = max(playerObj -> x + CAMERALAGX, SCREENCOLS / 2.0f);
                        }
                    }
                    if (moveCameraY) {
                        if (playerObj -> y > cameraY + CAMERALAGY) {
                            cameraY = min(playerObj -> y - CAMERALAGY, col -> getRows() - SCREENROWS / 2.0f);
                        }
                        else if (playerObj -> y < cameraY - CAMERALAGY) {
                            cameraY = max(playerObj -> y + CAMERALAGY, SCREENROWS / 2.0f);
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

                    if (playerObj -> shouldChangeRooms) {
                        shouldChangeRooms = true;
                        playerObj -> shouldChangeRooms = false;
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
        delete playerObj;
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

