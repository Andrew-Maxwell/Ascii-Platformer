#ifndef GAME_MENU_HPP
#define GAME_MENU_HPP
#include "configdata.hpp"

enum status{runStatus, pauseStatus, inventoryStatus, optionsStatus, inputOptionsStatus, loadStatus, deadStatus, doorStatus, saveStatus, menuStatus, quitStatus};

class gameMenu {

    Vector2 oldMouse = GetMousePosition();
    Vector2 scrollMouse;
    bool scrollClicked = false;

    bool firstCallToReleased;
    int xCount, yCount;
    int scroll, oldScroll;
    int xSelect = 0, ySelect = 0;
    bool mouseMode = false, firstTick;
    //Measure amount of time a key was pressed for scrolling
    int upDown = 0, downDown = 0, leftDown = 0, rightDown = 0;
    int tickNo;

    void init(int newXCount, int newYCount);

    bool mouseReleasedOnce();

    void handleInput();

    void handleInput(inputMap& in);

    void scrollBar();

    void inputBox(string title, string& input);

    bool button(string text, int buttonCol, int buttonRow, string inBrackets = "", int x = 1, int y = -1);

//    int slider(int currentValue, int range, int length, int buttonCol, int buttonRow, int x = 1, int y = -1);

//    int miniMenu(vector<string> labels);

public:

    bool goBack();

    string chooseSave(int& status, listData saves);

    Color editColor(Color oldColor);

    void inventory(int& status, outfit& o, inputMap& in);

    void pause(int& status);

    void main(int& status);

    void options(int& status, configData& config);

    void editInputMap(int& status, inputMap& toEdit);

};

#endif //GAME_MENU_HPP
