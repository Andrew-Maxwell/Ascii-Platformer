#ifndef GAME_MENU_HPP
#define GAME_MENU_HPP
#include "menu.hpp"
#include "screen.hpp"
#include "meta.hpp"
#include "configdata.hpp"
#include "input.hpp"

enum status{runStatus, pauseStatus, inventoryStatus, optionsStatus, inputOptionsStatus, singleLoadStatus, coopLoadStatus, deadStatus, doorStatus, saveStatus, menuStatus, quitStatus};

class gameMenu : public menu {

public:

    //Pre-game menus

    void main(int& status);

    string chooseSave(int& status, listData& saves);

    //In-game menus

    void pause(int& status);

    void inventory(int& status, outfit& o, inputMap& in);

    //Config menus

    void options(int& status, configData& config);

    Color editColor(Color oldColor);

    void editInputMap(int& status, inputMap& toEdit);

};

#endif //GAME_MENU_HPP
