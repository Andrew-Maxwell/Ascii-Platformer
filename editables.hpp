#include "meta.hpp"
#include "entitymeta.hpp"
#include "layer.hpp"
#include "charfills.hpp"

#ifndef EDITABLES_HPP
#define EDITABLES_HPP

#define CAMERASPEED 1

/*****************************************************************************/
//editableLayer
//A layer designed to be modified, then mayNeedToSave.
/*****************************************************************************/

class editableLayer : public layer {

    protected:

    Value* json;
    int flashCount = -1;
    bool visible = true;
    bool isLayer;
    Color original;
    vector<vector<int*>> frames;
    int width, knownWidth;
    int tileX1, tileY1, tileX2, tileY2;
    int currentFrame = 0, lastFrame = 0;
    bool isColorChanged = false, selected = false;
    

    public:

    editableLayer( float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, bool newIsLayer, string newFileName, char display, Value* newJson);

    //Accessors
    
    bool getIsLayer();

    float getX();

    float getY();

    float getSizeFactor();
    
    virtual void setSizeFactor(float newSizeFactor);

    Color getColor();

    virtual void setColor(Color newColor);

    void select();

    void deselect();

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
    
    virtual void move(vector<tuple<int, int>> mousePos);

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

    virtual void save();

    //Display

    void print(float cameraX, float cameraY, Font displayFont);
};

/*****************************************************************************/
//editableCollider
//Like an editableLayer in every way, except that it never writes metadata.
/*****************************************************************************/

class editableCollider : virtual public editableLayer {


    public:

    editableCollider (float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, bool newIsLayer, string newFileName, char display, Value* dummyJson);

    //Dummy functions which don't do anything

    void setColor (Color newColor);

    void setSizeFactor (float newSizeFactor);

    void move (vector<tuple<int, int>> mousePos);

    //Reduced functionality.

    void save();

};

#endif //EDITABLES_HPP
