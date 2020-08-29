#ifndef GAMELAYER_HPP
#define GAMELAYER_HPP

#include "layer.hpp"
#include "meta.hpp"
#include "world.hpp"

using namespace rapidjson;

struct colorEffect {
    int channel;
    Color tint;
    bool doLighting;
    bool doHighlight;
};

struct movementEffect {
    int channel;
    int type;
    bool x, y, z;
    int period;
    float value;
};

/******************************************************************************/
//gameLayer
//Implements layer functionality (e.g. animations) which the editor doesn't use.
/******************************************************************************/

class gameLayer : public layer {

    float offsetX = 0, offsetY = 0, offsetZ = 0;
    Color baseColor;
    bool baseDoLighting;
    bool baseDoHighlight;

    vector<colorEffect> colorEffects;
    vector<movementEffect> movementEffects;

    public:

    gameLayer(float newx, float newy, Color newTint, float newScale, string newFileName);

    gameLayer(gameLayer& other);

    ~gameLayer();

    unsigned int type();

    void setLighting(bool newDoLighting, bool newDoHighlight);

    void addColorEffect(int channel, Color tint, bool doLighting, bool doHighlight);

    void addMovementEffect(int channel, int type, bool x, bool y, bool z, int period, float value);

    void tickGet();

    void print();

};

#endif //GAMELAYER_HPP
