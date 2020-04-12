#ifndef EDITORLEVELDATA_HPP
#define EDITORLEVELDATA_HPP

#include "data.hpp"
#include "meta.hpp"
#include "layer.hpp"
#include "editables.hpp"
#include "canvas.hpp"
#include "world.hpp"

class editorLevelData : public data {

    set<string> loadedLayers;

    public:

    //Initialize global world and theCanvas pointers and read in editables

    void initializeEditor(list<editableLayer*>& layers);

    void addEntity(float x, float y, float sizeFactor);

    //Save changes to entities

    void writeEntities(list<editableLayer*>& layers);

};

#endif //EDITORLEVELDATA_HPP
