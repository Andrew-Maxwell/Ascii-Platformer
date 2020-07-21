#include "configdata.hpp"

    configData::configData(string newFileName, bool reset) {
        fileName = newFileName;
        if (reset || !load(fileName)) {
            Document::AllocatorType& a = json.GetAllocator();
            json.AddMember("fullscreen", Value(false), a);
            json.AddMember("hudFontSize", Value(16), a);
            json.AddMember("gameFontSize", Value(16), a);
            json.AddMember("autoCamera", Value(false), a);
            json.AddMember("configs", Value(kArrayType), a);
            Color tints[8] = {GOLD, SKYBLUE, MAROON, BEIGE, DARKBLUE, DARKBROWN, VIOLET, PINK};
            for (int i = 0; i < 8; i++) {
                inputMap newMap(i - 1);
                Value newConfig(kObjectType);
                Value mapJson(kObjectType);
                mapJson.AddMember("device", Value(i - 1).Move(), a);
                mapJson.AddMember("useMouseAim", Value(i - 1 == -1).Move(), a);
                mapJson.AddMember("inputList", Value(kArrayType).Move(), a);
                for (int i = 0; i < newMap.count(); i++) {
                    Value newInput(kObjectType);
                    newInput.AddMember("isAxis", Value(newMap[i].isAxis).Move(), a);
                    newInput.AddMember("positive", Value(newMap[i].positive).Move(), a);
                    newInput.AddMember("id", Value(newMap[i].id).Move(), a);
                    mapJson["inputList"].PushBack(newInput.Move(), a);
                }
                newConfig.AddMember("inputMap", mapJson.Move(), a);
                Value tintJson(kArrayType);
                setColor(tintJson, tints[i]);
                newConfig.AddMember("tint", tintJson.Move(), a);
                json["configs"].PushBack(newConfig.Move(), a);
            }
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

    bool configData::getAutoCamera() {
        assert(json.HasMember("autoCamera"));
        return json["autoCamera"].GetBool();
    }

    void configData::setAutoCamera(bool autoCamera) {
        assert(json.HasMember("autoCamera"));
        json["autoCamera"].SetBool(autoCamera);
    }

    playerConfig configData::getPlayerConfig(int index) {
        assert(0 <= index && index < 8);
        playerConfig toReturn;
        toReturn.configNo = index;
        Value& configJson = json["configs"][index];
        toReturn.in = inputMap(configJson["inputMap"]["device"].GetInt());
        toReturn.in.useMouseAim = configJson["inputMap"]["useMouseAim"].GetBool();
        Value& list = configJson["inputMap"]["inputList"];
        for (SizeType i = 0; i < list.Size(); i++) {
            bool isAxis = list[i]["isAxis"].GetBool();
            bool positive = list[i]["positive"].GetBool();
            int id = list[i]["id"].GetInt();
            toReturn.in[i] = input(isAxis, positive, id);
        }
        toReturn.tint = getColor(configJson["tint"]);
        return toReturn;
    }

    Color configData::getConfigColor(int index) {
        assert(0 <= index && index < 8);
        return getColor(json["configs"][index]["tint"]);
    }

    void configData::setConfig(int index, playerConfig toWrite) {
        assert(0 <= index && index < 8);
        Value& configJson = json["configs"][index];
        Value& mapJson = configJson["inputMap"];
        mapJson["device"].SetInt(toWrite.in.device);
        mapJson["useMouseAim"].SetBool(toWrite.in.useMouseAim);
        Value& listJson = mapJson["inputList"];
        for (SizeType i = 0; i < listJson.Size(); i++) {
            listJson[i]["isAxis"].SetBool(toWrite.in[i].isAxis);
            listJson[i]["positive"].SetBool(toWrite.in[i].positive);
            listJson[i]["id"].SetInt(toWrite.in[i].id);
        }
        setColor(configJson["tint"], toWrite.tint);
    }

    vector<conflict> configData::getConflicts() {
        vector<conflict> toReturn;
        set<int> warnedKeys;
        for (int index1 = 0; index1 < 8; index1++) {
            for (int index2 = index1 + 1; index2 < 8; index2++) {
                int device1 = json["configs"][index1]["inputMap"]["device"].GetInt();
                int device2 = json["configs"][index2]["inputMap"]["device"].GetInt();
                if (device1 == device2) {
                    if (device1 == -1) {
                        set<int> keysUsed;
                        inputMap map1 = getPlayerConfig(index1).in;
                        inputMap map2 = getPlayerConfig(index2).in;
                        if (map1.useMouseAim && map2.useMouseAim) {
                            conflict c;
                            c.player1 = index1;
                            c.player2 = index2;
                            c.error = " both use mouse aim\n";
                            toReturn.push_back(c);
                        }
                        for (int i = 0; i < map1.count(); i++) {
                            if (map1[i].isDefined()) {
                                keysUsed.insert(map1[i].id);
                            }
                        }
                        for (int i = 0; i < map2.count(); i++) {
                            if (keysUsed.count(map2[i].id) && map2[i].isDefined() && !warnedKeys.count(map2[i].id)) {
                                conflict c;
                                c.player1 = index1;
                                c.player2 = index2;
                                c.error = " both use " + map2[i].name(device1) + "\n";
                                toReturn.push_back(c);
                                warnedKeys.insert(map2[i].id);
                            }
                        }
                    }
                    else {
                        conflict c;
                        c.player1 = index1;
                        c.player2 = index2;
                        c.error = " both use gamepad " + to_string(device1) + "\n";
                        toReturn.push_back(c);
                    }
                }
            }
        }
        return toReturn;
    }
