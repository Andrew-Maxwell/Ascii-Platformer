#include "meta.hpp"

#ifndef CANVAS_HPP
#define CANVAS_HPP

#define NUMCHARS 879
#define FONTNAME "PxPlus_IBM_BIOS2.ttf"

class screen {

    protected:

    int fontSize = 16;
    int hudFontSize = 16;
    int screenRows, screenCols, cameraLagX, cameraLagY;

    Color background;
    Color light;
    Color dayLight = WHITE, sunsetLight = WHITE, nightLight = WHITE, dawnLight = WHITE;
    Color dayBackground = WHITE, sunsetBackground = WHITE, nightBackground = WHITE, dawnBackground = WHITE;
    int dayLength = 2880;

    int worldRows, worldCols;
    float playerSizeFactor;

    float cameraX, cameraY;
    bool moveCameraX, moveCameraY;
    Font displayFont;

    public:

    screen();

    void setParams(int newWorldRows, int newWorldCols, int newGameFontSize, int newHudFontSize, float newPlayerSizeFactor, int newDayLength);

    Color getColor();

    void setColor(Color newColor);

    void setAllColors(Color newDayLight, Color newSunsetLight, Color newNightLight, Color newDawnLight, Color newDayBackground, Color newSunsetBackground, Color newNightBackground, Color newDawnBackground);

    //Raylib functions I optimized to run faster in my use case
    //Not really necessary anymore

    int myGetGlyphIndex(int codepoint);

    void myDrawTexture(Texture2D& texture, Rectangle& sourceRec, Rectangle& destRec, Vector2 origin, float rotation, Color tint);

    void myDrawText(const char *text, Vector2 position, float fontSize, float spacing, Color tint);

    //Lighting/time-of-day functions

    float dayLevel();

    float sunsetLevel();

    float nightLevel();

    float dawnLevel();

    void calculateLighting();

    virtual void changeLighting() {}

    Color lighting(Color original);

    //Transition function

    void checkerboardTransition(int amountBlack);

    //Drawing functions

    void start(float playerX, float playerY, bool tabScreen);

    void start(bool tabScreen);

    void draw(float x, float y, Color tint, float scale, string text, bool doLight = true);

    void drawHud(float x, float y, Color tint, string text);

    void drawWithBackground(float x, float y, Color tint, Color background, float scale, string text, bool doLight = true);

    void drawHudWithBackground(float x, float y, Color tint, Color background, string text);
    //Used by button in the menus
    void drawHudWithBrackets(float x, float y, string text, bool selected);

    void drawScaleTest(float x, float y, Color tint, string text);

    void drawLayer(float x, float y, Color tint, float scale, Texture2D& t, bool doLight = true);

    void drawHudLayer(float x, float y, Color tint, Texture2D& t);

    virtual void drawLayerEditor(float x, float y, Color tint, float scale, Texture2D& t, bool selected, bool doLight = true) {}

    //Bars (world)

    void drawBarLeft(float x, float y, Color tint, float scale, float length, bool doLight = true);

    void drawBarRight(float x, float y, Color tint, float scale, float length, bool doLight = true);

    void drawBarDown(float x, float y, Color tint, float scale, float length, bool doLight = true);

    void drawBarUp(float x, float y, Color tint, float scale, float length, bool doLight = true);

    void drawBox(float x, float y, float width, float height, Color tint, float scale, bool doLight = true);

    //Bars (HUD)

    void drawHudBarLeft(float x, float y, Color tint, float length);

    void drawHudBarRight(float x, float y, Color tint, float length);

    void drawHudBarUp(float x, float y, Color tint, float length);

    void drawHudBarDown(float x, float y, Color tint, float length);

    void drawHudBox(float x, float y, float width, float height, Color tint);

    void end();

    virtual void moveCamera() {}

    //Accessors (mostly used by layer functions so that they don't print offscreen)

    Vector2 getCamera();

    Vector2 getMouseRelativeTo(float x, float y, float scale);

    int getScreenRows();

    int getScreenCols();

    int getFontSize();

    int getHudRows();

    int getHudCols();

    int getHudFontSize();

    void tweakHudScale();

    void tweakGameScale();

    int textLength(string text);

};

class editableCanvas : public screen {

    float speedMult, oldCameraX, oldCameraY, oldMouseX, oldMouseY;
    float cameraSpeed = 1;
    int lightingSelection;

    public:

    explicit editableCanvas();

    void changeLighting();

    void moveCamera();

    void drawLayerEditor(float x, float y, Color tint, float scale, Texture2D& t, bool selected, bool doLight = true);

};

#endif //CANVAS_HPP
