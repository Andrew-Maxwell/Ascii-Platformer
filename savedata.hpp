#ifndef SAVEDATA_HPP
#define SAVEDATA_HPP

#include "data.hpp"
#include "player.hpp"

using namespace rapidjson;

class saveData : public outfitData {

    public:

    bool hasOutfit(string name);

    string getRoom();

    Vector2 getPosition();

    void writeOutfit(outfit o, int playerNumber);

    void save(Vector2 position, string room);
};

#endif //SAVEDATA_HPP
