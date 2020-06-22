#include "layer.hpp"

/******************************************************************************/
//layer
//Static multi-character entities read in from file.
//Designed to be the visual part of the level.
/******************************************************************************/

    layer::layer( float newx, float newy, Color newTint, float newSizeFactor, string newFileName) :
        entity(newx, newy, newTint, newSizeFactor),
        fileName(newFileName) {}

    layer::~layer() {
        UnloadRenderTexture(tex);
    }

    unsigned int layer::type() {
        return LAYERTYPE;
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

    void layer::render() {
        BeginTextureMode(tex);
        ClearBackground((Color){0, 0, 0, 0});
        for (int i = 0; i < canvas.size(); i++) {
            theCanvas -> myDrawText(canvas[i].c_str(), (Vector2){0, i * 8}, 8, 0, (Color){255, 255, 255, 255});
        }
        EndTextureMode();
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

    void layer::print() {
        theCanvas -> drawLayer(x, y, tint, sizeFactor, tex.texture, doLighting);
//        for (int i = 0; i < canvas.size(); i++) {
//            theCanvas -> draw(x, y + i, tint, sizeFactor, canvas[i]);
//        }
//        Vector2 camera = theCanvas -> getCamera();
//        int iMin = max(int(camera.y - y - theCanvas -> getScreenRows() / sizeFactor / 2), 0);
//        for (int i = iMin; i < min((int)(camera.y - y + theCanvas -> getScreenRows() / 2.0 / sizeFactor) + 1, (int)canvas.size()); i++) {
//            theCanvas -> draw(x, y + i, tint, sizeFactor, canvas[i]);
//        }
//        theCanvas -> drawLayer(x, y, tint, sizeFactor, textTexture);
    }

