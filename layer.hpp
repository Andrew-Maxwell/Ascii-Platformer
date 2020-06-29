#ifndef LAYER_HPP
#define LAYER_HPP

#include "entity.hpp"
#include "meta.hpp"
#include "screen.hpp"

using namespace rapidjson;

//forward declaration

class collider;

/******************************************************************************/
//layer
//Visually large Multi-character entities read in from file.
//Designed to be the visual part of the level.
/******************************************************************************/

class layer : virtual public entity {

    protected:

    vector<string> screen;
    string fileName;
    RenderTexture2D tex;

    public:

    explicit layer( float newx, float newy, Color newTint, float newSizeFactor,
                    string newFileName);

    ~layer();

    unsigned int type();

    bool readLayer();

    void render();

    //accessors

    int getRows();

    int getCols();

    //junk

    void tickSet();

    void tickGet();

    bool finalize();

    //Active ingredient

    void print();
};

#endif //LAYER_HPP
