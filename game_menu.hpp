#ifndef GAME_MENU_HPP
#define GAME_MENU_HPP

enum status{run, pause, showInventory, options, breakDead, breakDoor, breakSave, breakMenu,  breakQuit};

void pauseMenu(int& status);

void mainMenu(int& status);

void optionsMenu(int& status, int previousStatus);

#endif //GAME_MENU_HPP
