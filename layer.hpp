#ifndef LAYER_HPP
#define LAYER_HPP

#include "entity.hpp"
#include "meta.hpp"
#include "canvas.hpp"

using namespace rapidjson;

//forward declaration

class collider;

/******************************************************************************/
//layer
//Visually large Multi-character entities read in from file.
//Designed to be the visual part of the level.
/******************************************************************************/

class layer : virtual public entity {

    public:

    vector<string> canvas;
    
    string fileName;

    explicit layer( float newx, float newy, Color newTint, float newSizeFactor,
                    string newFileName);

    bool readLayer();

    int getRows();

    int getCols();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

#endif //LAYER_HPP
