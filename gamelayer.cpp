#include "gamelayer.hpp"

/******************************************************************************/
/*gameLayer
Implements layer functionality (e.g. animations) which the editor doesn't use.*/
/******************************************************************************/

    gameLayer::gameLayer( float newx, float newy, Color newTint, float newSizeFactor, string newFileName) :
        entity(newx, newy, newTint, newSizeFactor),
        layer(newx, newy, newTint, newSizeFactor, newFileName) {
            type = LAYERTYPE;
            layer::readLayer();
        }
