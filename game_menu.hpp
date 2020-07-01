#ifndef GAME_MENU_HPP
#define GAME_MENU_HPP
#include "configdata.hpp"

enum status{runStatus, pauseStatus, inventoryStatus, optionsStatus, inputOptionsStatus, keyboardOptionsStatus, gamepadOptionsStatus, deadStatus, doorStatus, saveStatus, menuStatus, quitStatus};

class gameMenu {

    Vector2 oldMouse = GetMousePosition();
    int tickNo;
    int xCount, yCount;
    int xSelect = 0, ySelect = 0;
    bool mouseMode = false, firstTick;
    //Measure amount of time a key was pressed for scrolling
    int upDown = 0, downDown = 0, leftDown = 0, rightDown = 0;

    void init(int newXCount, int newYCount);

    void handleInput();

    void handleInput(inputMap& in);

    bool button(string text, int buttonCol, int buttonRow, int x = 1, int y = -1);

public:

    bool goBack();

    void inventory(int& status, outfit& o, inputMap& in);

    void pause(int& status);

    void main(int& status);

    void options(int& status, configData& config);

    void inputOptions(int& status, configData& config);

};

#endif //GAME_MENU_HPP
