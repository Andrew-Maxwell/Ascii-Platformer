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

    charFill() {}

    charFill(int newCodepoint) {
        codepoint = newCodepoint;
    }

    virtual int get(int x, int y) {
        return codepoint;
    }
};

struct randomCharFill : public charFill {

    vector<int> codepoints;

    randomCharFill() {}

    randomCharFill(vector<int> newCodepoints) {
        codepoints = newCodepoints;
    }

    int get(int x, int y) override {
        return codepoints[GetRandomValue(0, codepoints.size() - 1)];
    }
};

struct pipeCharFill : public charFill {

    int get(int x, int y) {
        uint8_t up, down, left, right;
        up = (int)(sin((float)x * (y % 4)) * 1000) % 3;
        left = (int)(cos((float)y * (x % 5)) * 100) % 3;
        down = (int)(sin((float)x * ((y + 1) % 4)) * 1000) % 3;
        right = (int)(cos((float)y * ((x + 1) % 5)) * 100) % 3;

        return 0;
    }
};

struct gridCharFill : public charFill {

    int size, horiz, vert, cross, displayCount = 0;

    gridCharFill(int newSize, int newHoriz, int newVert, int newCross) :
        size(newSize),
        horiz(newHoriz),
        vert(newVert),
        cross(newCross) {}

    int get(int x, int y) {
        if (x == -1 && y == -1) {
            if (displayCount++ % 60 < 30) {
                return cross;
            }
            else {
                return 48 + size;
            }
        }
        else {
            if (x % size == 0) {
                if (y % size == 0) {
                    return cross;
                }
                return vert;
            }
            else if (y % size == 0) {
                return horiz;
            }
            else {
                return ' ';
            }
        }
    }
};

struct diagGridCharFill : public charFill {

    int size, upRight, downRight, cross, displayCount = 0;

    diagGridCharFill(int newSize, int newUpRight, int newDownRight, int newCross) :
        size(newSize),
        upRight(newUpRight),
        downRight(newDownRight),
        cross(newCross) {}

    int get(int x, int y) {
        if (x == -1 && y == -1) {
            if (displayCount++ % 60 < 30) {
                return cross;
            }
            else {
                return 48 + size;
            }
        }
        else {
            if ((x + y) % size == 0) {
                if ((x -y) % size == 0) {
                    return cross;
                }
                return upRight;
            }
            else if ((x - y) % size == 0) {
                return downRight;
            }
            else {
                return ' ';
            }
        }
    }
};

struct textureCharFill : public charFill {

    vector<vector<int>> texture;

    textureCharFill(vector<vector<int>> newTexture) :
        texture(newTexture) {}

    int get(int x, int y) {
        return texture[y % texture.size()][x % texture[0].size()];
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

    void leftBrush(vector<tuple<int, int>> mousePos, int brushID, charFill* F, float density);

    //Cut, copy, and paste

    vector<vector<int>> cut(vector<tuple<int, int>> mousePos);

    vector<vector<int>> copy(vector<tuple<int, int>> mousePos);

    void paste(vector<tuple<int, int>> mousePos, vector<vector<int>> toPaste);

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
