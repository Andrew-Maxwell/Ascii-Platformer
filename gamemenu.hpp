#ifndef GAME_MENU_HPP
#define GAME_MENU_HPP
#include "menu.hpp"
#include "screen.hpp"
#include "meta.hpp"
#include "configdata.hpp"
#include "input.hpp"
#include "savedata.hpp"
#include "gameleveldata.hpp"

enum status{runStatus, pauseStatus, inventoryStatus, optionsStatus, inputOptionsStatus, loadStatus, readyRoomStatus, deadStatus, doorStatus, saveStatus, menuStatus, quitStatus};
enum gameModes{singlePlayerMode, coopMode, arenaMode};

class gameMenu : public menu {

public:

    //Pre-game menus

    void main(int& status, int& mode);

    string chooseSave(int& status, int mode);

    vector<playerConfig> readyRoom(saveData& save, bool loadedSave, gameLevelData& level, configData& config);

    //In-game menus

    void pause(int& status);

    void inventory(int& status, outfit& o, inputMap& in);

    //Config menus

    void options(int& status, configData& config);

    Color editColor(Color oldColor);

    void editInputMap(int& status, inputMap& toEdit);

};

#endif //GAME_MENU_HPP
