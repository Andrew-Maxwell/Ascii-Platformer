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

    Document json;

    FILE* A = fopen("testa.json", "r");
    fseek (A, 0, SEEK_END);
    char * buffer = new char[ftell (A)];
    fseek (A, 0, SEEK_SET);
    FileReadStream AReadStream(A, buffer, sizeof(buffer));
    if (json.ParseStream(AReadStream).HasParseError()) {
        cerr << "Error parsing json.\n" << endl;
    }
    fclose(A);
    free(buffer);

    if (json.RemoveMember("a")) {
        cout << "removed a\n";
    }
    else {
        cout << "Did not remove a\n";
    }
    if (json.RemoveMember("b")) {
        cout << "removed b\n";
    }
    else {
        cout << "Did not remove b\n";
    }

    FILE* B = fopen("testb.json", "r");
    fseek (B, 0, SEEK_END);
    char * buffer2 = new char[ftell (B)];
    fseek (B, 0, SEEK_SET);
    FileReadStream BReadStream(B, buffer2, sizeof(buffer2));
    if (json.ParseStream(BReadStream).HasParseError()) {
        cerr << "Error parsing json.\n" << endl;
    }
    fclose(B);
    free(buffer2);

    string valueName = "b";
    json[valueName.c_str()] = 300;
    string valueName2 = "c";
    Value label(valueName2.c_str(), json.GetAllocator());
    json.AddMember(label.Move(), Value(400).Move(), json.GetAllocator());


    FILE* out = fopen("testc.json", "wb");
    char buffer3[65536];
    FileWriteStream ws(out, buffer3, sizeof(buffer));
    PrettyWriter<FileWriteStream> writer(ws);
    json.Accept(writer);
    fclose(out);

}
