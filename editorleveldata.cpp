#include "editorleveldata.hpp"
#include <set>

/******************************************************************************/
//editorLevelData: Used to load level data for use by the editor code
/******************************************************************************/

    void editorLevelData::initializeEditor(list<editableLayer*>& layers) {

        //Read level background color

        uint8_t backgroundR = json.HasMember("R") ? json["R"].GetInt() : 255;
        uint8_t backgroundG = json.HasMember("G") ? json["G"].GetInt() : 255;
        uint8_t backgroundB = json.HasMember("B") ? json["B"].GetInt() : 255;
        uint8_t backgroundA = json.HasMember("A") ? json["A"].GetInt() : 255;
        int fontSize = json.HasMember("fontSize") ? json["fontSize"].GetInt() : 16;
        string colFileName = json.HasMember("collider") ? json["collider"].GetString() : "test_collider.txt";
        loadedLayers.insert(colFileName);
        Color background = {backgroundR, backgroundG, backgroundB, backgroundA};

        theCanvas = new editableCanvas();
        editableCollider* col = new editableCollider(0.0, 0.0, {255 - backgroundR, 255 - backgroundG, 255 - backgroundB, 255}, 1, true, colFileName, '!', NULL);
        world = new collider();
        theCanvas -> setParams(col -> getRows(), col -> getCols(), background, fontSize, 1.0f);

        //Want collider layer on top

        world -> addEntity(col);
        layers.push_back(col);
    }

    void editorLevelData::readEntitiesEditor(list<editableLayer*>& layers) {

        Value& entities = json["entities"];
        assert(entities.IsArray());

        for (SizeType i = 0; i < entities.Size(); i++) {

            Value& entity = entities[i];
            assert(entity.IsObject());

            //Read in generic data values (almost all entities will have these)

            string type = entity.HasMember("type") ? entity["type"].GetString() : "?";
            cout << "Reading entity " << type << endl;
            string fileName = entity.HasMember("fileName") ? entity["fileName"].GetString() : "No filename found.";

            if (type != "layer" || loadedLayers.find(fileName) == loadedLayers.end()) {
                if (type == "layer") {
                    loadedLayers.insert(fileName);
                }
                float x = entity.HasMember("x") ? entity["x"].GetFloat() : 0.0;
                float y = entity.HasMember("y") ? entity["y"].GetFloat() : 0.0;
                int R = entity.HasMember("R") ? entity["R"].GetInt() : 0;
                int G = entity.HasMember("G") ? entity["G"].GetInt() : 0;
                int B = entity.HasMember("B") ? entity["B"].GetInt() : 0;
                int A = entity.HasMember("A") ? entity["A"].GetInt() : 0;
                float sizeFactor = entity.HasMember("sizeFactor") ? entity["sizeFactor"].GetFloat() : 1.0;
                int width = entity.HasMember("width") ? entity["width"].GetInt() : 1;
                int height = entity.HasMember("height") ? entity["height"].GetInt() : 1;
                editableLayer * L = new editableLayer(x, y, {R, G, B, A}, sizeFactor, (type == "layer"), fileName, toupper(type[0]), width, height, &entity);
                layers.push_back(L);
                world -> addEntity(L);
            }
        }
    }


    void editorLevelData::writeEntities(list<editableLayer*>& layers) {
        cout << "Saving file...\n";
        
        //Save layer files
        
        Value& entities = json["entities"];
        assert(entities.IsArray());
        list<editableLayer*>::iterator saveIter = layers.begin();
        while (saveIter != layers.end()) {
            (*saveIter) -> save();
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
