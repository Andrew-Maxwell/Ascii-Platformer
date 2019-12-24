#include "meta.hpp"

#ifndef EDITOR_CLASSES_H
#define EDITOR_CLASSES_H

#define CAMERASPEED 1

using namespace std;

/*****************************************************************************/
//charFill
//Brush defines shape, charFill defines "color": what characters the brush area should be
//filled with.
/*****************************************************************************/

struct charFill {

    int codepoint;

    charFill(int newCodepoint) {
        codepoint = newCodepoint;
    }

    int get(int x, int y) {
        return codepoint;
    }
};

struct randomCharFill : public charFill {

    vector<int> codepoints;

    int get(int x, int y) {
        return codepoints[GetRandomValue(0, codepoints.size() - 1)];
    }
};

/*****************************************************************************/
//dummyEntity
//An entity that doesn't do anything. Used to display locations of actual entities in the editor.
/*****************************************************************************/

class dummyEntity : public entity {

    char toPrint[2] = " ";

    public:

    dummyEntity(  float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                    float newSizeFactor, char newC);

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};

/*****************************************************************************/
//editableLayer
//A layer designed to be modified, then mayNeedToSave.
/*****************************************************************************/

class editableLayer : public layer {

    int flashCount = -1;
    bool visible = true;
    Color original;
    vector<vector<int*>> frames;
    int width, knownWidth;
    int tileX1, tileY1, tileX2, tileY2;
    int currentFrame = 0, lastFrame = 0;

    public:

    editableLayer( float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, string newFileName);

    //Accessors

    float getX();

    float getY();

    float getSizeFactor();

    Color getColor();

    void setColor(Color newColor);

    //Propogate changes from intCanvas to the ordinary canvas (which is only used for display.)

    void update();

    //Set the layer to be displayed in red for a brief period.

    void flash();

    //Toggle hiding.

    void toggleHide();

    //Determine which tile a given mouse position on the screen is in.

    float getTileX(float cameraX, float mouseX);

    float getTileY(float cameraY, float mouseY);

    //Apply changes using left mouse button.

    void leftBrush(vector<tuple<int, int>> mousePos, int brushID, charFill F);

    //Select a tile to the palette (using right mouse button.

    int sample(int tileX, int tileY);

    //Undo and redo

    void undo();

    void redo();

    //Save

    void save();

    //Display

    void print(float cameraX, float cameraY, Font displayFont);
};

#endif //EDITOR_CLASSES_H
