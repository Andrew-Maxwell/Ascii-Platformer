#ifndef CONFIGDATA_HPP
#define CONFIGDATA_HPP
#include "meta.hpp"
#include "data.hpp"
#include "input.hpp"

class configData : public data {

    public:

    configData(string fileName);

    bool getFullscreen();

    void setFullScreen(bool fullscreen);

    int getGameFontSize();

    void setGameFontSize(int size);

    int getHudFontSize();

    void setHudFontSize(int size);

    int inputMapCount();

    inputMap getInputMap(int mapNo);

    void setInputMap(int mapNo, inputMap newMap);

    void addInputMap(inputMap newMap);

    void deleteInputMap(int mapNo);
};

#endif //CONFIGDATA_HPP
