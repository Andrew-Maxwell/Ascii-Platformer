#include <raylib.h>
#include <iostream>

using namespace std;

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "Fullscreen Test 2");
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RED);
        DrawText("Hullo thar", 0, 0, 16, BLACK);
        if (IsKeyPressed(KEY_ONE)) {
            SetWindowSize(640, 480);
        }
        else if (IsKeyPressed(KEY_TWO)) {
            SetWindowSize(1280, 720);
        }
        else if (IsKeyPressed(KEY_THREE)) {
            SetWindowSize(1920, 1080);
        }
        if (IsKeyPressed(KEY_SPACE)) {
            ToggleFullscreen();
        }
        if (IsKeyPressed(KEY_K)) {
            cout << GetMonitorWidth(0) << ", " << GetMonitorHeight(0) << endl;
        }
        EndDrawing();
    }
}
