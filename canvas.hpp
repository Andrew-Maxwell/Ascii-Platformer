#include "meta.hpp"

#ifndef CANVAS_HPP
#define CANVAS_HPP

#define NUMCHARS 875
#define FONTNAME "PxPlus_IBM_BIOS2.ttf"

class canvas {

    protected:

    int screenWidth = SCREENWIDTH;
    int screenHeight = SCREENHEIGHT;
    int fontSize = 16;
    int screenRows, screenCols, cameraLagX, cameraLagY;

    Color background;
    int worldRows, worldCols;
    float playerSizeFactor;

    float cameraX, cameraY;
    bool moveCameraX, moveCameraY;
    Font displayFont;

    //Raylib functions I optimized to run faster in my use case

    public:

    canvas();

    void setParams(int newWorldRows, int newWorldCols, Color newBackground, int newFontSize, float newPlayerSizeFactor);

    int myGetGlyphIndex(int codepoint);

    void myDrawTexture(Texture2D& texture, Rectangle& sourceRec, Rectangle& destRec, Vector2 origin, float rotation, Color tint);

    void myDrawText(const char *text, Vector2 position, float fontSize, float spacing, Color tint);

    //Drawing functions

    void start(float playerX, float playerY, bool tabScreen);

    void start(bool tabScreen);

    void draw(float x, float y, Color tint, float sizeFactor, string text);

    void drawLayer(float x, float y, Color tint, float sizeFactor, Texture2D& t);

    void drawHud(float x, float y, Color tint, string text);

    void drawBarLeft(float x, float y, Color tint, float sizeFactor, float length);

    void drawBarRight(float x, float y, Color tint, float sizeFactor, float length);

    void drawBarDown(float x, float y, Color tint, float sizeFactor, float length);

    void drawBarUp(float x, float y, Color tint, float sizeFactor, float length);

    void drawHudBarLeft(float x, float y, Color tint, float length);

    void drawHudBarRight(float x, float y, Color tint, float length);

    void drawHudBarUp(float x, float y, Color tint, float length);

    void drawHudBarDown(float x, float y, Color tint, float length);

    void end();

    virtual void moveCamera() {}

    //Accessors (mostly used by layer functions so that they don't print offscreen)

    Vector2 getCamera();

    Vector2 getMouseRelativeTo(float x, float y, float sizeFactor);

    int getScreenRows();

    int getScreenCols();

    int getFontSize();

    int getHudRows();

    int getHudCols();

    int getHudFontSize();

};

class editableCanvas : public canvas {

    float speedMult, oldCameraX, oldCameraY, oldMouseX, oldMouseY;
    float cameraSpeed = 1;

    public:

    explicit editableCanvas(int newWorldRows, int newWorldCols, Color newBackground, int newFontSize, float newPlayerSizeFactor);

    void moveCamera();

};

#endif //CANVAS_HPP
