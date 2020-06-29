#include "configdata.hpp"

    configData::configData(string fileName) {
        if (!load(fileName)) {
            Document::AllocatorType& a = json.GetAllocator();
            json.AddMember("fullscreen", Value(false), a);
            json.AddMember("hudFontSize", Value(16), a);
            json.AddMember("gameFontSize", Value(16), a);
            Value keys(kObjectType);
            keys.AddMember("up", Value(KEY_W), a);
            keys.AddMember("down", Value(KEY_S), a);
            keys.AddMember("left", Value(KEY_A), a);
            keys.AddMember("right", Value(KEY_D), a);
            keys.AddMember("inventory", Value(KEY_TAB), a);
            keys.AddMember("nextWeapon", Value(KEY_E), a);
            keys.AddMember("previousWeapon", Value(KEY_Q), a);
            keys.AddMember("explode", Value(KEY_R), a);
            keys.AddMember("lastCode", Value(KEY_LEFT_SHIFT), a);
            Value codes(kArrayType);
            for (int i = 0; i < 10; i++) {
                codes.PushBack(Value(KEY_ZERO + i), a);
            }
            keys.AddMember("code", codes, a);
            json.AddMember("keys", keys, a);
        }
    }

    bool configData::getFullscreen() {
        assert(json.HasMember("fullscreen"));
        return json["fullscreen"].GetBool();
    }

    void configData::setFullScreen(bool fullscreen) {
        assert(json.HasMember("fullscreen"));
        json["fullscreen"].SetBool(fullscreen);
    }

    int configData::getGameFontSize() {
        assert(json.HasMember("gameFontSize"));
        return json["gameFontSize"].GetInt();
    }

    void configData::setGameFontSize(int size) {
        assert(json.HasMember("gameFontSize"));
        json["gameFontSize"].SetInt(size);
    }

    int configData::getHudFontSize() {
        assert(json.HasMember("hudFontSize"));
        return json["hudFontSize"].GetInt();
    }

    void configData::setHudFontSize(int size) {
        assert(json.HasMember("hudFontSize"));
        json["hudFontSize"].SetInt(size);
    }

    keyMapping configData::getKeys() {
        keyMapping toReturn;
        Value& keys = json["keys"];
        toReturn.up = keys["up"].GetInt();
        toReturn.down = keys["down"].GetInt();
        toReturn.left = keys["left"].GetInt();
        toReturn.right = keys["right"].GetInt();
        toReturn.inventory = keys["inventory"].GetInt();
        toReturn.nextWeapon = keys["nextWeapon"].GetInt();
        toReturn.previousWeapon = keys["previousWeapon"].GetInt();
        toReturn.explode = keys["explode"].GetInt();
        toReturn.lastCode = keys["lastCode"].GetInt();
        for (int i = 0; i < 10; i++) {
            toReturn.code[i] = keys["code"][i].GetInt();
        }
        return toReturn;
    }

    void configData::setKeys(keyMapping newKeys) {
        json["keys"]["up"].SetInt(newKeys.up);
        json["keys"]["down"].SetInt(newKeys.down);
        json["keys"]["left"].SetInt(newKeys.left);
        json["keys"]["right"].SetInt(newKeys.right);
        json["keys"]["inventory"].SetInt(newKeys.inventory);
        json["keys"]["nextWeapon"].SetInt(newKeys.nextWeapon);
        json["keys"]["previousWeapon"].SetInt(newKeys.previousWeapon);
        json["keys"]["explode"].SetInt(newKeys.explode);
        json["keys"]["lastCode"].SetInt(newKeys.lastCode);
        for (int i = 0; i < 10; i++) {
            json["keys"]["code"][i].SetInt(newKeys.code[i]);
        }
    }
