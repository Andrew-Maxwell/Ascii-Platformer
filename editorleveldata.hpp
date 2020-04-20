#ifndef EDITORLEVELDATA_HPP
#define EDITORLEVELDATA_HPP

#include "data.hpp"
#include "meta.hpp"
#include "layer.hpp"
#include "editables.hpp"
#include "canvas.hpp"

class editorLevelData : public data {

    set<string> loadedLayers;

    public:

    //Initialize global theCanvas and read in editables

    void initializeEditor(list<editableLayer*>& layers);

    Value& getNewEntity(float x, float y, float sizeFactor, int count);

    //Save changes to entities

    void writeEntities(list<editableLayer*>& layers);

};

#endif //EDITORLEVELDATA_HPP
