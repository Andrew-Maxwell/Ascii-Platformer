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
#include "canvas.hpp"
#include "world.hpp"
#include "enemy.hpp"
#include "newtestentity.hpp"

class gameLevelData : public outfitData {

    list<layer*> layerCache;

    public:

    void generateLayerCache();

    //Initialize global world and theCanvas pointers

    Color getBackgroundColor();

    string getWorldFileName();

    int getFontSize();

    //Read all of the entities into the global world collider

   void readEntitiesGame(set<int> collectedPickups, player* playerPtr);

    Vector2 getPlayerPosition();

};

#endif //DATA_HPP
