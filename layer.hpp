#ifndef LAYER_HPP
#define LAYER_HPP

#include "entitymeta.hpp"
#include "meta.hpp"

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

    explicit layer( float newx, float newy, uint8_t R, uint8_t G,
                    uint8_t B, uint8_t A, float newSizeFactor,
                    string newFileName);

    bool readLayer();

    int getRows();

    int getCols();

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};

#endif //LAYER_HPP
