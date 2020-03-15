#include "bullet.hpp"

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
#include "water.hpp"

collider* world = NULL;

#define DRAWFPS true

using namespace rapidjson;

/******************************************************************************/
//Read in a list of entities from a file
/******************************************************************************/

void readEntities(Color& background, player* playerPtr, string& fileName) {

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

    uint8_t backgroundR = json.HasMember("R") ? json["R"].GetInt() : 255;
    uint8_t backgroundG = json.HasMember("G") ? json["G"].GetInt() : 255;
    uint8_t backgroundB = json.HasMember("B") ? json["B"].GetInt() : 255;
    uint8_t backgroundA = json.HasMember("A") ? json["A"].GetInt() : 255;
    string colliderFileName = json.HasMember("collider") ? json["collider"].GetString() : "test_collider.txt";
    background = {backgroundR, backgroundG, backgroundB, backgroundA};
    world = new collider(0.0, 0.0, colliderFileName);

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
        uint8_t R = entity.HasMember("R") ? entity["R"].GetInt() : 0;
        uint8_t G = entity.HasMember("G") ? entity["G"].GetInt() : 0;
        uint8_t B = entity.HasMember("B") ? entity["B"].GetInt() : 0;
        uint8_t A = entity.HasMember("A") ? entity["A"].GetInt() : 0;
        float sizeFactor = entity.HasMember("sizeFactor") ? entity["sizeFactor"].GetFloat() : 1.0;

        if (type == "layer") {
            string fileName = entity.HasMember("fileName") ? entity["fileName"].GetString() : "Error: No layer filename specified.";
            gameLayer * L = new gameLayer(x, y, {R, G, B, A}, sizeFactor, fileName);
            world -> addEntity(L);
        }
        else if (type == "endingGate") {
            int width = entity.HasMember("width") ? entity["width"].GetInt() : 3;
            int height = entity.HasMember("height") ? entity["height"].GetInt() : 3;
            endingGate * E = new endingGate(x, y, {R, G, B, A}, sizeFactor, width, height);
            world -> addCollideable(E);
        }
        else if (type == "door") {
            string nextRoom = entity.HasMember("nextRoom") ? entity["nextRoom"].GetString() : "Error: No door destination specified.";
            float destinationX = entity.HasMember("destinationX") ? entity["destinationX"].GetFloat() : 0.0;
            float destinationY = entity.HasMember("destinationY") ? entity["destinationY"].GetFloat() : 0.0;
            door * D = new door(x, y, {R, G, B, A}, sizeFactor, nextRoom, destinationX, destinationY);
            world -> addCollideable(D);
        }
        else if (type == "savePoint") {
            savePoint * S = new savePoint(x, y, {R, G, B, A}, sizeFactor);
            world -> addCollideable(S);
        }
        else if (type == "player") {
            playerPtr -> setColor({R, G, B, A});
            playerPtr -> setSizeFactor(sizeFactor);
            world -> addCollideable(playerPtr);
        }
        else if (type == "forceField") {
            int channel = entity.HasMember("channel") ? entity["channel"].GetInt() : 0.0;
            float power = entity.HasMember("power") ? entity["power"].GetFloat() : 0;
            float range = entity.HasMember("range") ? entity["range"].GetFloat() : 0;
            forceField * F = new forceField(x, y, {R, G, B, A}, sizeFactor, channel, power, range);
            world -> addCollideable(F);
        }
        else if (type == "rain") {
            float dropsPerTick = entity.HasMember("dropsPerTick") ? entity["dropsPerTick"].GetFloat() : 1.0;
            float xMomentum = entity.HasMember("xMomentum") ? entity["xMomentum"].GetFloat() : 0.0;
            bool isSnow = entity.HasMember("snow") ? entity["snow"].GetBool() : 0;
            rain * newRain = new rain(x, y, {R, G, B, A}, sizeFactor, dropsPerTick, xMomentum, isSnow);
            world -> addEntity(newRain);
        }
        else if (type == "water") {
            int width = entity.HasMember("width") ? entity["width"].GetInt() : 1;
            float depth = entity.HasMember("depth") ? entity["depth"].GetFloat() : 1.0;
            float wavelength = entity.HasMember("wavelength") ? entity["wavelength"].GetFloat() : 1.0;
            float amplitude = entity.HasMember("amplitude") ? entity["amplitude"].GetFloat() : 1.0;
            water * newWater = new water(x, y, {R, G, B, A}, sizeFactor, width, depth, wavelength, amplitude);
            world -> addCollideable(newWater);
        }
        else if (type == "gunPickUp") {
            int pickUpID = entity.HasMember("pickUpID") ? entity["pickUpID"].GetInt() : -1;
            if (!playerPtr -> isCollected(pickUpID)) {
                int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2511;
                int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                int gunID = entity.HasMember("gunID") ? entity["gunID"].GetInt() : 0;
                bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                gunPickUp * newGunPickUp = new gunPickUp(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickUpID, touch, gunID);
                world -> addCollideable(newGunPickUp);
            }
        }
        else if (type == "ammoPickUp") {
            int pickUpID = entity.HasMember("pickUpID") ? entity["pickUpID"].GetInt() : -1;
            if (!playerPtr -> isCollected(pickUpID)) {
                int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x25a7;
                int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                int gunID = entity.HasMember("gunID") ? entity["gunID"].GetInt() : 0;
                int ammoCount = entity.HasMember("ammoCount") ? entity["ammoCount"].GetInt() : 1;
                bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                ammoPickUp * newAmmoPickUp = new ammoPickUp(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickUpID, touch, gunID, ammoCount);
                world -> addCollideable(newAmmoPickUp);
            }
        }
        else if (type == "healthPickUp") {
            int pickUpID = entity.HasMember("pickUpID") ? entity["pickUpID"].GetInt() : -1;
            if (!playerPtr -> isCollected(pickUpID)) {
                int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2665;
                int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                int healthCount = entity.HasMember("healthCount") ? entity["healthCount"].GetInt() : 4;
                bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                healthPickUp * newHealthPickUp = new healthPickUp(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickUpID, touch, healthCount);
                world -> addCollideable(newHealthPickUp);
            }
        }
        else if (type == "maxHealthPickUp") {
            int pickUpID = entity.HasMember("pickUpID") ? entity["pickUpID"].GetInt() : -1;
            if (!playerPtr -> isCollected(pickUpID)) {
                int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2665;
                int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                int healthCount = entity.HasMember("healthCount") ? entity["healthCount"].GetInt() : 4;
                bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                maxHealthPickUp * newMaxHealthPickUp = new maxHealthPickUp(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickUpID, touch, healthCount);
                world -> addCollideable(newMaxHealthPickUp);
            }
        }
        else if (type == "airPickUp") {
            int pickUpID = entity.HasMember("pickUpID") ? entity["pickUpID"].GetInt() : -1;
            if (!playerPtr -> isCollected(pickUpID)) {
                int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x02da;
                int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                int airCount = entity.HasMember("airCount") ? entity["airCount"].GetInt() : 4;
                bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                airPickUp * newAirPickUp = new airPickUp(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickUpID, touch, airCount);
                world -> addCollideable(newAirPickUp);
            }
        }
        else if (type == "maxAirPickUp") {
            int pickUpID = entity.HasMember("pickUpID") ? entity["pickUpID"].GetInt() : -1;
            if (!playerPtr -> isCollected(pickUpID)) {
                int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2299;
                int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                int airCount = entity.HasMember("airCount") ? entity["airCount"].GetInt() : 4;
                bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                maxAirPickUp * newMaxAirPickUp = new maxAirPickUp(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickUpID, touch, airCount);
                world -> addCollideable(newMaxAirPickUp);
            }
        }
        else if (type == "opPickUp") {
            int pickUpID = entity.HasMember("pickUpID") ? entity["pickUpID"].GetInt() : -1;
            if (!playerPtr -> isCollected(pickUpID)) {
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
                opPickUp * newOpPickUp = new opPickUp(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickUpID, touch, message);
                world -> addCollideable(newOpPickUp);
            }
        }
        else {
            cerr << "Bad entity type: " << type << endl;
            break;
        }
    }
    free(buffer);
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
    player* playerPtr;
    string savedNextRoom;
    bool shouldChangeRooms = false;
    bool showInventory = false;

    while (!(won > 0 || WindowShouldClose())) {      //While we don't want to stop playing, and haven't won

        //Initialize

        playerPtr = new player(0.0, 0.0, {255, 255, 255, 255}, 1, "test.txt");
        if (argc == 1) {
            if (playerPtr -> load("save")) {
                cout << "Loaded save.\n";
            }
            else {
                playerPtr -> nextRoom = "test.txt";
            }
        }
        else {
            playerPtr -> nextRoom = argv[1];
        }

        while (!(won || WindowShouldClose())) {         //While we're still alive (playerPtr persists between rooms)

            //Read in entities, including colllider

            Color background;
            readEntities(background, playerPtr, playerPtr -> nextRoom);
            cout << "Finished reading in entities\n";

            //Camera initializations

            float cameraX = world -> getCols() / 2;
            float cameraY = world -> getRows() / 2;
            bool moveCameraX = (world -> getCols() > SCREENCOLS * playerPtr -> getSizeFactor());
            bool moveCameraY = (world -> getRows() > SCREENROWS * playerPtr -> getSizeFactor());

            shouldChangeRooms = false;

            while (!(won || WindowShouldClose() || shouldChangeRooms)) {        //While we're in the same room

                //Begin the tick

                won = playerPtr -> won;
                tickStart = chrono::steady_clock::now();

                //Inventory screen?

                if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_I)) {
                    showInventory = !showInventory;
                }

                if (showInventory) {
                    BeginDrawing();
                    ClearBackground(UIBACKGROUND);
                    playerPtr -> drawTabScreen(displayFont);
                    EndDrawing();
                }

                //Otherwise, just show normally

                else {

                    //Update camera

                    if (moveCameraX) {
                        if (playerPtr -> x > cameraX + CAMERALAGX) {
                            cameraX = min(playerPtr -> x - CAMERALAGX, world -> getCols() - SCREENCOLS / 2.0f);
                        }
                        else if (playerPtr -> x < cameraX - CAMERALAGX) {
                            cameraX = max(playerPtr -> x + CAMERALAGX, SCREENCOLS / 2.0f);
                        }
                    }
                    if (moveCameraY) {
                        if (playerPtr -> y > cameraY + CAMERALAGY) {
                            cameraY = min(playerPtr -> y - CAMERALAGY, world -> getRows() - SCREENROWS / 2.0f);
                        }
                        else if (playerPtr -> y < cameraY - CAMERALAGY) {
                            cameraY = max(playerPtr -> y + CAMERALAGY, SCREENROWS / 2.0f);
                        }
                    }

                    //Update entities

                    world -> tickSet();
                    world -> tickGet();
                    world -> finalize();

                    //Display the world

                    BeginDrawing();
                    ClearBackground(background);
                    world -> print(cameraX, cameraY, displayFont);
                    if (DRAWFPS) {
                        myDrawText(displayFont, to_string(GetFPS()).c_str(), (Vector2){10, 10}, 16, 0, WHITE);
                    }
                    EndDrawing();

                    //Handle room changing

                    if (playerPtr -> shouldChangeRooms) {
                        shouldChangeRooms = true;
                        playerPtr -> shouldChangeRooms = false;
                        delete world;
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
        delete playerPtr;
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

