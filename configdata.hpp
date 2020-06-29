#ifndef CONFIGDATA_HPP
#define CONFIGDATA_HPP
#include "meta.hpp"
#include "data.hpp"

class configData : public data {

    public:

    configData(string fileName);

    bool getFullscreen();

    void setFullScreen(bool fullscreen);

    int getGameFontSize();

    void setGameFontSize(int size);

    int getHudFontSize();

    void setHudFontSize(int size);

    keyMapping getKeys();

    void setKeys(keyMapping newKeys);

};

#endif //CONFIGDATA_HPP
