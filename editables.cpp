#include "editables.hpp"

using namespace rapidjson;

/*****************************************************************************/
//editableLayer
//A layer designed to be modified, then mayNeedToSave. (if isLayer = true)
//Also used to represent other entities (isLayer = false.) Not all of the same editing functions apply.
/*****************************************************************************/

    editableLayer::editableLayer( float newX, float newY, Color newTint, float newScale, bool newIsLayer, bool newIsEditable, string newFileName, char newDisplay, int newDisplayWidth, int newDisplayHeight, Value& newJson, bool newDoLighting) :
        entity(newX, newY, newTint, newScale),
        layer(newX, newY, newTint, newScale, newFileName),
        displayWidth(newDisplayWidth),
        displayHeight(newDisplayHeight),
        isLayer(newIsLayer),
        isEditable(newIsEditable),
        doLighting(newDoLighting)
    {

        //Set original color to reset to after flashing

        display = newDisplay;
        tint = original = newTint;
        json = newJson;

        //If layer Read in screen information. Canvas stores the layer in UTF-8; intCanvas is in UTF-32 (?)
        //Anyway, intCanvas is much easier to edit, and changes are propogated to screen to display using update()
        //intCanvas is stored in "frames" for undoing/redoing. Complete state is saved in each frame.

        if (isLayer) {

            screen.clear();
            ifstream worldFile = getLevelIFStream(fileName);
            string line;
            getline(worldFile, line);
            screen.push_back(line);
            vector<int*> intCanvas;
            int* codepoints = GetCodepoints(line.c_str(), &knownWidth);
            int* newCodepoints = new int[knownWidth];
            for (int i = 0; i < knownWidth; i++) {
                newCodepoints[i] = codepoints[i];
            }
            intCanvas.push_back(newCodepoints);

            while (getline(worldFile, line)) {
                screen.push_back(line);
                vector<int> intLine;
                int* codepoints = GetCodepoints(line.c_str(), &width);
                if (width != knownWidth) {
                    cerr << "Error: width mismatch in file " << fileName << ", line" << intCanvas.size() + 1 << ". Make sure every line has the same number of utf-8 characters.";
                }
                int* newCodepoints = new int[width];
                for (int i = 0; i < width; i++) {
                    newCodepoints[i] = codepoints[i];
                }
                intCanvas.push_back(newCodepoints);
            }
            frames.push_back(intCanvas);
            worldFile.close();
        }

        else {
            for (int i = 0; i < displayHeight; i++) {
                screen.push_back(string(displayWidth, display));
            }
        }
        tex = LoadRenderTexture(getCols() * 8, getRows() * 8);
        render();
    }

    editableLayer::editableLayer(editableLayer& other) : 
        entity(other.x, other.y, other.tint, other.scale),
        layer(other.x, other.y, other.tint, other.scale, other.fileName) {
        display = other.display;
        json.CopyFrom(other.json, doc.GetAllocator());
        visible = true;
        isLayer = other.isLayer;
        original = other.original;
        frames = other.frames;
        width = other.width;
        knownWidth = other.knownWidth;
        currentFrame = other.currentFrame;
        lastFrame = other.lastFrame;
        isColorChanged = other.isColorChanged;
        selected = false;
        displayWidth = other.displayWidth;
        displayHeight = other.displayHeight;
        screen = other.screen;
        tex = LoadRenderTexture(getCols() * 8, getRows() * 8);
        render();
    }


/*****************************************************************************/
//Accessors
/*****************************************************************************/

    bool editableLayer::getIsEditable() {
        return isEditable && isLayer;
    }

    float editableLayer::getX() {
        return x;
    }

    float editableLayer::getY() {
        return y;
    }

    float editableLayer::getSizeFactor() {
        return scale;
    }

    void editableLayer::setSizeFactor(float newScale) {
        scale = newScale;
    }

    Color editableLayer::getColor() {
        return tint;
    }

    void editableLayer::setColor(Color newColor) {
        tint = newColor;
        original = newColor;
    }

    void editableLayer::select() {
        selected = true;
    }

    void editableLayer::deselect() {
        selected = false;
    }

    Value& editableLayer::getJson() {
        return json;
    }

/*****************************************************************************/
//update()
//Propogate changes from intCanvas to the ordinary screen (which is only used for display.)
/*****************************************************************************/

    void editableLayer::update() {
        screen.clear();
        for (int i = 0; i < frames[currentFrame].size(); i++) {
            char* lineC = TextToUtf8(frames[currentFrame][i], knownWidth);
            screen.push_back(lineC);
            free(lineC);
         //  cout << "\t" << intCanvas[i] << endl;
        }
        rendered = false;
        render();
    }

/*****************************************************************************/
//flash()
//Set the layer to be displayed in red for a brief period.
/*****************************************************************************/

    void editableLayer::flash() {
        tint = {255, 0, 0, 255};
        flashCount = 10;
    }

/*****************************************************************************/
//Toggle hiding.
/*****************************************************************************/

    void editableLayer::toggleHide() {
        visible = !visible;
    }


/*****************************************************************************/
//move entity using left mouse button (for all entities.)
/*****************************************************************************/

    void editableLayer::move(vector<intVector2> mousePos) {
        x += mousePos[0].x;
        y += mousePos[0].y;
    }


/*****************************************************************************/
//Get whether the mouse is over this tile
/*****************************************************************************/

    bool editableLayer::mouseOn() {
        intVector2 tile = intVector2(theScreen -> getMouseRelativeTo(x, y, scale));
        return (visible && tile.x >= 0 && tile.x <= getCols() && tile.y >= 0 && tile.y <= getRows());
    }

/*****************************************************************************/
//Get which tile the mouse is on
/*****************************************************************************/

    intVector2 editableLayer::getMouseTile() {
        return intVector2(theScreen -> getMouseRelativeTo(x, y, scale));
    }

/*****************************************************************************/
//Apply changes using left mouse button (for layers only, not other entities)
/*****************************************************************************/

    void editableLayer::leftBrush(vector<intVector2> mousePos, int brushID, charFill* F, float density, bool absolute) {

        //Create a new frame in undo history (deep copy)

        vector<int*> intCanvas;
        for (int* line : frames[currentFrame]) {
            int* newLine = new int[knownWidth];
            for (int i = 0; i < knownWidth; i++) {
                newLine[i] = line[i];
            }
            intCanvas.push_back(newLine);
        }

        //Allow for absolute vs. relative coordinates in get() calls
        int tileXOrig = 0, tileYOrig = 0;
        if (!absolute) {
            tileXOrig = mousePos[0].x;
            tileYOrig = mousePos[0].y;
        }

        //Apply the brush to the new frame

        switch(brushID) {
            case 0: {   //Pencil
                int tileX = mousePos[0].x;
                int tileY = mousePos[0].y;
                if (tileX >= 0 && tileX < knownWidth && tileY >= 0 && tileY < intCanvas.size()) {
                    intCanvas[tileY][tileX] = F -> get(tileX - tileXOrig, tileY - tileYOrig);
                }
                break;
            }

            case 1: {   //Square brush

                int x1 = max(0, min(mousePos[0].x, mousePos[1].x));
                int x2 = min(getCols() - 1, max(mousePos[0].x, mousePos[1].x));
                int y1 = max(0, min(mousePos[0].y, mousePos[1].y));
                int y2 = min(getRows() - 1, max(mousePos[0].y, mousePos[1].y));

                for (int i = 0; i <= y2 - y1; i++) {
                    for (int j = 0; j <= x2 - x1; j++) {
                        if (GetRandomValue(0, 1000) / 1000.0 <= density) {
                            intCanvas[y1 + i][x1 + j] = F -> get(x1 + j - tileXOrig, y1 + i - tileYOrig);
                        }
                    }
                }
                break;
            }

/*

                int tileX1 = mousePos[0].x;
                int tileY1 = mousePos[0].y;
                int tileX2 = mousePos[1].x;
                int tileY2 = mousePos[1].y;
                for (int i = min((int)intCanvas.size() - 1, max(0, min(tileY1, tileY2))); i != min((int)intCanvas.size() - 1, max(0, max(tileY1, tileY2))) + 1; i++) {
                    for (int j = max(0, min(tileX1, tileX2)); j != min(knownWidth, max(tileX1, tileX2)) + 1; j++) {
                        if (GetRandomValue(0, 1000) / 1000.0 <= density) {
                            intCanvas[i][j] = F -> get(j - tileXOrig, i - tileYOrig);
                        }
                    }
                }
                break;
            }

*/

            case 2: {   //Diamond brush (Corners are always 90 degrees - so it's a rectangle rotated 45 degrees)
                int tileX1, tileY1, tileX2, tileY2;
                if (abs(mousePos[0].x - mousePos[1].x) < abs(mousePos[0].y - mousePos[1].y)) {
                    if (mousePos[0].y < mousePos[1].y) {
                        tileX1 = mousePos[0].x;
                        tileY1 = mousePos[0].y;
                        tileX2 = mousePos[1].x;
                        tileY2 = mousePos[1].y;
                    }
                    else {
                        tileX1 = mousePos[1].x;
                        tileY1 = mousePos[1].y;
                        tileX2 = mousePos[0].x;
                        tileY2 = mousePos[0].y;
                    }
                    for (int i = 0; i <= tileY2 - tileY1; i++) {
                        if (tileY1 + i >= 0 && tileY1 + i < getRows()) {
                            for (int j = max(0, max(tileX1 - i, tileX2 - (tileY2 - tileY1 - i))); j <=  min(knownWidth, min(tileX1 + i, tileX2 + (tileY2 - tileY1 - i))); j++) {
                                if (GetRandomValue(0, 1000) / 1000.0 <= density) {
                                    intCanvas[tileY1 + i][j] = F -> get(j - tileXOrig, tileY1 + i - tileYOrig);
                                }
                            }
                        }
                    }
                }
                else {
                    if (mousePos[0].x < mousePos[1].x) {
                        tileX1 = mousePos[0].x;
                        tileY1 = mousePos[0].y;
                        tileX2 = mousePos[1].x;
                        tileY2 = mousePos[1].y;
                    }
                    else {
                        tileX1 = mousePos[1].x;
                        tileY1 = mousePos[1].y;
                        tileX2 = mousePos[0].x;
                        tileY2 = mousePos[0].y;
                    }
                    for (int i = 0; i <= tileX2 - tileX1; i++) {
                        if (tileX1 + i >= 0 && tileX1 + i < knownWidth) {
                            for (int j = max(0, max(tileY1 - i, tileY2 - (tileX2 - tileX1 - i))); j <= min(getRows() - 1, min(tileY1 + i, tileY2 + (tileX2 - tileX1 - i))); j++) {
                                if (GetRandomValue(0, 1000) / 1000.0 <= density) {
                                    intCanvas[j][tileX1 + i] = F -> get(i - tileXOrig, tileY1 + j - tileYOrig);
                                }
                            }
                        }
                    }
                }
                break;
            }
            case 4: {   //Quadrilateral brush

                //Separate vectors from first to second and third mouse click into magnitude and direction (unit vectors
                //v1 and v2)

                Vector2 origin = {mousePos[0].x + 0.5, mousePos[0].y + 0.5};
                Vector2 v1 = {mousePos[1].x - mousePos[0].x, mousePos[1].y - mousePos[0].y};
                float v1Magnitude = Vector2Length(v1);
                v1 = Vector2Normalize(v1);
                Vector2 v2 = {mousePos[2].x - mousePos[1].x, mousePos[2].y - mousePos[1].y};
                float v2Magnitude = Vector2Length(v2);
                v2 = Vector2Normalize(v2);

                //Fill in the area

                for (float count1 = 0; count1 <= v1Magnitude; count1 += 0.5) {
                    for (float count2 = 0; count2 <= v2Magnitude; count2 += 0.5) {
                        Vector2 result = Vector2Add(Vector2Add(Vector2Scale(v1, count1), Vector2Scale(v2, count2)), origin);
                        if (0 < result.x && result.x < knownWidth && result.y > 0 && result.y < intCanvas.size() && (GetRandomValue(0, 1000) / 1000.0 <= density)) {
                            intCanvas[(int)result.y][(int)result.x] = F -> get((int)result.x - tileXOrig, (int)result.y - tileYOrig);
                        }
                    }
                }
                break;
            }

            case 5: {   //Gradient brush (no blending)

                int gradSelect = F -> get(0, 0);
                vector<int> gradChars;
                if (0x2581 <= gradSelect && gradSelect <= 0x2587) {
                    gradChars = {0x2588, 0x2587, 0x2586, 0x2585, 0x2584, 0x2583, 0x2582, 0x2581, ' '};
                }
                else if (0x2586 <= gradSelect && gradSelect <= 0x258F) {
                    gradChars = {0x2588, 0x2589, 0x258a, 0x258b, 0x258c, 0x258d, 0x258e, 0x258f, ' '};
                }
                else if (0x2591 <= gradSelect && gradSelect <= 0x2593) {
                    gradChars = {0x2588, 0x2593, 0x2592, 0x2591, ' '};
                }
                else {
                    gradChars = {'E', 'R', 'R', 'O', 'R'};
                    cout << "Error: Attempted to use gradient with a non-gradient fill.\n";
                }
                int charIndex = 0;

                //Separate vectors from first to second and third mouse click into magnitude and direction (unit vectors
                //v1 and v2)

                Vector2 origin = {mousePos[0].x + 0.5, mousePos[0].y + 0.5};
                Vector2 v1 = {mousePos[1].x - mousePos[0].x, mousePos[1].y - mousePos[0].y};
                float v1Magnitude = Vector2Length(v1);
                v1 = Vector2Normalize(v1);
                Vector2 v2 = {mousePos[2].x - mousePos[1].x, mousePos[2].y - mousePos[1].y};
                float v2Magnitude = Vector2Length(v2);
                v2 = Vector2Normalize(v2);

                //Fill in the area

                for (float count2 = 0; count2 <= v2Magnitude; count2 += 0.5) {
                    if (count2 > v2Magnitude / gradChars.size() * (charIndex + 1)) {
                        charIndex++;
                    }
                    for (float count1 = 0; count1 <= v1Magnitude; count1 += 0.5) {
                        Vector2 result = Vector2Add(Vector2Add(Vector2Scale(v1, count1), Vector2Scale(v2, count2)), origin);
                        if (0 < result.x && result.x < knownWidth && result.y > 0 && result.y < intCanvas.size() && GetRandomValue(0, 1000) / 1000.0 <= density) {
                            intCanvas[(int)result.y][(int)result.x] = gradChars[charIndex];
                        }
                    }
                }
                break;
            }

            case 6: {   //Gradient brush (with blending)

                int gradSelect = F -> get(0, 0);
                vector<int> gradChars;
                if (0x2581 <= gradSelect && gradSelect <= 0x2587) {
                    gradChars = {0x2588, 0x2587, 0x2586, 0x2585, 0x2584, 0x2583, 0x2582, 0x2581, ' '};
                }
                else if (0x2586 <= gradSelect && gradSelect <= 0x258F) {
                    gradChars = {0x2588, 0x2589, 0x258a, 0x258b, 0x258c, 0x258d, 0x258e, 0x258f, ' '};
                }
                else if (0x2591 <= gradSelect && gradSelect <= 0x2593) {
                    gradChars = {0x2588, 0x2593, 0x2592, 0x2591, ' '};
                }
                else {
                    gradChars = {'E', 'R', 'R', 'O', 'R'};
                    cout << "Error: Attempted to use gradient with a non-gradient fill.\n";
                }
                int charIndex = 0;

                //Separate vectors from first to second and third mouse click into magnitude and direction (unit vectors
                //v1 and v2)

                Vector2 origin = {mousePos[0].x + 0.5, mousePos[0].y + 0.5};
                Vector2 v1 = {mousePos[1].x - mousePos[0].x, mousePos[1].y - mousePos[0].y};
                float v1Magnitude = Vector2Length(v1);
                v1 = Vector2Normalize(v1);
                Vector2 v2 = {mousePos[2].x - mousePos[1].x, mousePos[2].y - mousePos[1].y};
                float v2Magnitude = Vector2Length(v2);
                v2 = Vector2Normalize(v2);

                //Fill in the area

                for (float count2 = 0; count2 <= v2Magnitude; count2 += 0.5) {
                    if (count2 > v2Magnitude / gradChars.size() * (charIndex + 1)) {
                        charIndex++;
                    }
                    for (float count1 = 0; count1 <= v1Magnitude; count1 += 0.5) {
                        Vector2 result = Vector2Add(Vector2Add(Vector2Scale(v1, count1), Vector2Scale(v2, count2)), origin);
                        if (0 < result.x && result.x < knownWidth && result.y > 0 && result.y < intCanvas.size() && GetRandomValue(0, 1000) / 1000.0 <= density) {
                            if (find(gradChars.begin(), gradChars.end(), intCanvas[(int)result.y][(int)result.x]) - gradChars.begin() > charIndex) {
                                intCanvas[(int)result.y][(int)result.x] = gradChars[charIndex];
                            }
                        }
                    }
                }
                break;
            }

            case 7: {   //Ragged

                //Separate vectors from first to second and third mouse click into magnitude and direction (unit vectors
                //v1 and v2)

                Vector2 origin = {mousePos[0].x + 0.5, mousePos[0].y + 0.5};
                Vector2 v1 = {mousePos[1].x - mousePos[0].x, mousePos[1].y - mousePos[0].y};
                float v1Magnitude = Vector2Length(v1);
                v1 = Vector2Normalize(v1);
                Vector2 v2 = {mousePos[2].x - mousePos[1].x, mousePos[2].y - mousePos[1].y};
                float v2Magnitude = Vector2Length(v2);
                v2 = Vector2Normalize(v2);

                for (float count1 = 0; count1 <= v1Magnitude; count1 += 0.5) {
                    float spikeLength = GetRandomValue(0, v2Magnitude * 10) / 10;
                    for (float count2 = 0; count2 <= spikeLength; count2 += 0.5) {
                        Vector2 result = Vector2Add(Vector2Add(Vector2Scale(v1, count1), Vector2Scale(v2, count2)), origin);
                        if (0 < result.x && result.x < knownWidth && result.y > 0 && result.y < intCanvas.size() && GetRandomValue(0, 1000) / 1000.0 <= density) {
                            intCanvas[(int)result.y][(int)result.x] = F -> get((int)result.x - tileXOrig, (int)result.y - tileYOrig);
                        }
                    }
                }
                break;
            }

            case 9: {   //Replace
                if (mousePos[0].y >= 0 && mousePos[0].y < intCanvas.size() && mousePos[0].x >= 0 && mousePos[0].x < knownWidth) {
                    int toReplace = intCanvas[(int)mousePos[0].y][(int)mousePos[0].x];
                    for (int i = 0; i < intCanvas.size(); i++) {
                        for (int j = 0; j < knownWidth; j++) {
                            if (intCanvas[i][j] == toReplace) {
                                intCanvas[i][j] = F -> get(j - tileXOrig, i - tileYOrig);
                            }
                        }
                    }
                }
                break;
            }

            default: {
                cerr << "leftBrush called with invalid brush selection.";
            }
        }
        //Discard any frames more recent than the new one (any redo frames)

        while (frames.size() > currentFrame + 1) {
            frames.pop_back();
        }

        //Push the new frame

        currentFrame++;
        frames.push_back(intCanvas);

        //Update the visible screen

        update();
    }


/*****************************************************************************/
//setArea()
/*****************************************************************************/

    void editableLayer::setArea() {
        if (json.HasMember("height") && json.HasMember("width")) {
            intVector2 newArea = intVector2(theScreen -> getMouseRelativeTo(x, y, scale));
            newArea.x++;
            newArea.y++;
            if (newArea.x >= 1 && newArea.y >= 1) {
                displayWidth = newArea.x;
                displayHeight = newArea.y;
                screen.clear();
                for (int i = 0; i < displayHeight; i++) {
                    screen.push_back(string(displayWidth, display));
                }
                UnloadRenderTexture(tex);
                tex = LoadRenderTexture(getCols() * 8, getRows() * 8);
                rendered = false;
                render();
            }
        }
    }

/*****************************************************************************/
//erase
/*****************************************************************************/

    void editableLayer::erase(vector<intVector2> mousePos) {

        //Create new frame

        vector<int*> intCanvas;
        for (int* line : frames[currentFrame]) {
            int* newLine = new int[knownWidth];
            for (int i = 0; i < knownWidth; i++) {
                newLine[i] = line[i];
            }
            intCanvas.push_back(newLine);
        }

        //Get position to cut

        int x1 = max(0, min(mousePos[0].x, mousePos[1].x));
        int x2 = min(getCols() - 1, max(mousePos[0].x, mousePos[1].x));
        int y1 = max(0, min(mousePos[0].y, mousePos[1].y));
        int y2 = min(getRows() - 1, max(mousePos[0].y, mousePos[1].y));

        //Fill with space character

        for (int yIter = 0; yIter <= y2 - y1; yIter++) {
            for (int xIter = 0; xIter <= x2 - x1; xIter++) {
                intCanvas[yIter + y1][xIter + x1] = ' ';
            }
        }

        //Discard any frames more recent than the new one (any redo frames)

        while (frames.size() > currentFrame + 1) {
            frames.pop_back();
        }

        //Push the new frame

        currentFrame++;
        frames.push_back(intCanvas);

        //Update the visible screen

        update();
    }


/*****************************************************************************/
//Cut
/*****************************************************************************/

    vector<vector<int>> editableLayer::cut(vector<intVector2> mousePos) {

        //Create new frame

        vector<int*> intCanvas;
        for (int* line : frames[currentFrame]) {
            int* newLine = new int[knownWidth];
            for (int i = 0; i < knownWidth; i++) {
                newLine[i] = line[i];
            }
            intCanvas.push_back(newLine);
        }

        //Get position to cut

        int x1 = max(0, min(mousePos[0].x, mousePos[1].x));
        int x2 = min(getCols() - 1, max(mousePos[0].x, mousePos[1].x));
        int y1 = max(0, min(mousePos[0].y, mousePos[1].y));
        int y2 = min(getRows() - 1, max(mousePos[0].y, mousePos[1].y));

        //Copy and fill with space character

        vector<vector<int>> toReturn(y2 - y1 + 1, vector<int>(x2 - x1 + 1, ' '));
        for (int yIter = 0; yIter <= y2 - y1; yIter++) {
            for (int xIter = 0; xIter <= x2 - x1; xIter++) {
                toReturn[yIter][xIter] = intCanvas[yIter + y1][xIter + x1];
                intCanvas[yIter + y1][xIter + x1] = ' ';
            }
        }

        //Discard any frames more recent than the new one (any redo frames)

        while (frames.size() > currentFrame + 1) {
            frames.pop_back();
        }

        //Push the new frame

        currentFrame++;
        frames.push_back(intCanvas);

        //Update the visible screen

        update();

        return toReturn;
    }

/*****************************************************************************/
//Copy
/*****************************************************************************/

    vector<vector<int>> editableLayer::copy(vector<intVector2> mousePos) {
        int x1 = max(0, min(mousePos[0].x, mousePos[1].x));
        int x2 = min(getCols() - 1, max(mousePos[0].x, mousePos[1].x));
        int y1 = max(0, min(mousePos[0].y, mousePos[1].y));
        int y2 = min(getRows() - 1, max(mousePos[0].y, mousePos[1].y));
        vector<vector<int>> toReturn(y2 - y1 + 1, vector<int>(x2 - x1 + 1, ' '));
        for (int yIter = 0; yIter <= y2 - y1; yIter++) {
            for (int xIter = 0; xIter <= x2 - x1; xIter++) {
                toReturn[yIter][xIter] = frames[currentFrame][yIter + y1][xIter + x1];
            }
        }
        return toReturn;
    }

/*****************************************************************************/
//Paste
/*****************************************************************************/

    void editableLayer::paste(vector<intVector2> mousePos, vector<vector<int>> toPaste) {

        //Create new frame

        vector<int*> intCanvas;
        for (int* line : frames[currentFrame]) {
            int* newLine = new int[knownWidth];
            for (int i = 0; i < knownWidth; i++) {
                newLine[i] = line[i];
            }
            intCanvas.push_back(newLine);
        }

        int tileX1 = mousePos[0].x;
        int tileY1 = mousePos[0].y;

        //Apply the change

        for (int yIter = max(tileY1, 0); yIter < min(tileY1 + toPaste.size(), intCanvas.size()); yIter++) {
            for (int xIter = max(tileX1, 0); xIter < min(tileX1 + (int)toPaste[0].size(), knownWidth); xIter++) {
                if (toPaste[yIter - tileY1][xIter - tileX1] != ' ') {
                    intCanvas[yIter][xIter] = toPaste[yIter - tileY1][xIter - tileX1];
                }
            }
        }

        //Discard any frames more recent than the new one (any redo frames)

        while (frames.size() > currentFrame + 1) {
            frames.pop_back();
        }

        //Push the new frame

        currentFrame++;
        frames.push_back(intCanvas);

        //Update the visible screen

        update();
    }

/*****************************************************************************/
//Sample
//Select a tile to the palette (using right mouse button.
/*****************************************************************************/


    int editableLayer::sample(int tileX, int tileY) {
        tileY = min((int)frames[currentFrame].size() - 1, max(0, tileY));
        tileX = min(knownWidth - 1, max(0, tileX));
        return frames[currentFrame][tileY][tileX];
    }

/*****************************************************************************/
//Undo and redo
/*****************************************************************************/

    void editableLayer::undo() {
        if (currentFrame > 0) {
            currentFrame--;
        }
        update();
    }

    void editableLayer::redo() {
        if (currentFrame < frames.size() - 1) {
            currentFrame++;
        }
        update();
    }

/*****************************************************************************/
//Save
/*****************************************************************************/

    void editableLayer::save() {
        if (isLayer && isEditable) {
            cout << "Saving layer " << fileName << endl;
            ofstream layerOut;
            layerOut.open(fileName);
            for (string line : screen) {
                layerOut << line << endl;
            }
            layerOut.close();
        }
        if (json.HasMember("tint")) {
            json["tint"][0] = tint.r;
            json["tint"][1] = tint.g;
            json["tint"][2] = tint.b;
            json["tint"][3] = tint.a;
        }
        if (json.HasMember("x")) {
            json["x"] = x;
        }
        if (json.HasMember("y")) {
            json["y"] = y;
        }
        if (json.HasMember("scale")) {
            json["scale"] = scale;
        }
        if (json.HasMember("width")) {
            json["width"] = displayWidth;
        }
        if (json.HasMember("height")) {
            json["height"] = displayHeight;
        }
    }

/*****************************************************************************/
//Display
/*****************************************************************************/

    void editableLayer::print() {
        if (flashCount-- == 0) {
            tint = original;
        }
        if (visible) {
            theScreen -> drawLayerEditor(x, y, tint, scale, tex.texture, selected, (flashCount <= 0 && doLighting));
        }
    }

/*****************************************************************************/
//editableCollider functions
//Constructor
/*****************************************************************************/

    editableCollider::editableCollider (float newX, float newY, Color newTint, float newScale, bool newIsLayer, string newFileName, char display, Value& dummyJson) :
        entity(newX, newY, newTint, newScale),
        editableLayer(newX, newY, newTint, newScale, newIsLayer, true, newFileName, display, 0, 0, dummyJson, false) {}

/*****************************************************************************/
//Dummy functions which don't do anything for the collider
/*****************************************************************************/

    void editableCollider::setSizeFactor (float newScale) {}

    void editableCollider::move(vector<intVector2> mousePos) {}

/*****************************************************************************/
//Doesn't write anything to JSON
/*****************************************************************************/

    void editableCollider::save() {
        cout << "Saving collider " << fileName << endl;
        ofstream layerOut;
        layerOut.open(fileName);
        for (string line : screen) {
            layerOut << line << endl;
        }
        layerOut.close();
    }

    void editableCollider::print() {
        if (flashCount-- == 0) {
            Color background = theScreen -> getColor();
            tint = (Color){255 - background.r, 255 - background.g, 255 - background.b, 255};
        }
        if (visible) {
            theScreen -> drawLayerEditor(x, y, tint, scale, tex.texture, selected, false);
        }
    }

