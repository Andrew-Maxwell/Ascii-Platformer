#include <raylib.h>

using namespace std;

int main() {
    int ticks = 0;
    InitWindow(1605, 400, "Medwindow");
    SetTargetFPS(60);
    ToggleFullscreen();
    while (ticks++ < 60 && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }
}
