#ifndef GAME_MENU_HPP
#define GAME_MENU_HPP
#include "configdata.hpp"

enum status{run, pause, showInventory, options, keyOptions, breakDead, breakDoor, breakSave, breakMenu,  breakQuit};

void pauseMenu(int& status);

void mainMenu(int& status);

void optionsMenu(int& status, configData& config);

void keyOptionsMenu(int& status, configData& config);

#endif //GAME_MENU_HPP
