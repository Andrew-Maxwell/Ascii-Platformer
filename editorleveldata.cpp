#include "editorleveldata.hpp"
#include <set>

/******************************************************************************/
//editorLevelData: Used to load level data for use by the editor code
/******************************************************************************/

    void editorLevelData::initializeEditor(list<editableLayer*>& layers, Color& background) {

        //Read level background color

        int fontSize = json.HasMember("fontSize") ? json["fontSize"].GetInt() : 16;
        string colFileName = json.HasMember("collider") ? json["collider"].GetString() : "test_collider.txt";
        loadedLayers.insert(colFileName);

        theCanvas = new editableCanvas();
        Value dummyValue(kObjectType);
        editableCollider* col = new editableCollider(0.0, 0.0, WHITE, 1, true, colFileName, '!', dummyValue);
        theCanvas -> setParams(col -> getRows(), col -> getCols(), fontSize, 1.0f, getDayLength());
        initializeColors(theCanvas);

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
            Color tint = entity.HasMember("tint") ? getColor(entity["tint"]) : WHITE;
            float sizeFactor = entity.HasMember("sizeFactor") ? entity["sizeFactor"].GetFloat() : 1.0;
            int width = entity.HasMember("width") ? entity["width"].GetInt() : 1;
            int height = entity.HasMember("height") ? entity["height"].GetInt() : 1;
            editableLayer * L = new editableLayer(x, y, tint, sizeFactor, (type == "layer"), loadedLayers.find(fileName) == loadedLayers.end(), fileName, toupper(type[0]), width, height, entity);
            if (type == "layer") {
                loadedLayers.insert(fileName);
            }
            layers.push_back(L);
        }

        //Want collider layer on top

        layers.push_back(col);

    }

    Value& editorLevelData::getNewEntity(float x, float y, float sizeFactor, int count) {
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
        newEntity.AddMember("sizeFactor", Value(sizeFactor).Move(), a);
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
            if (entity.HasMember("x")) {        //if not the collider (The collider has an empty JSON)
                entities.PushBack(entity, a);
            }
            saveIter++;
        }
        cout << "Finished saving layers.\n";

        //Save entity metadata (to json)
        
        FILE* entityFile = fopen(fileName.c_str(), "wb");
        char buffer[65536];
        FileWriteStream entityWriteStream(entityFile, buffer, sizeof(buffer));
        PrettyWriter<FileWriteStream> writer(entityWriteStream);
        json.Accept(writer);
        fclose(entityFile);
    }
