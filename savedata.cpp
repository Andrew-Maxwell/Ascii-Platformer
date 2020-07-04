#include "savedata.hpp"

    //Add an outfit to the rapidjson document

    bool saveData::hasOutfit(string name) {
        return json.HasMember(name.c_str());
    }

    string saveData::getRoom() {
        return json["room"].GetString();
    }

    Vector2 saveData::getPosition() {
        float x = json["x"].GetFloat();
        float y = json["y"].GetFloat();
        return (Vector2){x, y};
    }

    //put outfit in JSON

    void saveData::writeOutfit(outfit o, int playerNumber) {

        string name = o.name + to_string(playerNumber);
        json.RemoveMember(name.c_str());

        Document::AllocatorType& a = json.GetAllocator();
        Value newOutfit(kObjectType);
        Value nameValue;
        nameValue.SetString(o.name.c_str(), o.name.length(), a);
        newOutfit.AddMember("name", nameValue, a);
        newOutfit.AddMember("display", Value(o.display), a);
        newOutfit.AddMember("health", Value(o.health).Move(), a);
        newOutfit.AddMember("maxHealth", Value(o.maxHealth).Move(), a);
        newOutfit.AddMember("air", Value(o.air).Move(), a);
        newOutfit.AddMember("maxAir", Value(o.maxAir).Move(), a);
        newOutfit.AddMember("elasticity", Value(o.elasticity), a);
        newOutfit.AddMember("gravity", Value(o.gravity), a);

        newOutfit.AddMember("acceleration", Value(o.acceleration), a);
        newOutfit.AddMember("speed", Value(o.speed), a);
        newOutfit.AddMember("friction", Value(o.friction), a);
        newOutfit.AddMember("waterAcceleration", Value(o.waterAcceleration), a);
        newOutfit.AddMember("waterSpeed", Value(o.waterSpeed), a);
        newOutfit.AddMember("waterFriction", Value(o.waterFriction), a);

        newOutfit.AddMember("jumpSpeed", Value(o.jumpSpeed), a);
        newOutfit.AddMember("jumpCount", Value(o.jumpCount), a);
        newOutfit.AddMember("autoRejump", Value(o.autoRejump), a);
        newOutfit.AddMember("walljump", Value(o.walljump), a);

        Value gunsArray(kArrayType);

        //copy weapons

        for (int i = 0; i < o.guns.size(); i++) {
            Value newGun(kObjectType);
            newGun.AddMember("unlocked", Value(o.guns[i].unlocked).Move(), a);
            newGun.AddMember("automatic", Value(o.guns[i].automatic).Move(), a);
            newGun.AddMember("hitWall", Value(o.guns[i].hitWall).Move(), a);
            newGun.AddMember("hitWater", Value(o.guns[i].hitWater).Move(), a);
            newGun.AddMember("hitEntity", Value(o.guns[i].hitEntity).Move(), a);
            newGun.AddMember("sticky", Value(o.guns[i].sticky).Move(), a);
            newGun.AddMember("explosionRange", Value(o.guns[i].explosionRange).Move(), a);
            newGun.AddMember("explosionPower", Value(o.guns[i].explosionPower).Move(), a);
            newGun.AddMember("lifetime", Value(o.guns[i].lifetime).Move(), a);
            newGun.AddMember("speed", Value(o.guns[i].speed).Move(), a);
            newGun.AddMember("damage", Value(o.guns[i].damage).Move(), a);
            newGun.AddMember("gravity", Value(o.guns[i].gravity).Move(), a);
            newGun.AddMember("elasticity", Value(o.guns[i].elasticity).Move(), a);
            newGun.AddMember("gunID", Value(o.guns[i].gunID).Move(), a);
            newGun.AddMember("gunType", Value(o.guns[i].gunType).Move(), a);
            newGun.AddMember("ammo", Value(o.guns[i].ammo).Move(), a);
            newGun.AddMember("maxAmmo", Value(o.guns[i].maxAmmo).Move(), a);
            newGun.AddMember("cooldown", Value(o.guns[i].cooldown).Move(), a);
            newGun.AddMember("bulletDisplay", Value(o.guns[i].bulletDisplay).Move(), a);
            newGun.AddMember("particleCount", Value(o.guns[i].particleCount).Move(), a);
            Value display;
            display.SetString(o.guns[i].display.c_str(), o.guns[i].display.length(), a);
            newGun.AddMember("display", display, a);
            newGun.AddMember("R", Value(o.guns[i].tint.r).Move(), a);
            newGun.AddMember("G", Value(o.guns[i].tint.g).Move(), a);
            newGun.AddMember("B", Value(o.guns[i].tint.b).Move(), a);
            newGun.AddMember("A", Value(o.guns[i].tint.a).Move(), a);
            gunsArray.PushBack(newGun, a);
        }
        newOutfit.AddMember("guns", gunsArray, a);

        //copy puzzleOps

        Value opsArray(kArrayType);
        for (int i = 0; i < o.ops.size(); i++) {
            Value newOp(kObjectType);
            newOp.AddMember("unlocked", Value(o.ops[i].unlocked).Move(), a);
            newOp.AddMember("opID", Value(o.ops[i].opID).Move(), a);

            Value subOps(kArrayType);
            for (uint8_t subOp : o.ops[i].operations) {
                subOps.PushBack(Value(subOp).Move(), a);
            }
            newOp.AddMember("operations", subOps, a);

            Value operands(kArrayType);
            for (uint8_t operand : o.ops[i].operands) {
                operands.PushBack(Value(operand).Move(), a);
            }
            newOp.AddMember("operands", operands, a);
        }
        newOutfit.AddMember("ops", opsArray, a);

        //Copy channels

        Value channels(kArrayType);
        for (int i = 0; i < 10; i++) {
            channels.PushBack(Value(int(o.channels[i].to_ulong())).Move(), a);
        }
        newOutfit.AddMember("channels", channels, a);

        //Store it in the JSON under the outfit name

        Value label;
        label.SetString(name.c_str(), name.length(), a);
        json.AddMember(label, newOutfit, json.GetAllocator());
    }

    set<int> saveData::getCollectedPickups() {
        set<int> toReturn;
        if (json.HasMember("collectedPickups")) {
            for (SizeType i = 0; i < json["collectedPickups"].Size(); i++) {
                toReturn.insert(json["collectedPickups"][i].GetInt());
            }
        }
        return toReturn;
    }

    void saveData::setCollectedPickups(set<int> newSet) {
        Document::AllocatorType& a = json.GetAllocator();
        if (json.HasMember("collectedPickups")) {
            json.RemoveMember("collectedPickups");
        }
        Value collectedPickups(kArrayType);
        for (int pickupID : newSet) {
            collectedPickups.PushBack(Value(pickupID).Move(), a);
        }
        json.AddMember("collectedPickups", collectedPickups, a);
    }

    void saveData::save(Vector2 position, string room) {
        Document::AllocatorType& a = json.GetAllocator();
        json.RemoveMember("x");
        json.AddMember("x", position.x, a);
        json.RemoveMember("y");
        json.AddMember("y", position.y, a);
        json.RemoveMember("room");
        json.AddMember("room", StringRef(room.c_str()), a);

        FILE* out = fopen(fileName.c_str(), "wb");
        char buffer[65536];
        FileWriteStream ws(out, buffer, sizeof(buffer));
        PrettyWriter<FileWriteStream> writer(ws);
        json.Accept(writer);
        fclose(out);
    }



