#include "gameleveldata.hpp"

/******************************************************************************/
//gameLevelData: Used to load level data for use by the game code
/******************************************************************************/

    //Generate all of the layers. (Layers take a long time to load.)

    void gameLevelData::generateLayerCache() {
        const Value& entities = json["entities"];
        assert(entities.IsArray());

        for (auto layerIter = layerCache.begin(); layerIter != layerCache.end(); layerIter++) {
            delete (*layerIter);
        }
        layerCache.clear();

        for (SizeType i = 0; i < entities.Size(); i++) {
            const Value& entity = entities[i];
            assert(entity.IsObject());
            string type = entity.HasMember("type") ? entity["type"].GetString() : "unknown entity";

            if (type == "layer" || type == "elevator" || type == "physicalBlock") {
                float x = entity.HasMember("x") ? entity["x"].GetFloat() : 0.0;
                float y = entity.HasMember("y") ? entity["y"].GetFloat() : 0.0;
                Color tint = entity.HasMember("tint") ? getColor(entity["tint"]) : (Color){0, 255, 0, 255};
                bool doLighting = entity.HasMember("doLighting") ? entity["doLighting"].GetBool() : true;
                bool doHighlight = entity.HasMember("doHighlight") ? entity["doHighlight"].GetBool() : false;
                float scale = entity.HasMember("scale") ? entity["scale"].GetFloat() : 1.0;
                string fileName = entity.HasMember("fileName") ? entity["fileName"].GetString() : "Error: No layer filename specified.";
                gameLayer * L = new gameLayer(x, y, tint, scale, fileName);
                if (entity.HasMember("effects")) {
                    for (SizeType j = 0; j < entity["effects"].Size(); j++) {
                        const Value& effect = entity["effects"][j];
                        int channel = effect["channel"].GetInt();
                        if (effect.HasMember("tint")) {
                            Color tint = getColor(effect["tint"]);
                            bool doLighting = effect.HasMember("doLighting") ? effect["doLighting"].GetBool() : true;
                            bool doHighlight = effect.HasMember("doHighlight") ? effect["doHighlight"].GetBool() : false;
                            L -> addColorEffect(channel, tint, doLighting, doHighlight);
                        }
                        else {  //Movement effect
                            int type = effect["type"].GetInt();
                            bool xMovement = effect.HasMember("xMovement") ? effect["xMovement"].GetBool() : false;
                            bool yMovement = effect.HasMember("yMovement") ? effect["yMovement"].GetBool() : false;
                            bool zMovement = effect.HasMember("zMovement") ? effect["zMovement"].GetBool() : false;
                            int period = effect.HasMember("period") ? effect["period"].GetInt() : 1;
                            float value = effect.HasMember("value") ? effect["value"].GetFloat() : 1;
                            L -> addMovementEffect(channel, type, xMovement, yMovement, zMovement, period, value);
                        }
                    }
                }
                L -> setLighting(doLighting, doHighlight);
                layerCache.push_back(L);
            }
        }
    }

    string gameLevelData::getWorldFileName() {
        return json.HasMember("collider") ? json["collider"].GetString() : "test_collider.txt";
    }

    int gameLevelData::getFontSize() {
        return json.HasMember("fontSize") ? json["fontSize"].GetInt() : 16;
    }

    int gameLevelData::getPlayerCount() {
        int toReturn = 0;
        for (SizeType i = 0; i < json["entities"].Size(); i++) {
            if (json["entities"][i]["type"] == "player") {
                toReturn++;
            }
        }
        return toReturn;
    }

    //Read in entities to global world collider

    void gameLevelData::readEntitiesGame(vector<player*>& players, bool movePlayers, set<int>* collectedPickups) {

        auto layerIter = layerCache.begin();
        auto playerIter = players.begin();

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
            Color tint = entity.HasMember("tint") ? getColor(entity["tint"]) : (Color){0, 255, 0, 255};
            bool doLighting = entity.HasMember("doLighting") ? entity["doLighting"].GetBool() : true;
            bool doHighlight;
            if (entity.HasMember("doHighlight")) {
                doHighlight = entity["doHighlight"].GetBool();
            }
            else if (type == "endingGate" || type == "door" || type == "savePoint" || type == "player"
                    || type == "forceField" || type == "linearField" || type == "snakeWall" || type == "activeWall"
                    || type == "elevator" || type == "gunPickup" || type == "ammoPickup"
                    || type == "opPickup" || type == "outfitPickup") {
                doHighlight = true;
            }
            else {
                doHighlight = false;
            }

            float scale = entity.HasMember("scale") ? entity["scale"].GetFloat() : 1.0;

            if (type == "layer") {  //Add layers from the layer cache
                world -> addEntity(*layerIter);
                layerIter++;
            }
            else if (type == "endingGate") {
                int width = entity.HasMember("width") ? entity["width"].GetInt() : 3;
                int height = entity.HasMember("height") ? entity["height"].GetInt() : 3;
                endingGate * E = new endingGate(x, y, tint, scale, width, height);
                world -> addCollideable(E);
                E -> setZPosition(world -> getZPosition());
            }
            else if (type == "door") {
                string nextRoom = entity.HasMember("nextRoom") ? entity["nextRoom"].GetString() : "Error: No door destination specified.";
                float destinationX = entity.HasMember("destinationX") ? entity["destinationX"].GetFloat() : 0.0;
                float destinationY = entity.HasMember("destinationY") ? entity["destinationY"].GetFloat() : 0.0;
                door * D = new door(x, y, tint, scale, nextRoom, destinationX, destinationY);
                D -> setLighting(doLighting, doHighlight);
                world -> addCollideable(D);
            }
            else if (type == "savePoint") {
                savePoint * S = new savePoint(x, y, tint, scale);
                S -> setLighting(doLighting, doHighlight);
                world -> addCollideable(S);
                S -> setZPosition (world -> getZPosition());
            }
            else if (type == "player") {
                //if movePlayers then move players to level-defined positions (dead)
                if (playerIter != players.end()) {
                    if (movePlayers) {
                        (*playerIter) -> moveTo((Vector2){x, y});
                    }
                    (*playerIter) -> setLighting(doLighting, doHighlight);
                    (*playerIter) -> setSizeFactor(scale);
                    world -> addCollideable(*playerIter);
                    (*playerIter) -> setZPosition (world -> getZPosition());
                    world -> addHudEntity(*playerIter);
                    playerIter++;
                }
            }
            else if (type == "forceField") {
                int channel = entity.HasMember("channel") ? entity["channel"].GetInt() : 0.0;
                float power = entity.HasMember("power") ? entity["power"].GetFloat() : 0;
                float range = entity.HasMember("range") ? entity["range"].GetFloat() : 0;
                Color onTint = entity.HasMember("onTint") ? getColor(entity["onTint"]) : tint;
                forceField * F = new forceField(x, y, tint, onTint, scale, channel, power, range);
                F -> setLighting(doLighting, doHighlight);
                world -> addCollideable(F);
                F -> setZPosition(world -> getZPosition());
            }
            else if (type == "linearField") {
                int channel = entity.HasMember("channel") ? entity["channel"].GetInt() : 0.0;
                float xPower = entity.HasMember("xPower") ? entity["xPower"].GetFloat() : 0;
                float yPower = entity.HasMember("yPower") ? entity["yPower"].GetFloat() : 0;
                Color onTint = entity.HasMember("onTint") ? getColor(entity["onTint"]) : tint;
                int width = entity.HasMember("width") ? entity["width"].GetInt() : 5;
                int height = entity.HasMember("height") ? entity["height"].GetInt() : 5;
                linearField * L = new linearField(x, y, tint, onTint, scale, channel, xPower, yPower, width, height);
                L -> setLighting(doLighting, doHighlight);
                world -> addCollideable(L);
            }
            else if (type == "particleSpawner" || type == "rain" || type == "snow" || type == "smoke") {
                float width = entity.HasMember("width") ? entity["width"].GetFloat() : 1.0;
                float height = entity.HasMember("height") ? entity["height"].GetFloat() : 1.0;
                float depth = entity.HasMember("depth") ? entity["depth"].GetFloat() : 0;
                int channel = entity.HasMember("channel") ? entity["channel"].GetInt() : -1;
                vector<int> displayChars;
                if (entity.HasMember("displayChars")) {
                    if (entity["displayChars"].IsArray()) {
                        for (SizeType i = 0; i < entity["displayChars"].Size(); i++) {
                            displayChars.push_back(entity["displayChars"][i].GetInt());
                        }
                    }
                }
                int mix = entity.HasMember("mix") ? entity["mix"].GetInt() : 1 << 31;
                float elasticity = entity.HasMember("elasticity") ? entity["elasticity"].GetFloat() : 0;
                float xMomentum = entity.HasMember("xMomentum") ? entity["xMomentum"].GetFloat() : 0;
                float yMomentum = entity.HasMember("yMomentum") ? entity["yMomentum"].GetFloat() : 0;
                float maxSpeed = entity.HasMember("maxSpeed") ? entity["maxSpeed"].GetFloat() : 100;
                bool waterDelete = entity.HasMember("waterDelete") ? entity["waterDelete"].GetBool() : false;
                bool hitDelete = entity.HasMember("hitDelete") ? entity["hitDelete"].GetBool() : false;
                bool restDelete = entity.HasMember("restDelete") ? entity["restDelete"].GetBool() : false;
                float wobble = entity.HasMember("wobble") ? entity["wobble"].GetFloat() : 0;
                float gravity = entity.HasMember("gravity") ? entity["gravity"].GetFloat() : GRAVITY;
                float friction = entity.HasMember("friction") ? entity["friction"].GetFloat() : FRICTION;
                float lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 1 << 30;
                float rate;
                if (entity.HasMember("density")) {
                    rate = width * height * entity["density"].GetFloat();
                }
                else if (entity.HasMember("rate")) {
                    rate = entity["rate"].GetFloat();
                }
                else {
                    rate = width * height / 10;
                }
                bool fade = entity.HasMember("fade") ? entity["fade"].GetBool() : false;
                particleSpawner* newSpawner;
                if (type == "particleSpawner") {
                    newSpawner = new particleSpawner(x, y, tint, scale, width, height, depth, channel, displayChars, mix, elasticity, xMomentum, yMomentum, maxSpeed, waterDelete, hitDelete, restDelete, wobble, gravity, friction, lifetime, rate, fade);
                }
                else if (type == "rain") {
                    vector<int> rainDisplayChars = {0};
                    newSpawner = new particleSpawner(-20, -1, tint, scale, (world -> getCols()) / scale, 1, 0, channel, rainDisplayChars, 1 << 31, 0.1, xMomentum, 0.5, 0.5, true, true, true, 0, GRAVITY, 0.7, world -> getRows() * 10, rate, true);
                }
                else if (type == "snow") {
                    vector<int> snowDisplayChars = {'.', '*', 0x00a4, 0x02da, 0x0263c};
                    newSpawner = new particleSpawner(-20, -1, tint, scale, world -> getCols() + 40, 1, 0, -1, snowDisplayChars, 1 << 31, 0, xMomentum, 0.07, 0.07, true, true, true, 0.05, GRAVITY, 0, world -> getRows() * 50, rate, true);
                }
                else if (type == "smoke") {
                    vector<int> smokeDisplayChars = {'#', 0x15, '*', 0xe3, 0x11c, 0x15d, 0x160, 0x2c7, 0x2dc, 0x3be, 0x2248, 0x2591, 0x2592};
                    newSpawner = new particleSpawner(x, y, tint, scale, width, height, 0, channel, smokeDisplayChars, 10, 0, xMomentum, yMomentum, 0.07, true, false, false, 0.05, -0.01, 1, 120, rate, true);
                }
                else if (type == "leaves") {
                    vector<int> leafDisplayChars = {'.', ','};
                    newSpawner = new particleSpawner(x, y, tint, scale, width, height, 0, channel, leafDisplayChars, 25, 0.3, xMomentum, yMomentum, 0.15, false, false, true, 0.15, GRAVITY, FRICTION, lifetime, rate, false);
                }
                newSpawner -> setLighting(doLighting, doHighlight);
                world -> addEntity(newSpawner);
                newSpawner -> setZPosition(world -> getZPosition());
            }
            else if (type == "water") {
                int width = entity.HasMember("width") ? entity["width"].GetInt() : 1;
                float height = entity.HasMember("height") ? entity["height"].GetFloat() : 1.0;
                float wavelength = entity.HasMember("wavelength") ? entity["wavelength"].GetFloat() : 1.0;
                float amplitude = entity.HasMember("amplitude") ? entity["amplitude"].GetFloat() : 1.0;
                water * newWater = new water(x, y, tint, scale, width, height, wavelength, amplitude);
                newWater -> setLighting(doLighting, doHighlight);
                world -> addCollideable(newWater);
                newWater -> setZPosition(world -> getZPosition());
            }
            else if (type == "newTestEntity") {
                int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt(): '?';
                float elasticity = entity.HasMember("elasticity") ? entity["elasticity"].GetFloat(): 0.0;
                float newXMomentum = entity.HasMember("xMomentum") ? entity["xMomentum"].GetFloat(): 0.0;
                float newYMomentum = entity.HasMember("yMomentum") ? entity["yMomentum"].GetFloat(): 0.0;
                float newMaxSpeed = entity.HasMember("maxSpeed") ? entity["maxSpeed"].GetFloat(): 100.0;
                float newGravity = entity.HasMember("gravity") ? entity["gravity"].GetFloat(): GRAVITY;
                float newFriction = entity.HasMember("friction") ? entity["friction"].GetFloat(): FRICTION;
                if (type == "newTestEntity") {
                    int maxHealth = entity.HasMember("maxHealth") ? entity["maxHealth"].GetInt(): 20;
                    int newDamage = entity.HasMember("damage") ? entity["damage"].GetInt(): -5;
                    newTestEntity * newNTE = new newTestEntity(x, y, tint, scale, displayChar, elasticity, newXMomentum,
                        newYMomentum, newMaxSpeed, newGravity, newFriction, maxHealth, newDamage);
                    newNTE -> setLighting(doLighting, doHighlight);
                    world -> addCollideable(newNTE);
                }
            }
            else if (type == "snakeWall") {
                bool loop = entity.HasMember("loop") ? entity["loop"].GetBool() : true;
                int forwardChannel = entity.HasMember("forwardChannel") ? entity["forwardChannel"].GetInt() : -1;
                int reverseChannel = entity.HasMember("reverseChannel") ? entity["reverseChannel"].GetInt() : -1;
                int snakeLength = entity.HasMember("snakeLength") ? entity["snakeLength"].GetInt() : 1;
                int snakeHead = entity.HasMember("snakeHead") ? entity["snakeHead"].GetInt() : snakeLength;
                int ticksPerMove = entity.HasMember("ticksPerMove") ? entity["ticksPerMove"].GetInt() : 1;
                string display = entity.HasMember("display") ? utf8(entity["display"].GetInt()) : "#";
                assert(entity.HasMember("points"));
                assert(entity["points"].IsArray());
                snakeWall* newSnakeWall = new snakeWall(x, y, tint, scale, snakeLength, snakeHead, ticksPerMove, loop, forwardChannel, reverseChannel, display);
                for (SizeType i = 0; i < entity["points"].Size(); i++) {
                    int pointX = entity["points"][i]["x"].GetFloat();
                    int pointY = entity["points"][i]["y"].GetFloat();
                    newSnakeWall -> addPoint(pointX, pointY);
                }
                newSnakeWall -> finish();
                newSnakeWall -> setLighting(doLighting, doHighlight);
                world -> addEntity(newSnakeWall);
            }
            else if (type == "activeWall") {
                int width = entity.HasMember("width") ? entity["width"].GetInt() : 1;
                int height = entity.HasMember("height") ? entity["height"].GetInt() : 1;
                int channel = entity.HasMember("channel") ? entity["channel"].GetInt() : 0;
                int display = entity.HasMember("display") ? entity["display"].GetInt(): 0;
                activeWall * newActiveWall = new activeWall(x, y, tint, scale, width, height, channel, display);
                newActiveWall -> setLighting(doLighting, doHighlight);
                world -> addEntity(newActiveWall);
            }
            else if (type == "elevator") {
                float width = entity.HasMember("width") ? entity["width"].GetFloat() : 1;
                float height = entity.HasMember("height") ? entity["height"].GetFloat() : 1;
                float speed = entity.HasMember("speed") ? entity["speed"].GetFloat() : 0.05;
                bool loop = entity.HasMember("loop") ? entity["loop"].GetBool() : true;
                int forwardChannel = entity.HasMember("forwardChannel") ? entity["forwardChannel"].GetInt() : -1;
                int reverseChannel = entity.HasMember("reverseChannel") ? entity["reverseChannel"].GetInt() : -1;
                elevator* newElevator = new elevator(x, y, tint, scale, width, height, speed, loop, forwardChannel, reverseChannel, **layerIter);
                layerIter++;
                assert(entity.HasMember("points"));
                assert(entity["points"].IsArray());
                for (SizeType i = 0; i < entity["points"].Size(); i++) {
                    float pointX = entity["points"][i]["x"].GetFloat();
                    float pointY = entity["points"][i]["y"].GetFloat();
                    newElevator -> addPoint((Vector2){pointX, pointY});
                }
                int startingPoint = entity.HasMember("startingPoint") ? entity["startingPoint"].GetInt() : 0;
                newElevator -> finish(startingPoint);
                newElevator -> setLighting(doLighting, doHighlight);
                world -> addEntity(newElevator);
            }
            else if (type == "physicalBlock") {
                float width = entity.HasMember("width") ? entity["width"].GetFloat() : 1;
                float height = entity.HasMember("height") ? entity["height"].GetFloat(): 1;
                float elasticity = entity.HasMember("elasticity") ? entity["elasticity"].GetFloat() : 0;
                float xMomentum = entity.HasMember("xMomentum") ? entity["xMomentum"].GetFloat() : 0;
                float yMomentum = entity.HasMember("yMomentum") ? entity["yMomentum"].GetFloat() : 0;
                float maxSpeed = entity.HasMember("maxSpeed") ? entity["maxSpeed"].GetFloat() : 100;
                float gravity = entity.HasMember("gravity") ? entity["gravity"].GetFloat() : GRAVITY;
                float friction = entity.HasMember("friction") ? entity["friction"].GetFloat() : 0.9;
                physicalBlock* newPB = new physicalBlock(x, y, tint, scale, width, height, elasticity, xMomentum, yMomentum, maxSpeed, gravity, friction, **layerIter);
                layerIter++;
                newPB -> setLighting(doLighting, doHighlight);
                world -> addCollideable(newPB);
            }
            else if (type == "trigger" || type == "ANDRelay" || type == "ORRelay" || type == "NOTRelay" || type == "timerRelay" || type == "toggleRelay" || type == "randomSource") {
                vector<int> outputs;
                for (SizeType i = 0; i < entity["outputs"].Size(); i++) {
                    outputs.push_back(entity["outputs"][i].GetInt());
                }
                if (type == "trigger") {
                    int width = entity.HasMember("width") ? entity["width"].GetInt() : 1;
                    int height = entity.HasMember("height") ? entity["height"].GetInt() : 1;
                    bool mustInteract = entity.HasMember("mustInteract") ? entity["mustInteract"].GetBool() : false;
                    bool toggle = entity.HasMember("toggle") ? entity["toggle"].GetBool() : false;
                    set<int> triggeredBy;
                    if (entity.HasMember("triggeredBy")) {
                        for (SizeType i = 0; i < entity["triggeredBy"].Size(); i++) {
                            triggeredBy.insert(entity["triggeredBy"][i].GetInt());
                        }
                    }
                    trigger* newTrigger = new trigger(outputs, x, y, width, height, mustInteract, toggle, triggeredBy);
                    world -> addCollideable(newTrigger);
                }
                else {
                    logic* newLogic;
                    if (type == "ANDRelay" || type == "ORRelay") {
                        vector<int> inputs;
                        for (SizeType i = 0; i < entity["inputs"].Size(); i++) {
                            inputs.push_back(entity["inputs"][i].GetInt());
                        }
                        if (type == "ANDRelay") {
                            newLogic = new ANDRelay(outputs, inputs);
                        }
                        else if (type == "ORRelay") {
                            newLogic = new ORRelay(outputs, inputs);
                        }
                    }
                    else if (type == "randomSource") {
                        float mix = entity.HasMember("mix") ? entity["mix"].GetFloat() : 1;
                        float probability = entity.HasMember("probability") ? entity["probability"].GetFloat() : 0.5f;
                        newLogic = new randomSource(outputs, mix, probability);
                    }
                    else {
                        int input = entity["input"].GetInt();
                        if (type == "NOTRelay") {
                            newLogic = new NOTRelay(outputs, input);
                        }
                        else if (type == "timerRelay") {
                            int delay = entity["delay"].GetInt();
                            newLogic = new timerRelay(outputs, input, delay);
                        }
                        else if (type == "toggleRelay") {
                            newLogic = new toggleRelay(outputs, input);
                        }
                    }
                    world -> addEntity(newLogic);
                }
            }
            else if (type == "gunPickup" || type == "ammoPickup" ||
                     type == "opPickup" || type == "outfitPickup") {
                int pickupID;
                if (entity.HasMember("pickupID")) {
                    pickupID = entity["pickupID"].GetInt();
                }
                else {
                    pickupID = (1 << 31);
                }
                if (pickupID == (1 << 31) || collectedPickups -> count(pickupID) == 0) {
                    int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                    bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                    pickup* newPickup;
                    if (type == "gunPickup") {
                        int gunID = entity.HasMember("gunID") ? entity["gunID"].GetInt() : 0;
                        int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2511;
                        newPickup = new gunPickup(x, y, tint, scale, collectedPickups, displayChar, lifetime, pickupID, touch, gunID);
                    }
                    else if (type == "ammoPickup") {
                        int gunID = entity.HasMember("gunID") ? entity["gunID"].GetInt() : 0;
                        int ammoCount = entity.HasMember("ammoCount") ? entity["ammoCount"].GetInt() : 1;
                        int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2511;
                        newPickup = new ammoPickup(x, y, tint, scale, collectedPickups, displayChar, lifetime, pickupID, touch, gunID, ammoCount);
                    }
                    else if (type == "opPickup") {
                        int opID = entity.HasMember("opID") ? entity["opID"].GetInt() : -1;
                        int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x00b1;
                        newPickup = new opPickup(x, y, tint, scale, collectedPickups, displayChar, lifetime, pickupID, touch, opID);
                    }
                    else if (type == "outfitPickup") {
                        assert(entity.HasMember("key"));
                        assert(entity.HasMember("value"));
                        string key = entity["key"].GetString();
                        double value = entity["value"].GetDouble();
                        bool add = entity.HasMember("add") ? entity["add"].GetBool() : false;
                        int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : '?';
                        newPickup = new outfitPickup(x, y, tint, scale, collectedPickups, displayChar, lifetime, pickupID, touch, key, value, add);
                    }
                    else {
                        cout << "bad pickup type";
                        newPickup = NULL;
                    }
                    newPickup -> setLighting(doLighting, doHighlight);
                    world -> addCollideable(newPickup);
                    newPickup -> setZPosition(world -> getZPosition());
                }
            }
            else {
                cerr << "Bad entity type: " << type << endl;
            }
        }

    }
