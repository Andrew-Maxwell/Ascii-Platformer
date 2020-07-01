#include <raylib.h>
#include "input.hpp"

using namespace std;

input getInput(int device) {
    input newInput;
    input undefined;
    if (device == -1) {
        for (int i = 32; i < 349; i++) {
            if (IsKeyPressed(i)) {
                if (newInput.isDefined()) { //Multiple keys pressed
                    return undefined;
                }
                else {
                    newInput = input(false, false, i);
                }
            }
        }
    }
    else {
        for (int i = 1; i <= 17; i++) {
            if (IsGamepadButtonPressed(device, i)) {
                if (newInput.isDefined()) { //Multiple buttons pressed
                    return undefined;
                }
                else {
                    newInput = input(false, false, i);
                }
            }
        }
        for (int i = 0; i < 4; i++) {   //TODO: Change to GetGamepadAxisCount (need workaround for broken fifth axis on older gamepads)
            float value = GetGamepadAxisMovement(device, i);
            if (value != 0) {
                if (newInput.isDefined()) { //Multiple inputs
                    return undefined;
                }
                else {
                    newInput = input(true, (value > 0), i);
                }
            }
        }
    }
    return newInput;
}

int main() {
    InitWindow(100, 100, "Gamepad test");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        input newInput = getInput(0);
        if (newInput.isDefined()) {
            cout << newInput.name(0) << endl;
        }
        BeginDrawing();
        ClearBackground(GREEN);
        DrawText("Hi!", 20, 30, 16, BLACK);
        EndDrawing();
    }
}
