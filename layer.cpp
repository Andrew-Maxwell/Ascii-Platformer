#include "layer.hpp"

/******************************************************************************/
//layer
//Static multi-character entities read in from file.
//Designed to be the visual part of the level.
/******************************************************************************/

    layer::layer( float newx, float newy, Color newTint, float newScale, string newFileName) :
        entity(newx, newy, newTint, newScale),
        fileName(newFileName) {}

    layer::~layer() {
        if (rendered) {
            UnloadRenderTexture(tex);
        }
        rendered = false;
    }

    unsigned int layer::type() {
        return LAYERTYPE;
    }

    bool layer::readLayer() {
        screen.clear();
        string line;
        ifstream worldFile = getLevelIFStream(fileName);
        while (getline(worldFile, line)) {
            screen.push_back(line);
        }
        worldFile.close();
        return true;
    }

    void layer::render() {
        if (!rendered) {
            BeginTextureMode(tex);
            ClearBackground((Color){0, 0, 0, 0});
            for (int i = 0; i < screen.size(); i++) {
                theScreen -> myDrawText(screen[i].c_str(), (Vector2){0, i * 8}, 8, 0, (Color){255, 255, 255, 255});
            }
            EndTextureMode();
            rendered = true;
        }
    }

    int layer::getRows() {
        return screen.size();
    }

    int layer::getCols() {
        return theScreen -> textLength(screen[0]);
    }

    void layer::tickSet() {}

    void layer::tickGet() {}

    bool layer::finalize() {
        return false;
    }

    void layer::print() {
        theScreen -> drawLayer(x, y, tint, scale, tex.texture, doLighting, doHighlight);
    }

