#ifndef GAME_MENU_HPP
#define GAME_MENU_HPP
#include "configdata.hpp"

enum status{runStatus, pauseStatus, inventoryStatus, optionsStatus, keybindStatus, deadStatus, doorStatus, saveStatus, menuStatus, quitStatus};

class gameMenu {

    Vector2 oldMouse = GetMousePosition();
    int select = 0;
    bool mouseMode = false;
    int itemCount;
    //Measure amount of time a key was pressed for scrolling
    int upDown = 0, downDown = 0;

    void handleInput();

    bool button(int position, string text);

public:

    void pause(int& status);

    void main(int& status);

    void options(int& status, configData& config);

    void keyOptions(int& status, configData& config);

};

#endif //GAME_MENU_HPP
