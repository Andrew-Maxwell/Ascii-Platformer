#ifndef EDITORLEVELDATA_HPP
#define EDITORLEVELDATA_HPP

#include "data.hpp"
#include "meta.hpp"
#include "layer.hpp"
#include "editables.hpp"
#include "screen.hpp"

class editorLevelData : public levelData {

    set<string> loadedLayers;

    public:

    void loadPalette(int* palette);

    void savePalette(int* palette);

    //Initialize global theScreen and read in editables

    void initializeEditor(list<editableLayer*>& layers, Color& background);

    Value& getNewEntity(float x, float y, float scale, int count);

    //Save changes to entities

    void writeEntities(list<editableLayer*>& layers, Color background);

};

#endif //EDITORLEVELDATA_HPP
