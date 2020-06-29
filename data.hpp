#ifndef DATA_HPP
#define DATA_HPP

#include "layer.hpp"
#include "screen.hpp"
#include "editables.hpp"
#include "player.hpp"

using namespace rapidjson;

class data {

    protected:

    string fileName = "";
    Document json;

    public:

    data();

    bool load(string fileName);

    void save();

    string getFileName();

    Color getColor(const Value& tintJson);
};

class levelData : virtual public data {

    public:

    void initializeColors(screen* toInit);

    int getDayLength();

};

class outfitData : virtual public data {

    public:

    outfit getOutfit(string name);

};

#endif //DATA_HPP
