#include "world.cpp"
#include "raymath.h";

using namespace std;

string bw(bool x) {
    if (x) {
        return "yes";
    }
    return "no";
}

int main() {

    collider world;
    world.addRectangle(movingRectangle(1, 1, 2, 2, 2, 1, 3, 2, 0));
    bool hitX, hitY;
    Vector2 newPosition = world.go((Vector2){1, 1.5}, (Vector2){1, 1.5}, 0.5, 0.5, hitX, hitY, 1);
    cout << "hitX: " << bw(hitX) << "hitY: " << bw(hitY) << "(" << newPosition.x << ", " << newPosition.y << ")\n";

};
