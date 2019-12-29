#include "editor_classes.hpp"

using namespace std;

/*****************************************************************************/
//dummyEntity
//An entity that doesn't do anything. Used to display locations of actual entities in the editor.
/*****************************************************************************/

    dummyEntity::dummyEntity(  float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                    float newSizeFactor, char newC) :
        entity(newX, newY, R, G, B, A, newSizeFactor) { 
            toPrint[0] = newC;
        }

    void dummyEntity::tickSet(collider& col) {}

    void dummyEntity::tickGet(collider& col) {}

    bool dummyEntity::finalize() {return false;}

    void dummyEntity::print(float cameraX, float cameraY, Font displayFont)  {
        DrawTextEx(displayFont, toPrint, (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
    }

/*****************************************************************************/
//editableLayer
//A layer designed to be modified, then mayNeedToSave.
/*****************************************************************************/

    editableLayer::editableLayer( float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, string newFileName) :
        layer(newX, newY, R, G, B, A, newSizeFactor, newFileName),
        entity(newX, newY, R, G, B, A, newSizeFactor) {

        //Set original color to reset to after flashing

        original = {R, G, B, A};

        //Read in canvas information. Canvas stores the layer in UTF-8; intCanvas is in UTF-32 (?)
        //Anyway, intCanvas is much easier to edit, and changes are propogated to canvas to display using update()
        //intCanvas is stored in "frames" for undoing/redoing. Complete state is saved in each frame.

        canvas.clear();
        ifstream worldFile;
        worldFile.open(fileName);
        if (!worldFile) {
            cerr << "Error opening layer file.";
            exit(EXIT_FAILURE);
        }
        string line;
        getline(worldFile, line);
        canvas.push_back(line);
        vector<int*> intCanvas;
        int* codepoints = GetCodepoints(line.c_str(), &knownWidth);
        int* newCodepoints = new int[knownWidth];
        for (int i = 0; i < knownWidth; i++) {
            newCodepoints[i] = codepoints[i];
        }
        intCanvas.push_back(newCodepoints);

        while (getline(worldFile, line)) {
            canvas.push_back(line);
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

/*****************************************************************************/
//Accessors
/*****************************************************************************/

    float editableLayer::getX() {
        return x;
    }

    float editableLayer::getY() {
        return y;
    }

    float editableLayer::getSizeFactor() {
        return sizeFactor;
    }

    Color editableLayer::getColor() {
        return tint;
    }

    void editableLayer::setColor(Color newColor) {
        tint = newColor;
        original = newColor;
        cout << fileName << " color set to " << (int)tint.r << " " << (int)tint.g << " " << (int)tint.b << " " << (int)tint.a << endl;
    }

/*****************************************************************************/
//update()
//Propogate changes from intCanvas to the ordinary canvas (which is only used for display.)
/*****************************************************************************/

    void editableLayer::update() {
        canvas.clear();
        for (int i = 0; i < frames[currentFrame].size(); i++) {
            char* lineC = TextToUtf8(frames[currentFrame][i], knownWidth);
            canvas.push_back(lineC);
         //  cout << "\t" << intCanvas[i] << endl;
        }
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
//Determine which tile a given mouse position on the screen is in.
/*****************************************************************************/

    float editableLayer::getTileX(float cameraX, float mouseX) {
        int tileX = mouseX / FONTSIZE / sizeFactor + cameraX - SCREENCOLS / sizeFactor / 2 + x;
        if (tileX < 0) {
            return 0;
        }
        if (tileX > knownWidth - 1) {
            return knownWidth - 1;
        }
        return tileX;
    }

    float editableLayer::getTileY(float cameraY, float mouseY) {
        int tileY = mouseY / FONTSIZE / sizeFactor + cameraY - SCREENROWS / sizeFactor / 2 + y;
        if (tileY < 0) {
            return 0;
        }
        if (tileY > frames[0].size() - 1) {
            return frames[0].size() - 1;
        }
        return tileY;
    }

/*****************************************************************************/
//Apply changes using left mouse button.
/*****************************************************************************/

    void editableLayer::leftBrush(vector<tuple<int, int>> mousePos, int brushID, charFill* F, float density) {

        //Create a new frame in undo history (deep copy)

        vector<int*> intCanvas;
        for (int* line : frames[currentFrame]) {
            int* newLine = new int[knownWidth];
            for (int i = 0; i < knownWidth; i++) {
                newLine[i] = line[i];
            }
            intCanvas.push_back(newLine);
        }

        //Apply the brush to the new frame

        switch(brushID) {
            case 0: {   //Pencil
                intCanvas[get<1>(mousePos[0])][get<0>(mousePos[0])] = F -> get(get<0>(mousePos[0]), get<1>(mousePos[0]));
                break;
            }
            case 1: {   //Square brush
                int tileX1 = get<0>(mousePos[0]);
                int tileY1 = get<1>(mousePos[0]);
                int tileX2 = get<0>(mousePos[1]);
                int tileY2 = get<1>(mousePos[1]);
                for (int i = max(0, min(tileY1, tileY2)); i != min((int)intCanvas.size() - 1, max(tileY1, tileY2)) + 1; i++) {
                    for (int j = max(0, min(tileX1, tileX2)); j != min(knownWidth, max(tileX1, tileX2)) + 1; j++) {
                        if (GetRandomValue(0, 1000) / 1000.0 < density) {
                            intCanvas[i][j] = F -> get(j, i);
                        }
                    }
                }
                break;
            }
            case 2: {   //Diamond brush (Corners are always 90 degrees - so it's a rectangle rotated 45 degrees)
                int tileX1, tileY1, tileX2, tileY2;
                if (abs(get<0>(mousePos[0]) - get<0>(mousePos[1])) < abs(get<1>(mousePos[0]) - get<1>(mousePos[1]))) {
                    if (get<1>(mousePos[0]) < get<1>(mousePos[1])) {
                        tileX1 = get<0>(mousePos[0]);
                        tileY1 = get<1>(mousePos[0]);
                        tileX2 = get<0>(mousePos[1]);
                        tileY2 = get<1>(mousePos[1]);
                    }
                    else {
                        tileX1 = get<0>(mousePos[1]);
                        tileY1 = get<1>(mousePos[1]);
                        tileX2 = get<0>(mousePos[0]);
                        tileY2 = get<1>(mousePos[0]);
                    }
                    for (int i = 0; i <= tileY2 - tileY1; i++) {
                        for (int j = max(0, max(tileX1 - i, tileX2 - (tileY2 - tileY1 - i))); j <=  min(knownWidth, min(tileX1 + i, tileX2 + (tileY2 - tileY1 - i))); j++) {
                            if (GetRandomValue(0, 1000) / 1000.0 < density) {
                                intCanvas[tileY1 + i][j] = F -> get(j, tileY1 + i);
                            }
                        }
                    }
                }
                else {
                    if (get<0>(mousePos[0]) < get<0>(mousePos[1])) {
                        tileX1 = get<0>(mousePos[0]);
                        tileY1 = get<1>(mousePos[0]);
                        tileX2 = get<0>(mousePos[1]);
                        tileY2 = get<1>(mousePos[1]);
                    }
                    else {
                        tileX1 = get<0>(mousePos[1]);
                        tileY1 = get<1>(mousePos[1]);
                        tileX2 = get<0>(mousePos[0]);
                        tileY2 = get<1>(mousePos[0]);
                    }
                    for (int i = 0; i <= tileX2 - tileX1; i++) {
                        for (int j = max(0, max(tileY1 - i, tileY2 - (tileX2 - tileX1 - i))); j <= min(getRows(), min(tileY1 + i, tileY2 + (tileX2 - tileX1 - i))); j++) {
                            if (GetRandomValue(0, 1000) / 1000.0 < density) {
                                intCanvas[j][tileX1 + i] = F -> get(tileX1 + i, j);
                            }
                        }
                    }
                }
                break;
            }
            case 4: {   //Quadrilateral brush

                //Separate vectors from first to second and third mouse click into magnitude and direction (unit vectors
                //v1 and v2)

                Vector2 origin = {get<0>(mousePos[0]) + 0.5, get<1>(mousePos[0]) + 0.5};
                Vector2 v1 = {get<0>(mousePos[1]) - get<0>(mousePos[0]), get<1>(mousePos[1]) - get<1>(mousePos[0])};
                float v1Magnitude = Vector2Length(v1);
                v1 = Vector2Normalize(v1);
                Vector2 v2 = {get<0>(mousePos[2]) - get<0>(mousePos[1]), get<1>(mousePos[2]) - get<1>(mousePos[1])};
                float v2Magnitude = Vector2Length(v2);
                v2 = Vector2Normalize(v2);

                //Fill in the area

                for (float count1 = 0; count1 <= v1Magnitude; count1 += 0.5) {
                    for (float count2 = 0; count2 <= v2Magnitude; count2 += 0.5) {
                        Vector2 result = Vector2Add(Vector2Add(Vector2Scale(v1, count1), Vector2Scale(v2, count2)), origin);
                        if (0 < result.x && result.x < knownWidth && result.y > 0 && result.y < intCanvas.size() && (GetRandomValue(0, 1000) / 1000.0 < density)) {
                            intCanvas[(int)result.y][(int)result.x] = F -> get((int)result.x, (int)result.y);
                        }
                    }
                }
                break;
            }

            case 5: {   //Gradient brush (ignores selected charFill)

                vector<int> gradChars = {0x2588, 0x2593, 0x2592, 0x2591, ' '};
                int charIndex = 0;

                //Separate vectors from first to second and third mouse click into magnitude and direction (unit vectors
                //v1 and v2)

                Vector2 origin = {get<0>(mousePos[0]) + 0.5, get<1>(mousePos[0]) + 0.5};
                Vector2 v1 = {get<0>(mousePos[1]) - get<0>(mousePos[0]), get<1>(mousePos[1]) - get<1>(mousePos[0])};
                float v1Magnitude = Vector2Length(v1);
                v1 = Vector2Normalize(v1);
                Vector2 v2 = {get<0>(mousePos[2]) - get<0>(mousePos[1]), get<1>(mousePos[2]) - get<1>(mousePos[1])};
                float v2Magnitude = Vector2Length(v2);
                v2 = Vector2Normalize(v2);

                //Fill in the area

                for (float count2 = 0; count2 <= v2Magnitude; count2 += 0.5) {
                    if (count2 > v2Magnitude / gradChars.size() * (charIndex + 1)) {
                        charIndex++;
                    }
                    for (float count1 = 0; count1 <= v1Magnitude; count1 += 0.5) {
                        Vector2 result = Vector2Add(Vector2Add(Vector2Scale(v1, count1), Vector2Scale(v2, count2)), origin);
                        if (0 < result.x && result.x < knownWidth && result.y > 0 && result.y < intCanvas.size() && GetRandomValue(0, 1000) / 1000.0 < density) {
                            intCanvas[(int)result.y][(int)result.x] = gradChars[charIndex];
                        }
                    }
                }
                break;
            }

            case 6: {   //Other gradient brush

                vector<int> gradChars = {0x2588, 0x2587, 0x2586, 0x2585, 0x2584, 0x2583, 0x2582, 0x2581, ' '};
                int charIndex = 0;

                //Separate vectors from first to second and third mouse click into magnitude and direction (unit vectors
                //v1 and v2)

                Vector2 origin = {get<0>(mousePos[0]) + 0.5, get<1>(mousePos[0]) + 0.5};
                Vector2 v1 = {get<0>(mousePos[1]) - get<0>(mousePos[0]), get<1>(mousePos[1]) - get<1>(mousePos[0])};
                float v1Magnitude = Vector2Length(v1);
                v1 = Vector2Normalize(v1);
                Vector2 v2 = {get<0>(mousePos[2]) - get<0>(mousePos[1]), get<1>(mousePos[2]) - get<1>(mousePos[1])};
                float v2Magnitude = Vector2Length(v2);
                v2 = Vector2Normalize(v2);

                //Fill in the area

                for (float count2 = 0; count2 <= v2Magnitude; count2 += 0.5) {
                    if (count2 > v2Magnitude / gradChars.size() * (charIndex + 1)) {
                        charIndex++;
                    }
                    for (float count1 = 0; count1 <= v1Magnitude; count1 += 0.5) {
                        Vector2 result = Vector2Add(Vector2Add(Vector2Scale(v1, count1), Vector2Scale(v2, count2)), origin);
                        if (0 < result.x && result.x < knownWidth && result.y > 0 && result.y < intCanvas.size() && GetRandomValue(0, 1000) / 1000.0 < density) {
                            intCanvas[(int)result.y][(int)result.x] = gradChars[charIndex];
                        }
                    }
                }
                break;
            }

            case 7: {   //Ragged

                //Separate vectors from first to second and third mouse click into magnitude and direction (unit vectors
                //v1 and v2)

                Vector2 origin = {get<0>(mousePos[0]) + 0.5, get<1>(mousePos[0]) + 0.5};
                Vector2 v1 = {get<0>(mousePos[1]) - get<0>(mousePos[0]), get<1>(mousePos[1]) - get<1>(mousePos[0])};
                float v1Magnitude = Vector2Length(v1);
                v1 = Vector2Normalize(v1);
                Vector2 v2 = {get<0>(mousePos[2]) - get<0>(mousePos[1]), get<1>(mousePos[2]) - get<1>(mousePos[1])};
                float v2Magnitude = Vector2Length(v2);
                v2 = Vector2Normalize(v2);

                for (float count1 = 0; count1 <= v1Magnitude; count1 += 0.5) {
                    float spikeLength = GetRandomValue(0, v2Magnitude * 10) / 10;
                    for (float count2 = 0; count2 <= spikeLength; count2 += 0.5) {
                        Vector2 result = Vector2Add(Vector2Add(Vector2Scale(v1, count1), Vector2Scale(v2, count2)), origin);
                        if (0 < result.x && result.x < knownWidth && result.y > 0 && result.y < intCanvas.size() && GetRandomValue(0, 1000) / 1000.0 < density) {
                            intCanvas[(int)result.y][(int)result.x] = F -> get((int)result.x, (int)result.y);
                        }
                    }
                }
                break;
            }

            case 9: {   //Replace

                int toReplace = intCanvas[get<1>(mousePos[0])][get<0>(mousePos[0])];
                for (int i = 0; i < intCanvas.size(); i++) {
                    for (int j = 0; j < knownWidth; j++) {
                        if (intCanvas[i][j] == toReplace) {
                            intCanvas[i][j] = F -> get(j, i);
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

        //Update the visible canvas

        update();
    }

/*****************************************************************************/
    //Cut
/*****************************************************************************/

    vector<vector<int>> editableLayer::cut(vector<tuple<int, int>> mousePos) {

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

        int x1 = min(get<0>(mousePos[0]), get<0>(mousePos[1]));
        int x2 = max(get<0>(mousePos[0]), get<0>(mousePos[1]));
        int y1 = min(get<1>(mousePos[0]), get<1>(mousePos[1]));
        int y2 = max(get<1>(mousePos[0]), get<1>(mousePos[1]));

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

        //Update the visible canvas

        update();

        return toReturn;
    }

/*****************************************************************************/
//Copy
/*****************************************************************************/

    vector<vector<int>> editableLayer::copy(vector<tuple<int, int>> mousePos) {
        int x1 = min(get<0>(mousePos[0]), get<0>(mousePos[1]));
        int x2 = max(get<0>(mousePos[0]), get<0>(mousePos[1]));
        int y1 = min(get<1>(mousePos[0]), get<1>(mousePos[1]));
        int y2 = max(get<1>(mousePos[0]), get<1>(mousePos[1]));
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

    void editableLayer::paste(vector<tuple<int, int>> mousePos, vector<vector<int>> toPaste) {

        //Create new frame

        vector<int*> intCanvas;
        for (int* line : frames[currentFrame]) {
            int* newLine = new int[knownWidth];
            for (int i = 0; i < knownWidth; i++) {
                newLine[i] = line[i];
            }
            intCanvas.push_back(newLine);
        }

        int tileX1 = get<0>(mousePos[0]);
        int tileY1 = get<1>(mousePos[0]);

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

        //Update the visible canvas

        update();
    }

/*****************************************************************************/
//Sample
//Select a tile to the palette (using right mouse button.
/*****************************************************************************/


    int editableLayer::sample(int tileX, int tileY) {
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
        cout << "Saving layer " << fileName << endl;
        ofstream outfile;
        outfile.open(fileName);
        for (string line : canvas) {
            outfile << line << endl;
        }
        outfile.close();
    }

/*****************************************************************************/
//Display
/*****************************************************************************/

    void editableLayer::print(float cameraX, float cameraY, Font displayFont) {
        if (flashCount-- == 0) {
            tint = original;
        }
        if (visible) {
            layer::print(cameraX, cameraY, displayFont);
        }
    }
