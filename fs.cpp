#include <raylib.h>

using namespace std;

int main() {
    InitWindow(1280, 720, "Fullscreen Test");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RED);
        EndDrawing();
        if (IsKeyPressed(KEY_SPACE)) {
            ToggleFullscreen();
        }
    }
}
