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
                uint8_t R = entity.HasMember("R") ? entity["R"].GetInt() : 0;
                uint8_t G = entity.HasMember("G") ? entity["G"].GetInt() : 0;
                uint8_t B = entity.HasMember("B") ? entity["B"].GetInt() : 0;
                uint8_t A = entity.HasMember("A") ? entity["A"].GetInt() : 0;
                float sizeFactor = entity.HasMember("sizeFactor") ? entity["sizeFactor"].GetFloat() : 1.0;
                string fileName = entity.HasMember("fileName") ? entity["fileName"].GetString() : "Error: No layer filename specified.";
                gameLayer * L = new gameLayer(x, y, {R, G, B, A}, sizeFactor, fileName);
                layerCache.push_back(L);
            }
        }
    }

    //Initialize global world and theCanvas pointers

    Color gameLevelData::getBackgroundColor() {
        uint8_t backgroundR = json.HasMember("R") ? json["R"].GetInt() : 255;
        uint8_t backgroundG = json.HasMember("G") ? json["G"].GetInt() : 255;
        uint8_t backgroundB = json.HasMember("B") ? json["B"].GetInt() : 255;
        uint8_t backgroundA = json.HasMember("A") ? json["A"].GetInt() : 255;
        return (Color){backgroundR, backgroundG, backgroundB, backgroundA};
    }

    string gameLevelData::getWorldFileName() {
        return json.HasMember("collider") ? json["collider"].GetString() : "test_collider.txt";
    }

    int gameLevelData::getFontSize() {
        return json.HasMember("fontSize") ? json["fontSize"].GetInt() : 16;
    }

    //Read in entities to global world collider

    void gameLevelData::readEntitiesGame(set<int> collectedPickups, player* thePlayer) {

        auto layerIter = layerCache.begin();

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

            if (type == "layer") {  //Add layers from the layer cache
                world -> addEntity(*layerIter);
                layerIter++;
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
                thePlayer -> setColor((Color){R, G, B, A});
                thePlayer -> setSizeFactor(sizeFactor);
                world -> addCollideable(thePlayer);
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
                float height = entity.HasMember("height") ? entity["height"].GetFloat() : 1.0;
                float wavelength = entity.HasMember("wavelength") ? entity["wavelength"].GetFloat() : 1.0;
                float amplitude = entity.HasMember("amplitude") ? entity["amplitude"].GetFloat() : 1.0;
                water * newWater = new water(x, y, {R, G, B, A}, sizeFactor, width, height, wavelength, amplitude);
                world -> addCollideable(newWater);
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
                    newTestEntity * newNTE = new newTestEntity(x, y, {R, G, B, A}, sizeFactor, displayChar, elasticity, newXMomentum,
                        newYMomentum, newMaxSpeed, newGravity, newFriction, maxHealth, newDamage);
                    world -> addCollideable(newNTE);
                }
            }
            else if (type == "gunPickup" || type == "ammoPickup" || type == "healthPickup" ||
                     type == "maxHealthPickup" || type == "airPickup" || type == "maxAirPickup" ||
                     type == "opPickup") {
                int pickupID = entity.HasMember("pickupID") ? entity["pickupID"].GetInt() : 0;
                if (!(collectedPickups.count(pickupID))) {
                    int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                    bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                    pickup* newPickup;
                    if (type == "gunPickup") {
                        int gunID = entity.HasMember("gunID") ? entity["gunID"].GetInt() : 0;
                        int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2511;
                        newPickup = new gunPickup(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickupID, touch, gunID);
                    }
                    else if (type == "ammoPickup") {
                        int gunID = entity.HasMember("gunID") ? entity["gunID"].GetInt() : 0;
                        int ammoCount = entity.HasMember("ammoCount") ? entity["ammoCount"].GetInt() : 1;
                        int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2511;
                        newPickup = new ammoPickup(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickupID, touch, gunID, ammoCount);
                    }
                    else if (type == "healthPickup") {
                        int healthCount = entity.HasMember("healthCount") ? entity["healthCount"].GetInt() : 4;
                        int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2665;
                        newPickup = new healthPickup(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickupID, touch, healthCount);
                    }
                    else if (type == "maxHealthPickup") {
                        int healthCount = entity.HasMember("healthCount") ? entity["healthCount"].GetInt() : 4;
                        int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2665;
                        newPickup = new maxHealthPickup(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickupID, touch, healthCount);
                    }
                    else if (type == "airPickup") {
                        int airCount = entity.HasMember("airCount") ? entity["airCount"].GetInt() : 4;
                        int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x02da;
                        newPickup = new airPickup(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickupID, touch, airCount);
                    }
                    else if (type == "maxAirPickup") {
                        int airCount = entity.HasMember("airCount") ? entity["airCount"].GetInt() : 4;
                        int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2299;
                        newPickup = new maxAirPickup(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickupID, touch, airCount);
                    }
                    else if (type == "opPickup") {
                        int opID = entity.HasMember("opID") ? entity["opID"].GetInt() : -1;
                        int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x00b1;
                        newPickup = new opPickup(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickupID, touch, opID);
                    }
                    world -> addCollideable(newPickup);
                }
            }
            else {
                cerr << "Bad entity type: " << type << endl;
            }
        }

    }

    Vector2 gameLevelData::getPlayerPosition() {
        const Value& entities = json["entities"];
        assert(entities.IsArray());

        for (SizeType i = 0; i < entities.Size(); i++) {
            const Value& entity = entities[i];
            assert(entity.IsObject());
            string type = entity.HasMember("type") ? entity["type"].GetString() : "unknown entity";

            if (type == "player") {
                float x = entity.HasMember("x") ? entity["x"].GetFloat() : 0.0;
                float y = entity.HasMember("y") ? entity["y"].GetFloat() : 0.0;
                return (Vector2){x, y};
            }
        }
        return (Vector2){0.0, 0.0};
    }

