#include "configdata.hpp"

    configData::configData(string fileName) {
        if (!load(fileName)) {
            Document::AllocatorType& a = json.GetAllocator();
            json.AddMember("fullscreen", Value(false), a);
            json.AddMember("hudFontSize", Value(16), a);
            json.AddMember("gameFontSize", Value(16), a);
            json.AddMember("inputMaps", Value(kArrayType), a);
            inputMap defaultKeyboard(true);
            addInputMap(defaultKeyboard);
            inputMap defaultGamepad(false);
            addInputMap(defaultGamepad);
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

    int configData::inputMapCount() {
        return json["inputMaps"].Size();
    }

    inputMap configData::getInputMap(int mapNo) {
        Value& map = json["inputMaps"][mapNo];
        inputMap toReturn(map["keyboard"].GetBool());
        toReturn.useMouseAim = map["useMouseAim"].GetBool();
        Value& list = map["inputList"];
        for (SizeType i = 0; i < map.Size(); i++) {
            bool isAxis = list[i]["isAxis"].GetBool();
            bool positive = list[i]["positive"].GetBool();
            int id = list[i]["id"].GetInt();
            toReturn[i] = input(isAxis, positive, id);
        }
        return toReturn;
    }

    void configData::setInputMap(int mapNo, inputMap toWrite) {
        Value& map = json["inputMaps"][mapNo];
        map["keyboard"].SetBool(toWrite.keyboard);
        map["useMouseAim"].SetBool(toWrite.useMouseAim);
        Value& list = map["inputList"];
        for (SizeType i = 0; i < list.Size(); i++) {
            list[i]["isAxis"].SetBool(toWrite[i].isAxis);
            list[i]["positive"].SetBool(toWrite[i].positive);
            list[i]["id"].SetInt(toWrite[i].id);
        }
    }

    void configData::addInputMap(inputMap newMap) {
        Document::AllocatorType& a = json.GetAllocator();
        Value mapJson(kObjectType);
        mapJson.AddMember("keyboard", Value(newMap.keyboard).Move(), a);
        mapJson.AddMember("useMouseAim", Value(newMap.useMouseAim).Move(), a);
        mapJson.AddMember("inputList", Value(kArrayType).Move(), a);
        for (int i = 0; i < newMap.count(); i++) {
            Value newInput(kObjectType);
            newInput.AddMember("isAxis", Value(newMap[i].isAxis).Move(), a);
            newInput.AddMember("positive", Value(newMap[i].positive).Move(), a);
            newInput.AddMember("id", Value(newMap[i].id).Move(), a);
            mapJson["inputList"].PushBack(newInput.Move(), a);
        }
        json["inputMaps"].PushBack(mapJson.Move(), a);
    }

    void configData::deleteInputMap(int mapNo) {
        auto iterator = json["inputMaps"].Begin();
        for (int i = 0; i < mapNo; i++) {
            iterator++;
        }
        json["inputMaps"].Erase(iterator); //Might work, might not
    }

