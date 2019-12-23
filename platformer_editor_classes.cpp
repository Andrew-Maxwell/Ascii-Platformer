#include "platformer.h"

using namespace std;

class dummyEntity : entity {

    char toPrint[] = " ";

    public:

    dummyEntity(  float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                    float newSizeFactor, char newC) :
        entity(newX, newY, R, G, B, A, newSizeFactor) {
            toPrint[0] = c;
        }

    void tickSet(collider& col) {}

    void tickGet(collider& col) {}

    bool finalize() {return false;}

    void print(float cameraX, float cameraY, Font displayFont) {
        DrawTextEx(displayFont, toPrint, (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS $
    }
}

class editableLayer : layer {

    public:

    editableLayer( float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, string newFileName) :
        layer(newX, newY, R, G, B, A, newSizeFactor, newFileName) {}

    void print(float cameraX, float cameraY, Font displayFont) {
        layer::print(cameraX, cameraY, displayFont);

}
