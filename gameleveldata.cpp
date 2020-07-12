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

            if (type == "layer") {
                float x = entity.HasMember("x") ? entity["x"].GetFloat() : 0.0;
                float y = entity.HasMember("y") ? entity["y"].GetFloat() : 0.0;
                Color tint = entity.HasMember("tint") ? getColor(entity["tint"]) : (Color){0, 255, 0, 255};
                bool doLighting = entity.HasMember("doLighting") ? entity["doLighting"].GetBool() : true;
                float sizeFactor = entity.HasMember("sizeFactor") ? entity["sizeFactor"].GetFloat() : 1.0;
                string fileName = entity.HasMember("fileName") ? entity["fileName"].GetString() : "Error: No layer filename specified.";
                gameLayer * L = new gameLayer(x, y, tint, sizeFactor, fileName);
                L -> setDoLighting(doLighting);
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

    void gameLevelData::readEntitiesGame(vector<player*>& players, bool reloadPlayers, set<int>* collectedPickups) {

        auto layerIter = layerCache.begin();
        int playerIter = 0;

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
            float sizeFactor = entity.HasMember("sizeFactor") ? entity["sizeFactor"].GetFloat() : 1.0;

            if (type == "layer") {  //Add layers from the layer cache
                world -> addEntity(*layerIter);
                layerIter++;
            }
            else if (type == "endingGate") {
                int width = entity.HasMember("width") ? entity["width"].GetInt() : 3;
                int height = entity.HasMember("height") ? entity["height"].GetInt() : 3;
                endingGate * E = new endingGate(x, y, tint, sizeFactor, width, height);
                world -> addCollideable(E);
                E -> setZPosition(world -> getZPosition());
            }
            else if (type == "door") {
                string nextRoom = entity.HasMember("nextRoom") ? entity["nextRoom"].GetString() : "Error: No door destination specified.";
                float destinationX = entity.HasMember("destinationX") ? entity["destinationX"].GetFloat() : 0.0;
                float destinationY = entity.HasMember("destinationY") ? entity["destinationY"].GetFloat() : 0.0;
                door * D = new door(x, y, tint, sizeFactor, nextRoom, destinationX, destinationY);
                D -> setDoLighting(doLighting);
                world -> addCollideable(D);
            }
            else if (type == "savePoint") {
                savePoint * S = new savePoint(x, y, tint, sizeFactor);
                S -> setDoLighting(doLighting);
                world -> addCollideable(S);
                S -> setZPosition (world -> getZPosition());
            }
            else if (type == "player") {
                //if reloadplayers then create new players in level-defined positions (dead)
                //otherwise, we load all of the players that already exist from the previous room (door)
                player* playerToAdd;
                bool skip = false;
                if (reloadPlayers) {
                    cout << "\t...and creating one\n";
                    playerToAdd = new player (x, y, tint, sizeFactor);
                    playerToAdd -> setInputMap(inputMap(playerIter - 1));
                    players.push_back(playerToAdd);
                    playerToAdd -> setColor((Color)tint);
                }
                else if (playerIter < players.size()) {
                    cout << "\t...and inserting existing one\n";
                    playerToAdd = players[playerIter];
                }
                else {
                    cout << "\t...but not loading one\n";
                    skip = true;
                }

                if (!skip) {
                    playerToAdd -> setDoLighting(doLighting);
                    playerToAdd -> setSizeFactor(sizeFactor);
                    world -> addCollideable(playerToAdd);
                    playerToAdd -> setZPosition (world -> getZPosition());
                    world -> addHudEntity(playerToAdd);
                }
                playerIter++;
            }
            else if (type == "forceField") {
                int channel = entity.HasMember("channel") ? entity["channel"].GetInt() : 0.0;
                float power = entity.HasMember("power") ? entity["power"].GetFloat() : 0;
                float range = entity.HasMember("range") ? entity["range"].GetFloat() : 0;
                Color onTint = entity.HasMember("onTint") ? getColor(entity["onTint"]) : tint;
                forceField * F = new forceField(x, y, tint, onTint, sizeFactor, channel, power, range);
                F -> setDoLighting(doLighting);
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
                linearField * L = new linearField(x, y, tint, onTint, sizeFactor, channel, xPower, yPower, width, height);
                L -> setDoLighting(doLighting);
                world -> addCollideable(L);
            }
            else if (type == "rain") {
                float dropsPerTick = entity.HasMember("dropsPerTick") ? entity["dropsPerTick"].GetFloat() : 1.0;
                float xMomentum = entity.HasMember("xMomentum") ? entity["xMomentum"].GetFloat() : 0.0;
                bool isSnow = entity.HasMember("snow") ? entity["snow"].GetBool() : 0;
                rain * newRain = new rain(x, y, tint, sizeFactor, dropsPerTick, xMomentum, isSnow);
                newRain -> setDoLighting(doLighting);
                world -> addEntity(newRain);
                newRain -> setZPosition(world -> getZPosition());
            }
            else if (type == "water") {
                int width = entity.HasMember("width") ? entity["width"].GetInt() : 1;
                float height = entity.HasMember("height") ? entity["height"].GetFloat() : 1.0;
                float wavelength = entity.HasMember("wavelength") ? entity["wavelength"].GetFloat() : 1.0;
                float amplitude = entity.HasMember("amplitude") ? entity["amplitude"].GetFloat() : 1.0;
                water * newWater = new water(x, y, tint, sizeFactor, width, height, wavelength, amplitude);
                newWater -> setDoLighting(doLighting);
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
                    newTestEntity * newNTE = new newTestEntity(x, y, tint, sizeFactor, displayChar, elasticity, newXMomentum,
                        newYMomentum, newMaxSpeed, newGravity, newFriction, maxHealth, newDamage);
                    newNTE -> setDoLighting(doLighting);
                    world -> addCollideable(newNTE);
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
                        newPickup = new gunPickup(x, y, tint, sizeFactor, collectedPickups, displayChar, lifetime, pickupID, touch, gunID);
                    }
                    else if (type == "ammoPickup") {
                        int gunID = entity.HasMember("gunID") ? entity["gunID"].GetInt() : 0;
                        int ammoCount = entity.HasMember("ammoCount") ? entity["ammoCount"].GetInt() : 1;
                        int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2511;
                        newPickup = new ammoPickup(x, y, tint, sizeFactor, collectedPickups, displayChar, lifetime, pickupID, touch, gunID, ammoCount);
                    }
                    else if (type == "opPickup") {
                        int opID = entity.HasMember("opID") ? entity["opID"].GetInt() : -1;
                        int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x00b1;
                        newPickup = new opPickup(x, y, tint, sizeFactor, collectedPickups, displayChar, lifetime, pickupID, touch, opID);
                    }
                    else if (type == "outfitPickup") {
                        assert(entity.HasMember("key"));
                        assert(entity.HasMember("value"));
                        string key = entity["key"].GetString();
                        double value = entity["value"].GetDouble();
                        bool add = entity.HasMember("add") ? entity["add"].GetBool() : false;
                        int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : '?';
                        newPickup = new outfitPickup(x, y, tint, sizeFactor, collectedPickups, displayChar, lifetime, pickupID, touch, key, value, add);
                    }
                    else {
                        cout << "bad pickup type";
                        newPickup = NULL;
                    }
                    newPickup -> setDoLighting(doLighting);
                    world -> addCollideable(newPickup);
                    newPickup -> setZPosition(world -> getZPosition());
                }
            }
            else {
                cerr << "Bad entity type: " << type << endl;
            }
        }

    }
