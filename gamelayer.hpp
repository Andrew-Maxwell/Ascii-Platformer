#ifndef GAMELAYER_HPP
#define GAMELAYER_HPP

#include "col.hpp"
#include "layer.hpp"
#include "meta.hpp"

using namespace rapidjson;

/******************************************************************************/
/*gameLayer
Implements layer functionality (e.g. animations) which the editor doesn't use.*/
/******************************************************************************/

class gameLayer : public layer {
    
    public:
    
    gameLayer(float newx, float newy, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, string newFileName);
  
};

#endif //GAMELAYER_HPP
