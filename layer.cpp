#include "layer.hpp"

/******************************************************************************/
//layer
//Static multi-character entities read in from file.
//Designed to be the visual part of the level.
/******************************************************************************/


/*
    explicit layer( float newx, float newy, uint8_t R, uint8_t G,
                    uint8_t B, uint8_t A, float newSizeFactor,
                    string newFileName) :
                    entity(newx, newy, R, G, B, A, newSizeFactor) {
        fileName = newFileName;
        readLayer();
    }
*/

    layer::layer( float newx, float newy, Color newTint, float newSizeFactor, string newFileName) :
        entity(newx, newy, newTint, newSizeFactor),
        fileName(newFileName) {
            type = LAYERTYPE;
        }

    bool layer::readLayer() {
        canvas.clear();
        string line;
        ifstream worldFile = getLevelIFStream(fileName);
        while (getline(worldFile, line)) {
            canvas.push_back(line);
        }
        worldFile.close();
        return true;
    }

    int layer::getRows() {
        return canvas.size();
    }

    int layer::getCols() {
        return canvas[0].length();
    }

    void layer::tickSet() {}

    void layer::tickGet() {}

    bool layer::finalize() {
        return false;
    }

    void layer::print(float cameraX, float cameraY, Font displayFont) {
        for (int i = max((int)(cameraY - y - SCREENROWS / sizeFactor / 2), 0); i < min((int)(cameraY - y + SCREENROWS / 2.0 / sizeFactor) + 1, (int)canvas.size()); i++) {
            myDrawText(displayFont, canvas[i].c_str(), (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y + i) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 0, tint);
        }
    }

