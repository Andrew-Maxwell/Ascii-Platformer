#ifndef GAMELEVELDATA_HPP
#define GAMELEVELDATA_HPP

#include "data.hpp"
#include "bullet.hpp"
#include "door.hpp"
#include "effects.hpp"
#include "endinggate.hpp"
#include "forcefield.hpp"
#include "gamelayer.hpp"
#include "layer.hpp"
#include "meta.hpp"
#include "particles.hpp"
#include "pickups.hpp"
#include "player.hpp"
#include "rain.hpp"
#include "savepoint.hpp"
#include "water.hpp"
#include "screen.hpp"
#include "world.hpp"
#include "enemy.hpp"
#include "newtestentity.hpp"

class gameLevelData : public outfitData, public levelData {

    list<gameLayer*> layerCache;

    public:

    void generateLayerCache();

    //Initialize global world and theScreen pointers

    string getWorldFileName();

    int getFontSize();

    //Read all of the entities into the global world collider

    void readEntitiesGame(vector<player*>& players, bool reloadPlayers, set<int>* collectedPickups);

};

#endif //DATA_HPP
