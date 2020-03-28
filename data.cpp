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
        FILE* entityFile = fopen(fileName.c_str(), "r");
        if (entityFile == NULL) {
            return false;
        }
        else {
            fseek (entityFile, 0, SEEK_END);
            char * buffer = new char[ftell (entityFile)];
            fseek (entityFile, 0, SEEK_SET);
            FileReadStream entityReadStream(entityFile, buffer, sizeof(buffer));
            if (json.ParseStream(entityReadStream).HasParseError()) {
                cerr << "Error parsing json.\n" << endl;
            }
            fclose(entityFile);
            free(buffer);
            return true;
        }
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
        toReturn.health = outfitValue.HasMember("health") ? outfitValue["health"].GetInt() : 8;
        toReturn.maxHealth = outfitValue.HasMember("maxHealth") ? outfitValue["maxHealth"].GetInt() : 8;
        toReturn.air = outfitValue.HasMember("air") ? outfitValue["air"].GetInt() : 600;
        toReturn.maxAir = outfitValue.HasMember("maxAir") ? outfitValue["maxAir"].GetInt() : 600;

        //Get the list of guns

        if (outfitValue.HasMember("guns")) {
            const Value& guns = outfitValue["guns"];
            assert(guns.IsArray());

            for (SizeType i = 0; i < guns.Size(); i++) {
                const Value& gunData = guns[i];
                weapon newGun;
                newGun.unlocked = gunData.HasMember("unlocked") ? gunData["unlocked"].GetBool() : false;
                assert(gunData.HasMember("gunID"));
                newGun.gunID = gunData["gunID"].GetInt();
                newGun.gunType = gunData["gunType"].GetInt();
                newGun.ammo = gunData.HasMember("ammo") ? gunData["ammo"].GetInt() : 1;
                newGun.maxAmmo = gunData.HasMember("maxAmmo") ? gunData["maxAmmo"].GetInt() : 1;
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
