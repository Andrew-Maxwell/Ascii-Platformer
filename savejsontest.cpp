#include <fstream>
#include <iostream>
#include <set>
#include <vector>
#include <bitset>
#include <raylib.h>
#include <cassert>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/pointer.h>

using namespace std;
using namespace rapidjson;

struct weapon {
    bool unlocked = false;
    bool automatic;
    bool hitWall, hitWater, hitEntity;
    bool sticky;
    float explosionRange, explosionPower;
    int lifetime;
    float speed;
    int damage;
    float gravity;
    float elasticity;
    int gunID;
    int gunType;
    int ammo = 0;
    int maxAmmo = 0;
    int lastFired = 0;
    int cooldown = 60;
    int bulletDisplay;
    int particleCount;
    string display = "";
    Color tint, tintFaded;

    bool operator== (const weapon& other) {
        return gunID == other.gunID;
    }
};

struct puzzleOp {
    bool unlocked = false;
    int opID;
    string display = "";
    vector<uint8_t> operations;
    vector<uint8_t> operands;

    bool operator== (const puzzleOp& other) {
        return opID == other.opID;
    }
};

struct outfit {
    string name;
    int display;
    int health = 8;
    int maxHealth;
    int air;
    int maxAir;

    float elasticity, gravity, jumpSpeed;
    float acceleration, speed, friction;
    float waterAcceleration, waterSpeed, waterFriction;
    int jumpCount;
    bool autoRejump; //Creates a jetpack-like effect
    bool walljump;

    vector<weapon> guns;

    vector<puzzleOp> ops;
    bitset<8> channels[10];
    Color channelColors[10];

    set<int> collectedPickups;
};

int main(int argc, char** argv) {

    Document json;

    FILE* A = fopen("save.json", "r");
    fseek (A, 0, SEEK_END);
    char * buffer = new char[ftell (A)];
    fseek (A, 0, SEEK_SET);
    FileReadStream AReadStream(A, buffer, sizeof(buffer));
    if (json.ParseStream(AReadStream).HasParseError()) {
        cerr << "Error parsing json.\n" << endl;
    }
    fclose(A);
    free(buffer);

    outfit toReturn;
    assert(json.HasMember("test_outfit"));
    const Value& outfitValue = json["test_outfit"];
    assert(outfitValue.IsObject());
    assert(outfitValue.HasMember("name"));
    toReturn.name = outfitValue["name"].GetString();
    toReturn.display = outfitValue.HasMember("display") ? outfitValue["display"].GetInt() : '@';
    toReturn.health = outfitValue.HasMember("health") ? outfitValue["health"].GetInt() : 8;
    toReturn.maxHealth = outfitValue.HasMember("maxHealth") ? outfitValue["maxHealth"].GetInt() : 8;
    toReturn.air = outfitValue.HasMember("air") ? outfitValue["air"].GetInt() : 600;
    toReturn.maxAir = outfitValue.HasMember("maxAir") ? outfitValue["maxAir"].GetInt() : 600;

    toReturn.elasticity = outfitValue.HasMember("elasticity") ? outfitValue["elasticity"].GetFloat() : 0.0;
    toReturn.gravity = outfitValue.HasMember("gravity") ? outfitValue["gravity"].GetFloat() : 0;

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
            newGun.gravity = gunData.HasMember("gravity") ? gunData["gravity"].GetFloat() : 0;
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
                }
                else {
                    newGun.display = gunData["display"].GetString();
                }
            }
            newGun.tint = gunData.HasMember("tint") ? BLACK : (Color){255, 0, 0, 255};
            newGun.tintFaded = (Color){(newGun.tint.r + 122) / 2, (newGun.tint.g + 122) / 2, (newGun.tint.b + 122) / 2, (newGun.tint.a + 122) / 2};
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
                        if (operands[j].GetInt() <= 0) {
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
                        if (operands[j].GetInt() > 0) {
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
                newOp.operands.push_back(operands[j].GetInt());
            }
            toReturn.ops.push_back(newOp);
        }
    }

    if (outfitValue.HasMember("channels")) {
        const Value& channels = outfitValue["channels"];
        for (SizeType i = 0; i < channels.Size(); i++) {
            toReturn.channels[i] = bitset<8>(channels[i].GetInt());
        }
        cout << "Channels found.\n";
    }
    else {
        cout << "No channels found.\n";
        for (int i = 0; i < 10; i++) {
            toReturn.channels[i] = bitset<8>(0);
        }
    }

    if (outfitValue.HasMember("collectedPickups")) {
        const Value& collectedPickups = outfitValue["collectedPickups"];
        for (SizeType i = 0; i < collectedPickups.Size(); i++) {
            toReturn.collectedPickups.insert(collectedPickups[i].GetInt());
        }
        cout << "collectedPickups found.\n";
    }
    else {
        cout << "No collectedPickups found.\n";
    }
}
