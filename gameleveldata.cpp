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

    void gameLevelData::initializeGame(float playerSizeFactor) {

        //Read level meta information: background color and collider file name

        uint8_t backgroundR = json.HasMember("R") ? json["R"].GetInt() : 255;
        uint8_t backgroundG = json.HasMember("G") ? json["G"].GetInt() : 255;
        uint8_t backgroundB = json.HasMember("B") ? json["B"].GetInt() : 255;
        uint8_t backgroundA = json.HasMember("A") ? json["A"].GetInt() : 255;
        string colliderFileName = json.HasMember("collider") ? json["collider"].GetString() : "test_collider.txt";
        int fontSize = json.HasMember("fontSize") ? json["fontSize"].GetInt() : 16;
        Color background = {backgroundR, backgroundG, backgroundB, backgroundA};
        world = new collider(0.0, 0.0, colliderFileName);
        theCanvas -> setParams(world -> getRows(), world -> getCols(), background,
            fontSize, playerSizeFactor);
    }

    //Read in entities to global world collider

    void gameLevelData::readEntitiesGame(set<int> collectedPickups) {

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
                //Do nothing
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
            else if (type == "gunPickup") {
                int pickupID = entity.HasMember("pickupID") ? entity["pickupID"].GetInt() : 0;
                if (!(collectedPickups.count(pickupID))) {
                    int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2511;
                    int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                    int gunID = entity.HasMember("gunID") ? entity["gunID"].GetInt() : 0;
                    bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                    gunPickup * newGunPickup = new gunPickup(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickupID, touch, gunID);
                    world -> addCollideable(newGunPickup);
                }
            }
            else if (type == "ammoPickup") {
                int pickupID = entity.HasMember("pickupID") ? entity["pickupID"].GetInt() : 0;
                if (!(collectedPickups.count(pickupID))) {
                    int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x25a7;
                    int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                    int gunID = entity.HasMember("gunID") ? entity["gunID"].GetInt() : 0;
                    int ammoCount = entity.HasMember("ammoCount") ? entity["ammoCount"].GetInt() : 1;
                    bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                    ammoPickup * newAmmoPickup = new ammoPickup(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickupID, touch, gunID, ammoCount);
                    world -> addCollideable(newAmmoPickup);
                }
            }
            else if (type == "healthPickup") {
                int pickupID = entity.HasMember("pickupID") ? entity["pickupID"].GetInt() : 0;
                if (!(collectedPickups.count(pickupID))) {
                    int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2665;
                    int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                    int healthCount = entity.HasMember("healthCount") ? entity["healthCount"].GetInt() : 4;
                    bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                    healthPickup * newHealthPickup = new healthPickup(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickupID, touch, healthCount);
                    world -> addCollideable(newHealthPickup);
                }
            }
            else if (type == "maxHealthPickup") {
                int pickupID = entity.HasMember("pickupID") ? entity["pickupID"].GetInt() : 0;
                if (!(collectedPickups.count(pickupID))) {
                    int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2665;
                    int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                    int healthCount = entity.HasMember("healthCount") ? entity["healthCount"].GetInt() : 4;
                    bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                    maxHealthPickup * newMaxHealthPickup = new maxHealthPickup(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickupID, touch, healthCount);
                    world -> addCollideable(newMaxHealthPickup);
                }
            }
            else if (type == "airPickup") {
                int pickupID = entity.HasMember("pickupID") ? entity["pickupID"].GetInt() : 0;
                if (!(collectedPickups.count(pickupID))) {
                    int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x02da;
                    int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                    int airCount = entity.HasMember("airCount") ? entity["airCount"].GetInt() : 4;
                    bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                    airPickup * newAirPickup = new airPickup(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickupID, touch, airCount);
                    world -> addCollideable(newAirPickup);
                }
            }
            else if (type == "maxAirPickup") {
                int pickupID = entity.HasMember("pickupID") ? entity["pickupID"].GetInt() : 0;
                if (!(collectedPickups.count(pickupID))) {
                    int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 0x2299;
                    int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                    int airCount = entity.HasMember("airCount") ? entity["airCount"].GetInt() : 4;
                    bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                    maxAirPickup * newMaxAirPickup = new maxAirPickup(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickupID, touch, airCount);
                    world -> addCollideable(newMaxAirPickup);
                }
            }
            else if (type == "opPickup") {
                int pickupID = entity.HasMember("pickupID") ? entity["pickupID"].GetInt() : 0;
                if (!(collectedPickups.count(pickupID))) {
                    int displayChar = entity.HasMember("displayChar") ? entity["displayChar"].GetInt() : 'O';
                    int lifetime = entity.HasMember("lifetime") ? entity["lifetime"].GetInt() : 0x7FFFFFFF;
                    bool touch = entity.HasMember("touch") ? entity["touch"].GetBool() : false;
                    int opID = entity.HasMember("opID") ? entity["opID"].GetInt() : -1;
                    opPickup * newOpPickup = new opPickup(x, y, {R, G, B, A}, sizeFactor, displayChar, lifetime, pickupID, touch, opID);
                    world -> addCollideable(newOpPickup);
                }
            }
            else {
                cerr << "Bad entity type: " << type << endl;
            }
        }

    }

    void gameLevelData::readPlayer(player* playerPtr) {
        const Value& entities = json["entities"];
        assert(entities.IsArray());

        for (SizeType i = 0; i < entities.Size(); i++) {
            const Value& entity = entities[i];
            assert(entity.IsObject());
            string type = entity.HasMember("type") ? entity["type"].GetString() : "unknown entity";

            if (type == "player") {
                uint8_t R = entity.HasMember("R") ? entity["R"].GetInt() : 0;
                uint8_t G = entity.HasMember("G") ? entity["G"].GetInt() : 0;
                uint8_t B = entity.HasMember("B") ? entity["B"].GetInt() : 0;
                uint8_t A = entity.HasMember("A") ? entity["A"].GetInt() : 0;
                float sizeFactor = entity.HasMember("sizeFactor") ? entity["sizeFactor"].GetFloat() : 1.0;
                playerPtr -> setColor({R, G, B, A});
                playerPtr -> setSizeFactor(sizeFactor);
                world -> addCollideable(playerPtr);
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

