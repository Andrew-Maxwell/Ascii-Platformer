#include <fstream>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>

using namespace std;
using namespace rapidjson;

int main(int argc, char** argv) {
    if (argc != 5) {
        cout << "Usage: [levelName] [layers] [rows] [columns]\n";
        exit(EXIT_FAILURE);
    }
    string levelName = argv[1];
    int layers = atoi(argv[2]);
    int rows = atoi(argv[3]);
    int cols = atoi(argv[4]);

    Document d;
    d.SetObject();
    Document::AllocatorType& a = d.GetAllocator();
    string colFileName = levelName + "_collider.lyr";
    d.AddMember("collider", Value().SetString(colFileName.c_str(), colFileName.length(), a), a);
    Value dayLight(kArrayType);
    for (int i = 0; i < 4; i++) {
        dayLight.PushBack(Value(255).Move(), a);
    }
    Value dayBackground(kArrayType);
    for (int i = 0; i < 4; i++) {
        dayBackground.PushBack(Value(255).Move(), a);
    }
    d.AddMember("dayLight", dayLight.Move(), a);
    d.AddMember("dayBackground", dayBackground.Move(), a);
    d.AddMember("dayLength", Value(18000).Move(), a);
    d.AddMember("fontSize", Value(16).Move(), a);

    Value entities;
    entities.SetArray();
    Value player(kObjectType);
    player.AddMember("type", Value("player").Move(), a);
    player.AddMember("x", Value(0).Move(), a);
    player.AddMember("y", Value(0).Move(), a);
    player.AddMember("R", Value(50).Move(), a);
    player.AddMember("G", Value(255).Move(), a);
    player.AddMember("B", Value(50).Move(), a);
    player.AddMember("A", Value(255).Move(), a);
    player.AddMember("scale", Value(1.0).Move(), a);
    entities.PushBack(player, a);
    for (int i = 0; i < layers; i++) {
        Value layer(kObjectType);
        layer.AddMember("type", Value("layer").Move(), a);
        layer.AddMember("x", Value(0).Move(), a);
        layer.AddMember("y", Value(0).Move(), a);
        layer.AddMember("R", Value(50).Move(), a);
        layer.AddMember("G", Value(255).Move(), a);
        layer.AddMember("B", Value(50).Move(), a);
        layer.AddMember("A", Value(255).Move(), a);
        layer.AddMember("scale", Value(1.0).Move(), a);
        string layerFileName = levelName + "_layer" + to_string(i) + ".lyr";
        layer.AddMember("fileName", Value().SetString(layerFileName.c_str(), layerFileName.length(), a), a);
        entities.PushBack(layer, a);
    }
    d.AddMember("entities", entities, a);

    FILE* out = fopen((levelName + ".json").c_str(), "wb");
    char buffer[65536];
    FileWriteStream ws(out, buffer, sizeof(buffer));
    PrettyWriter<FileWriteStream> writer(ws);
    d.Accept(writer);
    fclose(out);

    for (int i = 0; i < layers; i++) {
        ofstream layerFileOut;
        layerFileOut.open(levelName + "_layer" + to_string(i) + ".lyr");
        for (int rowNo = 0; rowNo < rows; rowNo++) {
            for (int colNo = 0; colNo < cols; colNo++) {
                layerFileOut << ' ';
            }
            layerFileOut << endl;
        }
        layerFileOut.close();
    }

    ofstream colliderFileOut;
    colliderFileOut.open(levelName + "_collider.lyr");
    for (int rowNo = 0; rowNo < rows; rowNo++) {
        for (int colNo = 0; colNo < cols; colNo++) {
            colliderFileOut << '.';
        }
        colliderFileOut << endl;
    }
    colliderFileOut.close();
}
