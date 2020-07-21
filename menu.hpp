#ifndef MENU_HPP
#define MENU_HPP
#include "screen.hpp"
#include "meta.hpp"
#include "input.hpp"

class menu {

public:

    Vector2 oldMouse = GetMousePosition();
    Vector2 scrollMouse;
    bool scrollClicked = false;
    bool locked = false;    //While true, ignore keyboard input

    bool firstCallToReleased;
    int xCount, yCount, pageRows;
    int scroll, oldScroll;
    int xSelect = 0, ySelect = 0;
    bool mouseMode = false, firstTick;
    //Measure amount of time a key was pressed for scrolling
    int upDown = 0, downDown = 0, leftDown = 0, rightDown = 0;
    int tickNo;

    bool onAnyGamepad(bool (*gamepadProperty)(int, int), int button);

    bool axisOnAnyGamepad(int axis1, int invert1, int axis2 = 0, int invert2 = 0);

    void handleKey (int keyOne, int keyTwo, int button1, int axis1, int axis2, int invertAxes, input in, int& keyDown, int& select, int itemCount, int direction);

    void init(int newXCount, int newYCount);

    bool mouseReleasedOnce();

    void handleInput(bool doScroll = true);

    void handleInput(inputMap& in, bool doScroll = true);

    void drawTitle(string title);

    void scrollBar();

    bool button(string text, int buttonCol, int buttonRow, string inBrackets = "", int x = 1, int y = -1);

//    int slider(int currentValue, int range, int length, int buttonCol, int buttonRow, int x = 1, int y = -1);

//    int miniMenu(vector<string> labels);

public:

    bool goBack();

};

//Designed to be displayed on top of another Menu while the other Menu is locked.

class inputBox : public menu {

    int width = 48, height = 7, x, y;
    int del = 0, delDown = 0;

    void handleInput();

public:

    inputBox();

    void display(string title, string& input);

    //Should be called after display();

    bool okay();

    bool cancel();

};

#endif //MENU_HPP
