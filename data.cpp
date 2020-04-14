#include "data.hpp"

using namespace rapidjson;

/******************************************************************************/
//Generic parent class for reading jsons
/******************************************************************************/

    data::data() {
        json.SetObject();
    }

    bool data::load(string newFileName) {
        fileName = newFileName;
        FILE* jsonFile = getLevelFileP(fileName);
        if (jsonFile == NULL) {
            return false;
        }
        fseek (jsonFile, 0, SEEK_END);
        char * buffer = new char[ftell (jsonFile)];
        fseek (jsonFile, 0, SEEK_SET);
        FileReadStream entityReadStream(jsonFile, buffer, sizeof(buffer));
        if (json.ParseStream(entityReadStream).HasParseError()) {
            cerr << "Error parsing json.\n" << endl;
            exit(EXIT_FAILURE);
        }
        fclose(jsonFile);
        free(buffer);
        return true;
    }

    string data::getFileName() {
        return fileName;
    }

/******************************************************************************/
//Includes one method for reading outfits (player abilities)
/******************************************************************************/

    outfit outfitData::getOutfit(string name) {

        outfit toReturn;
        assert(json.HasMember(name.c_str()));
        const Value& outfitValue = json[name.c_str()];
        assert(outfitValue.IsObject());
        assert(outfitValue.HasMember("name"));
        toReturn.name = outfitValue["name"].GetString();
        toReturn.display = outfitValue.HasMember("display") ? outfitValue["display"].GetInt() : '@';
        toReturn.health = outfitValue.HasMember("health") ? outfitValue["health"].GetInt() : 8;
        toReturn.maxHealth = outfitValue.HasMember("maxHealth") ? outfitValue["maxHealth"].GetInt() : 8;
        toReturn.air = outfitValue.HasMember("air") ? outfitValue["air"].GetInt() : 600;
        toReturn.maxAir = outfitValue.HasMember("maxAir") ? outfitValue["maxAir"].GetInt() : 600;

        toReturn.elasticity = outfitValue.HasMember("elasticity") ? outfitValue["elasticity"].GetFloat() : 0.0;
        toReturn.gravity = outfitValue.HasMember("gravity") ? outfitValue["gravity"].GetFloat() : GRAVITY;

        toReturn.acceleration = outfitValue.HasMember("acceleration") ? outfitValue["acceleration"].GetFloat() : 0.018;
        toReturn.speed = outfitValue.HasMember("speed") ? outfitValue["speed"].GetFloat() : 0.2;
        toReturn.friction = outfitValue.HasMember("friction") ? outfitValue["friction"].GetFloat() : 0.5;
        toReturn.waterAcceleration = outfitValue.HasMember("waterAcceleration") ? outfitValue["waterAcceleration"].GetFloat() : 0.01;
        toReturn.waterSpeed = outfitValue.HasMember("swimSpeed") ? outfitValue["swimSpeed"].GetFloat() : 0.12;
        toReturn.waterFriction = outfitValue.HasMember("waterFriction") ? outfitValue["waterFriction"].GetFloat() : 0.95;

        toReturn.jumpSpeed = outfitValue.HasMember("jumpSpeed") ? outfitValue["jumpSpeed"].GetFloat() : 0.2;
        toReturn.jumpCount = outfitValue.HasMember("jumpCount") ? outfitValue["jumpCount"].GetInt() : 0;
        toReturn.autoRejump = outfitValue.HasMember("autoRejump") ? outfitValue["autoRejump"].GetBool() : false;
        toReturn.walljump = outfitValue.HasMember("walljump") ? outfitValue["walljump"].GetBool() : false;

        //Get the list of guns

        if (outfitValue.HasMember("guns")) {
            const Value& guns = outfitValue["guns"];
            assert(guns.IsArray());

            for (SizeType i = 0; i < guns.Size(); i++) {
                const Value& gunData = guns[i];
                weapon newGun;
                newGun.unlocked = gunData.HasMember("unlocked") ? gunData["unlocked"].GetBool() : false;
                newGun.hitWall = gunData.HasMember("hitWall") ? gunData["hitWall"].GetBool() : true;
                newGun.hitWater = gunData.HasMember("hitWater") ? gunData["hitWater"].GetBool() : true;
                newGun.hitEntity = gunData.HasMember("hitEntity") ? gunData["hitEntity"].GetBool() : true;
                newGun.sticky = gunData.HasMember("sticky") ? gunData["sticky"].GetBool() : false;
                newGun.automatic = gunData.HasMember("automatic") ? gunData["automatic"].GetBool() : false;
                newGun.explosionRange = gunData.HasMember("explosionRange") ? gunData["explosionRange"].GetFloat() : 0.0f;
                newGun.explosionPower = gunData.HasMember("explosionPower") ? gunData["explosionPower"].GetFloat() : 0.0f;
                newGun.lifetime = gunData.HasMember("lifetime") ? gunData["lifetime"].GetInt() : 60;
                newGun.speed = gunData.HasMember("speed") ? gunData["speed"].GetFloat() : 1.0f;
                newGun.damage = gunData.HasMember("damage") ? gunData["damage"].GetInt() : 1;
                newGun.gravity = gunData.HasMember("gravity") ? gunData["gravity"].GetFloat() : GRAVITY;
                newGun.elasticity = gunData.HasMember("elasticity") ? gunData["elasticity"].GetFloat() : 0.0f;
                newGun.bulletDisplay = gunData.HasMember("bulletDisplay") ? gunData["bulletDisplay"].GetInt() : 0;
                newGun.particleCount = gunData.HasMember("particleCount") ? gunData["particleCount"].GetInt() : 0;
                assert(gunData.HasMember("gunID"));
                newGun.gunID = gunData["gunID"].GetInt();
                newGun.gunType = gunData["gunType"].GetInt();
                newGun.ammo = gunData.HasMember("ammo") ? gunData["ammo"].GetInt() : 1;
                newGun.maxAmmo = gunData.HasMember("maxAmmo") ? gunData["maxAmmo"].GetInt() : -1;
                newGun.lastFired = -1 * 10000000;
                newGun.cooldown = gunData.HasMember("cooldown") ? gunData["cooldown"].GetInt() : 0;
                if (gunData.HasMember("display")) {
                    if (gunData["display"].IsInt()) {
                        int displayInt = gunData["display"].GetInt();
                        newGun.display = string(TextToUtf8(&displayInt, 1));
                    }
                    else {
                        newGun.display = gunData["display"].GetString();
                    }
                }
                uint8_t R = gunData.HasMember("R") ? gunData["R"].GetInt() : 255;
                uint8_t G = gunData.HasMember("G") ? gunData["G"].GetInt() : 255;
                uint8_t B = gunData.HasMember("B") ? gunData["B"].GetInt() : 255;
                uint8_t A = gunData.HasMember("A") ? gunData["A"].GetInt() : 255;
                newGun.tint = {R, G, B, A};
                newGun.tintFaded = {(R + 122) / 2, (G + 122) / 2, (B + 122) / 2, (A + 122) / 2};
                toReturn.guns.push_back(newGun);
            }
        }

        //Get the list of ops

        if (outfitValue.HasMember("ops")) {
            const Value& ops = outfitValue["ops"];
            assert(ops.IsArray());

            for (SizeType i = 0; i < ops.Size(); i++) {
                const Value& opData = ops[i];
                puzzleOp newOp;
                newOp.unlocked = opData.HasMember("unlocked") ? opData["unlocked"].GetBool() : false;
                newOp.opID = opData.HasMember("opID") ? opData["opID"].GetInt() : 0;
                const Value& subOps = opData["operations"];
                assert(subOps.IsArray());
                const Value& operands = opData["operands"];
                assert(operands.IsArray());
                int displayChar = 0;
                for (SizeType j = 0; j < subOps.Size(); j++) {
                    string op = subOps[j].GetString();
                    if (op == "rotate") {
                        newOp.operations.push_back(1);
                        if (!displayChar) {
                            if (operands[i].GetInt() > 0) {
                                displayChar = 0xab;
                            }
                            else {
                                displayChar = 0xbb;
                            }
                        }
                    }
                    else if (op == "shift") {
                        newOp.operations.push_back(2);
                        if (!displayChar) {
                            if (operands[i].GetInt() > 0) {
                                displayChar = '<';
                            }
                            else {
                                displayChar = '>';
                            }
                        }
                    }
                    else if (op == "set") {
                        newOp.operations.push_back(3);
                        if (!displayChar) {
                            displayChar = 0x25a3;
                        }
                    }
                    else if (op == "reset") {
                        newOp.operations.push_back(4);
                        if (!displayChar) {
                            displayChar = 0x2610;
                        }
                    }
                    else if (op == "xor") {
                        newOp.operations.push_back(5);
                        if (!displayChar) {
                            displayChar = 'X';
                        }
                    }
                    else if (op == "load") {
                        newOp.operations.push_back(6);
                        if (!displayChar) {
                            displayChar = 'L';
                        }
                    }
                    else {
                        newOp.operations.push_back(0);
                        if (!displayChar) {
                            displayChar = '?';
                        }
                    }
                    newOp.operands.push_back(operands[i].GetInt());
                }
                newOp.display = string(TextToUtf8(&displayChar, 1));
                toReturn.ops.push_back(newOp);
            }
        }

        if (outfitValue.HasMember("channels")) {
            const Value& channels = outfitValue["channels"];
            for (SizeType i = 0; i < channels.Size(); i++) {
                toReturn.channels[i] = bitset<8>(channels[i].GetInt());
            }
        }
        else {
            for (int i = 0; i < 10; i++) {
                toReturn.channels[i] = bitset<8>(0);
            }
        }

        if (outfitValue.HasMember("collectedPickups")) {
            const Value& collectedPickups = outfitValue["collectedPickups"];
            for (SizeType i = 0; i < collectedPickups.Size(); i++) {
                toReturn.collectedPickups.insert(collectedPickups[i].GetInt());
            }
        }

        return toReturn;
    }
