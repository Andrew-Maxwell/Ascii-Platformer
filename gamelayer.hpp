#ifndef GAMELAYER_HPP
#define GAMELAYER_HPP


#include "layer.hpp"
#include "meta.hpp"

using namespace rapidjson;

/******************************************************************************/
/*gameLayer
Implements layer functionality (e.g. animations) which the editor doesn't use.*/
/******************************************************************************/

class gameLayer : public layer {
    
    public:
    
    gameLayer(float newx, float newy, Color newTint, float newSizeFactor, string newFileName);

    unsigned int type();

};

#endif //GAMELAYER_HPP
