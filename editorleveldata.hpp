#ifndef EDITORLEVELDATA_HPP
#define EDITORLEVELDATA_HPP

#include "data.hpp"
#include "meta.hpp"
#include "layer.hpp"
#include "editables.hpp"
#include "canvas.hpp"
#include "world.hpp"

class editorLevelData : public data {

    public:

    //Initialize global world and theCanvas pointers

    void initializeEditor(list<editableLayer*>& layers);

    //Read all of the entities in as editableLayers

    void readEntitiesEditor(list<editableLayer*>& layers);

    //Save changes to entities

    void writeEntities(list<editableLayer*>& layers);

};

#endif //EDITORLEVELDATA_HPP
