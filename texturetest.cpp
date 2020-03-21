#include <raylib.h>
#include <iostream>

using namespace std;

class textureTester {

    RenderTexture2D renderTex;

    public:

    void init(string text) {
        renderTex = LoadRenderTexture(100, 100);
        BeginTextureMode(renderTex);
        ClearBackground((Color){0, 0, 0, 0});
        DrawText(text.c_str(), 0, 0, 10, BLACK);
        EndTextureMode();
    }

    textureTester(string text) {
        init(text);
    }

    void print() {
        DrawTexture(renderTex.texture, 300, 300, BLACK);
    }
};

int main() {

    InitWindow(1600, 900, "Texture Test");
    SetTargetFPS(60);

    textureTester t("Hello World!");

    while(!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(WHITE);
        t.print();
        EndDrawing();
    }
}
