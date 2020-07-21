#ifndef CONFIGDATA_HPP
#define CONFIGDATA_HPP
#include "meta.hpp"
#include "data.hpp"
#include "input.hpp"
#include "player.hpp"

struct conflict {
    int player1, player2;
    string error;
};

class configData : public data {

    public:

    //Constructor

    configData(string fileName, bool reset = false);

    //Display parameters

    bool getFullscreen();

    void setFullScreen(bool fullscreen);

    int getGameFontSize();

    void setGameFontSize(int size);

    int getHudFontSize();

    void setHudFontSize(int size);

    bool getAutoCamera();

    void setAutoCamera(bool autoCamera);

    //Player parameters

    playerConfig getPlayerConfig(int index);

    Color getConfigColor(int index);

    void setConfig(int index, playerConfig newConfig);

    vector<conflict> getConflicts();

};

#endif //CONFIGDATA_HPP
