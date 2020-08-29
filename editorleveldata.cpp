#include "editorleveldata.hpp"
#include <set>

/******************************************************************************/
//editorLevelData: Used to load level data for use by the editor code
/******************************************************************************/

    void editorLevelData::loadPalette(int* palette) {
        if (json.HasMember("editorPalette")) {
            for (SizeType i = 0; i < 44; i++) {
                palette[i] = json["editorPalette"][i].GetInt();
            }
        }
    }

    void editorLevelData::savePalette(int* palette) {
        if (json.HasMember("editorPalette")) {
            json["editorPalette"].Clear();
        }
        else {
            Value newPalette(kArrayType);
            json.AddMember("editorPalette", newPalette.Move(), json.GetAllocator());
        }
        for (int i = 0; i < 44; i++) {
            json["editorPalette"].PushBack(Value(palette[i]).Move(), json.GetAllocator());
        }
    }

    void editorLevelData::initializeEditor(list<editableLayer*>& layers, Color& background) {

        //Read level background color

        int fontSize = json.HasMember("fontSize") ? json["fontSize"].GetInt() : 16;
        string colFileName = json.HasMember("collider") ? json["collider"].GetString() : "test_collider.txt";
        loadedLayers.insert(colFileName);

        theScreen = new editableCanvas();
        Value dummyValue(kObjectType);
        editableCollider* col = new editableCollider(0.0, 0.0, WHITE, 1, true, colFileName, '!', dummyValue);
        theScreen -> setParams(col -> getRows(), col -> getCols(), fontSize, 16, 1.0f, getDayLength());
        initializeColors(theScreen);
        theScreen -> changeLighting();

        Value& entities = json["entities"];
        assert(entities.IsArray());

        for (SizeType i = 0; i < entities.Size(); i++) {

            Value& entity = entities[i];
            assert(entity.IsObject());

            //Read in generic data values (almost all entities will have these)

            string type = entity.HasMember("type") ? entity["type"].GetString() : "?";
            cout << "Reading entity " << type << endl;
            string fileName = entity.HasMember("fileName") ? entity["fileName"].GetString() : "No filename found.";

            float x = entity.HasMember("x") ? entity["x"].GetFloat() : 0.0;
            float y = entity.HasMember("y") ? entity["y"].GetFloat() : 0.0;
            if (type == "elevator") {
                int startingPoint = entity.HasMember("startingPoint") ? entity["startingPoint"].GetInt() : 0;
                x = entity["points"][startingPoint]["x"].GetFloat();
                y = entity["points"][startingPoint]["y"].GetFloat();
            }
            Color tint = entity.HasMember("tint") ? getColor(entity["tint"]) : WHITE;
            bool doLighting = entity.HasMember("doLighting") ? entity["doLighting"].GetBool() : true;
            float scale = entity.HasMember("scale") ? entity["scale"].GetFloat() : 1.0;
            int width = entity.HasMember("width") ? entity["width"].GetInt() : 1;
            int height = entity.HasMember("height") ? entity["height"].GetInt() : 1;
            editableLayer * L = new editableLayer(x, y, tint, scale, (type == "layer" || type == "elevator"), loadedLayers.find(fileName) == loadedLayers.end(), fileName, toupper(type[0]), width, height, entity, doLighting);
            if (type == "layer" || type == "elevator") {
                loadedLayers.insert(fileName);
            }
            layers.push_back(L);
        }

        //Want collider layer on top

        layers.push_back(col);

    }

    Value& editorLevelData::getNewEntity(float x, float y, float scale, int count) {
        Document::AllocatorType& a = json.GetAllocator();
        Value newEntity(kObjectType);
        char buffer[15];
        int len = sprintf(buffer, "%c placeholder", 'A' + count);
        Value type;
        type.SetString(buffer, len, a);
        newEntity.AddMember("type", type.Move(), a);
        newEntity.AddMember("x", Value(x).Move(), a);
        newEntity.AddMember("y", Value(y).Move(), a);
        newEntity.AddMember("tint", Value(kArrayType).Move(), a);
        newEntity["tint"].PushBack(Value(0).Move(), a);
        newEntity["tint"].PushBack(Value(255).Move(), a);
        newEntity["tint"].PushBack(Value(0).Move(), a);
        newEntity["tint"].PushBack(Value(255).Move(), a);
        newEntity.AddMember("width", Value(1).Move(), a);
        newEntity.AddMember("height", Value(1).Move(), a);
        newEntity.AddMember("scale", Value(scale).Move(), a);
        json["entities"].PushBack(newEntity, a);
        return json["entities"][json["entities"].Size() - 1];
    }

    void editorLevelData::writeEntities(list<editableLayer*>& layers, Color background) {
        cout << "Saving file...\n";
        
        //Save layer files
        
        Value& entities = json["entities"];
        assert(entities.IsArray());
        entities.Clear();
        Document::AllocatorType& a = json.GetAllocator();
        list<editableLayer*>::iterator saveIter = layers.begin();
        while (saveIter != layers.end()) {
            (*saveIter) -> save();
            Value entity((*saveIter) -> getJson(), a);
            if (entity.HasMember("type")) {        //if not the collider (The collider has an empty JSON)
                entities.PushBack(entity, a);
            }
            saveIter++;
        }
        cout << "Finished saving layers.\n";

        if (json.HasMember("dayBackground")) {
            setColor(json["dayBackground"], theScreen -> getColor(0));
        }
        if (json.HasMember("dawnBackground")) {
            setColor(json["dawnBackground"], theScreen -> getColor(1));
        }
        if (json.HasMember("nightBackground")) {
            setColor(json["nightBackground"], theScreen -> getColor(2));
        }
        if (json.HasMember("sunsetBackground")) {
            setColor(json["sunsetBackground"], theScreen -> getColor(3));
        }

        //Save entity metadata (to json)
        
        FILE* entityFile = fopen(fileName.c_str(), "wb");
        char buffer[65536];
        FileWriteStream entityWriteStream(entityFile, buffer, sizeof(buffer));
        PrettyWriter<FileWriteStream> writer(entityWriteStream);
        json.Accept(writer);
        fclose(entityFile);
    }
