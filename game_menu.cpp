#include "game_menu.hpp"
#include "canvas.hpp"
#include "meta.hpp"

    void pauseMenu(int& status) {
        theCanvas -> start(true);
        int width = theCanvas -> getHudCols();
        int height = theCanvas -> getHudRows();
        string pausedLabel = "Game Paused";
        string optionsLabel = "Game Options";
        string menuLabel = "Main Menu (lose unsaved progress)";
        string quitLabel = "Quit (lose unsaved progress)";
        string resumeLabel = "Resume";
        theCanvas -> drawHud((width - pausedLabel.size()) / 2, height / 2 - 6, UIFOREGROUND, pausedLabel);
        if (theCanvas -> button((width - optionsLabel.size()) / 2, height / 2 - 4, optionsLabel)) {
            status = options;
        }
        else if (theCanvas -> button((width - menuLabel.size()) / 2, height / 2 - 2, menuLabel)) {
            status = breakMenu;
        }
        else if (theCanvas -> button((width - quitLabel.size()) / 2, height / 2, quitLabel)) {
            status = breakQuit;
        }
        else if (theCanvas -> button((width - resumeLabel.size()) / 2, height / 2 + 2, resumeLabel)) {
            status = run;
        }
        theCanvas -> end();
    }

    void mainMenu(int& status) {
        theCanvas -> start(true);
        int width = theCanvas -> getHudCols();
        int height = theCanvas -> getHudRows();
        string title = "ASCII Platformer";
        string startLabel = "Load Game";
        string optionsLabel = "Options";
        string quitLabel = "Quit";
        theCanvas -> drawHud((width - title.size()) / 2, height / 2 - 4, UIFOREGROUND, title);
        if (theCanvas -> button((width - startLabel.size()) / 2, height / 2 - 2, startLabel)) {
            status = breakDead;
        }
        else if (theCanvas -> button((width - optionsLabel.size()) / 2, height / 2, optionsLabel)) {
            status = options;
        }
        else if (theCanvas -> button((width - quitLabel.size()) / 2, height / 2 + 2, quitLabel)) {
            status = breakQuit;
        }
        theCanvas -> end();
    }

    void optionsMenu(int& status, int previousStatus) {
        theCanvas -> start(true);
        int width = theCanvas -> getHudCols();
        int height = theCanvas -> getHudRows();
        string title = "Options";
        theCanvas -> drawHud((width - title.size()) / 2, height / 2 - 8, UIFOREGROUND, title);
        //Display settings
        string fullscreenLabel = "Toggle Fullscreen";
        if (theCanvas -> button((width - fullscreenLabel.size()) / 2, height / 2 - 6, fullscreenLabel)) {
            if (IsWindowFullscreen()) {
                ToggleFullscreen();
                SetWindowSize(800, 600);
                SetWindowMinSize(800, 600);
            }
            else {
                int width = GetMonitorWidth(0);
                int height = GetMonitorHeight(0);
                ToggleFullscreen();
                SetWindowSize(width, height);
            }
        }
        string hudScaleLabel = "Tweak Interface Scale";
        if (theCanvas -> button((width - hudScaleLabel.size()) / 2, height / 2 - 4, hudScaleLabel)) {
            theCanvas -> tweakHudScale();
        }
        string gameScaleLabel = "Tweak Game Scale";
        if (theCanvas -> button((width - gameScaleLabel.size()) / 2, height / 2 - 2, gameScaleLabel)) {
            theCanvas -> tweakGameScale();
        }
        string gameScaleTest = "Game Scale Text";
        theCanvas -> drawScaleTest((width - gameScaleTest.size()) / 2, height / 2 - 0, RED, gameScaleTest);
        //Keybind settings
        string backLabel = "Back to Menu";
        if (theCanvas -> button((width - backLabel.size()) / 2, height / 2 + 4, backLabel)) {
            status = previousStatus;
        }
        theCanvas -> end();
    }
