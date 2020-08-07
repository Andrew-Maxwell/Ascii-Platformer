#include "gamelayer.hpp"

/******************************************************************************/
/*gameLayer
Implements layer functionality (e.g. animations) which the editor doesn't use.*/
/******************************************************************************/

    gameLayer::gameLayer( float newx, float newy, Color newTint, float newScale, string newFileName) :
        entity(newx, newy, newTint, newScale),
        layer(newx, newy, newTint, newScale, newFileName) {
            layer::readLayer();
            tex = LoadRenderTexture(getCols() * 8, getRows() * 8);
            layer::render();
        }

    gameLayer::gameLayer(gameLayer& other) :
        entity(other.x, other.y, other.tint, other.scale),
        layer(other.x, other.y, other.tint, other.scale, other.fileName) {
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
