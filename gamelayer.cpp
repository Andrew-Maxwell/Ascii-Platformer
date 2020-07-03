#include "gamelayer.hpp"

/******************************************************************************/
/*gameLayer
Implements layer functionality (e.g. animations) which the editor doesn't use.*/
/******************************************************************************/

    gameLayer::gameLayer( float newx, float newy, Color newTint, float newSizeFactor, string newFileName) :
        entity(newx, newy, newTint, newSizeFactor),
        layer(newx, newy, newTint, newSizeFactor, newFileName) {
            layer::readLayer();
            tex = LoadRenderTexture(getCols() * 8, getRows() * 8);
            layer::render();
        }

    gameLayer::~gameLayer() {
        UnloadRenderTexture(tex);
        cout << "gameLayer destroyed\n";
    }

    unsigned int gameLayer::type() {
        return GAMELAYERTYPE;
    }
