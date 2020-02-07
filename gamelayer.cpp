#include "gamelayer.hpp"

/******************************************************************************/
/*gameLayer
Implements layer functionality (e.g. animations) which the editor doesn't use.*/
/******************************************************************************/

    gameLayer::gameLayer( float newx, float newy, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, string newFileName) :
        entity(newx, newy, R, G, B, A, newSizeFactor),
        layer(newx, newy, R, G, B, A, newSizeFactor, newFileName) {
 
            layer::readLayer();
        }
