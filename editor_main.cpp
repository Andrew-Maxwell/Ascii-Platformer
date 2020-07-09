#include "meta.hpp"
#include "dummyentity.hpp"
#include "editables.hpp"
#include "charfills.hpp"
#include "editorleveldata.hpp"

using namespace rapidjson;

/******************************************************************************/
//Main loop
/******************************************************************************/

int main(int argc, char** argv) {

    if (argc != 2) {
        cout << "Usage: ./editor [main level file name]\n";
        exit(EXIT_FAILURE);
    }

    //Initialize raylib

    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "ASCII Platformer - level editor");
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    SetTargetFPS(60);
    SetExitKey(KEY_RIGHT_CONTROL);

    //Mouse interface variables

    vector<intVector2> mousePos;
    collider markers;

    //Clipboard

    vector<vector<int>> clipboard;

    //charFill variables - analogous to color in an image editor

    vector<charFill*> charFills;
    for (int i = 0; i < NUMCHARS; i++) {  //Populate all single-character charFills
        charFills.push_back(new charFill(FONTCHARS[i]));
    }

    //Random charFills

    randomCharFill* allRandom = new randomCharFill;
    allRandom -> codepoints = vector<int>(FONTCHARS, FONTCHARS + NUMCHARS);
    charFills.push_back(allRandom);

    randomCharFill* sparse = new randomCharFill({0x27, 0x2c, 0x2d, 0x2e, 0x60, 0x7e, 0xa8, 0xaf, 0xb0, 0xb4, 0xb7, 0xb8, 0xba, 0x2c6, 0x2c7, 0x2d9, 0x2db, 0x2dd, 0x384, 0x385, 0x387, 0x2018, 0x2019, 0x201a, 0x201b, 0x2022, 0x203f, 0x2040, 0x2054, 0x207b, 0x20aa, 0x2219, 0x256d, 0x256e, 0x256f, 0x2570, 0x2574, 0x2575, 0x2576, 0x2577, 0x2578, 0x2579, 0x257a, 0x257b, 0x25aa, 0x25ab, 0x25ac});
    charFills.push_back(sparse);

    randomCharFill* heavy = new randomCharFill({0x3, 0x4, 0x5, 0x6, 0x8, 0xe, 0xf, 0x13, 0x14, 0x15, 0x17, 0x23, 0xb6, 0xbc, 0x110, 0x132, 0x13d, 0x142, 0x158, 0x162, 0x17d, 0x1fd, 0x1fe, 0x278, 0x39b, 0x39e, 0x3a0, 0x3f4, 0x428, 0x2116, 0x2580, 0x2593, 0x25e2, 0x25e3, 0x25e4, 0x25e5, 0x25d8, 0x2587, 0x2588, 0x2590, 0x258c, 0x254b, 0x2549, 0x254a, 0x2548, 0x2547});
    charFills.push_back(heavy);

    randomCharFill* organic = new randomCharFill({0x0015, 0x001c, 0x0024, 0x7c, 0x7c, 0x7e, 0xa5, 0xa4, 0xa8, 0xaa, 0xb4, 0xca, 0xd4, 0x115, 0x15e, 0x15c, 0x15d, 0x175, 0x173, 0x01a1, 0x017f, 0x0192, 0x0251, 0x0278, 0x02c6, 0x2c7, 0x2c9, 0x2d8, 0x2d9, 0x2da, 0x2db, 0x2dc, 0x2dd, 0x37e, 0x384, 0x393, 0x390, 0x3ad, 0x3af, 0x3c3, 0x3f4, 0x431, 0x453, 0x452, 0x459, 0x20aa, 0x20ac, 0x25cb, 0x256d, 0x256e, 0x256f, 0x2570});
    charFills.push_back(organic);

    randomCharFill* mechanical = new randomCharFill({0x7, 0x8, 0x9, 0xf, 0x23, 0x5b, 0x5d, 0xa9, 0xae, 0x2c6, 0x2c9, 0x2dd, 0x29e, 0x394, 0x3c0, 0x415, 0x428, 0x448, 0x2020, 0x2021, 0x203c, 0x2026, 0x20aa, 0x2195, 0x2229, 0x2260, 0x2261});
    charFills.push_back(mechanical);

    randomCharFill* groundPlants = new randomCharFill({0x5, 0x12, 0x15, 0x17, 0x18, 0x19, 0x1c, 0x26, 0x2c, 0x2e, 0x49, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0xc7, 0xd5, 0xe6, 0x132, 0x17f, 0x17f, 0x192, 0x21b, 0x278, 0x3be, 0x3c8, 0x3b6, 0x3f4, 0x424, 0x444, 0x490, 0x491, 0x1d1b, 0x1d26, 0x1e9f, 0x2020, 0x2021, 0x20a4, 0x2113, 0x2126, 0x2195, 0x21a8, 0x222b, 0x2320});
    charFills.push_back(groundPlants);

    randomCharFill* solidGeometry = new randomCharFill({0x250f, 0x2513, 0x2517, 0x251b, 0x2501, 0x2503, 0x2523, 0x252b, 0x2533, 0x253b, 0x254b, 0x2580, 0x2581, 0x2582, 0x2583, 0x2584, 0x2585, 0x2586, 0x2587, 0x2588, 0x2588, 0x2588, 0x2590, 0x2590, 0x2590,0x2580, 0x2580, 0x258c, 0x258c, 0x258c, 0x25d8, 0x25d8, 0x25d8, 0x25d9, 0x25d9, 0x25e2, 0x25e2, 0x25e3, 0x25e3, 0x25e4, 0x25e4});
    charFills.push_back(solidGeometry);

    randomCharFill* shading = new randomCharFill({' ', 0x2591, 0x2592, 0x2593, 0x2588});
    charFills.push_back(shading);

    randomCharFill* vertical = new randomCharFill({ 0x0049, 0x005d, 0x012e, 0x2502, 0x2503, 0x257d, 0x257f, 0x25ae});
    charFills.push_back(vertical);

    randomCharFill* horizontal = new randomCharFill({0x001d, 0x002A, 0x2500, 0x2501, 0x257c, 0x257e});
    charFills.push_back(horizontal);

    randomCharFill* randomPipes = new randomCharFill;
    randomCharFill* singlePipes = new randomCharFill;
    randomCharFill* doublePipes = new randomCharFill;
    for (int i = 0x2500; i < 0x2504; i++) {
        randomPipes -> codepoints.push_back(i);
        singlePipes -> codepoints.push_back(i);
    }
    for (int i = 0x250c; i < 0x254c; i++) {
        randomPipes -> codepoints.push_back(i);
        singlePipes -> codepoints.push_back(i);
    }
    for (int i = 0x2550; i < 0x256c; i++) {
        randomPipes -> codepoints.push_back(i);
        doublePipes -> codepoints.push_back(i);
    }
    for (int i = 0x2574; i < 0x2580; i++) {
        randomPipes -> codepoints.push_back(i);
        singlePipes -> codepoints.push_back(i);
    }
    charFills.push_back(randomPipes);
    charFills.push_back(singlePipes);
    charFills.push_back(doublePipes);

    //Grid charfills

    for (int i = 1; i < 10; i++) {
        charFills.push_back(new gridCharFill(i, 0x2500, 0x2502, 0x253c));
        charFills.push_back(new gridCharFill(i, 0x2501, 0x2503, 0x254b));
        charFills.push_back(new gridCharFill(i, 0x2550, 0x2551, 0x256c));
        charFills.push_back(new diagGridCharFill(i, 0x2571, 0x2572, 0x2573));
    }

    //charFill selector

    int palette[44] = {767, 768, 769, 770, 771, 772, 773, 774,775, 776, 777, 778, 779, 780, 781, 782, 783, 784, 785, 786, 787, 788, 115, 46, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57};
    int paletteSelection = 0;
    int colliderSelected = 0;

    //Brush variables - analogous to brush shape, size, etc. in an image editor

    int brushID = 0;
    int brushClickCount = 1;
    string brushName = "(P)encil";
    float density = 1;
    bool absoluteBrush = false;

    //mayNeedToSave?

    bool mayNeedToSave = false;
    bool exit = false;

    //Level data

    int placeHoldersAdded = 0;
    list<editableLayer*> layers;
    editableLayer* layerClipboard = NULL;
    Color background;
    int fontSize;
    string fileName(argv[1]);
    editorLevelData level;
    level.load(fileName);
    cout << "Starting loading entities...\n";
    level.initializeEditor(layers, background);
    list<editableLayer*>::iterator thisLayer = layers.begin();
    (*thisLayer) -> select();
    cout << "Finished loading.\n";

    //Main loop

    while (!WindowShouldClose() || mayNeedToSave) {

        while (!(WindowShouldClose() || exit)) {

            if (IsKeyPressed(KEY_ESCAPE)) {
                exit = true;
            }

            //Character selector

            if (IsKeyDown(KEY_TAB)) {

                mousePos.clear();
                theScreen -> start(true);

                //Print out all available charFills

                for (int i = 0; i < charFills.size(); i++) {
                    int codePointToDisplay =  charFills[i] -> display();
                    char* temp = TextToUtf8(&codePointToDisplay, 1);
                    theScreen -> drawHud(i % (theScreen -> getHudCols() / 2) * 2,
                                         i / (theScreen -> getHudCols() / 2) * 2,
                                         UIFOREGROUND, string(temp));
                    free(temp);
                }

                //Print out currently selected brush

                string brushLabel = brushName + " Density: " + to_string(density) + (absoluteBrush? " Absolute mode" : " Relative mode");
                theScreen -> drawHud(1, theScreen -> getHudRows() - 2, UIFOREGROUND, brushLabel);

                theScreen -> end();

                //If a character is clicked, add it to the palette

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    Vector2 mouse = GetMousePosition();
                    int selection = (int)mouse.y / (2 * theScreen -> getHudFontSize()) *
                        (theScreen -> getHudCols() / 2) + (int)mouse.x / (2 * theScreen -> getHudFontSize());
                    if (selection > 0 && selection < charFills.size()) {
                        palette[paletteSelection + 22 * colliderSelected] = selection;
                    }
                }

                //Switch brush

                if (IsKeyPressed(KEY_P)) {
                    brushName = "(P)encil";
                    brushID = 0;
                    brushClickCount = 1;
                }
                if (IsKeyPressed(KEY_B)) {
                    brushName = "(B)ox";
                    brushID = 1;
                    brushClickCount = 2;
                }
                if (IsKeyPressed(KEY_D)) {
                    brushName = "(D)iamond";
                    brushID = 2;
                    brushClickCount = 2;
                }
                if (IsKeyPressed(KEY_E)) {
                    brushName = "(E)ntity move";
                    brushID = 3;
                    brushClickCount = 2;
                }
                if (IsKeyPressed(KEY_Q)) {
                    brushName = "(Q)uadrilateral";
                    brushID = 4;
                    brushClickCount = 3;
                }
                if (IsKeyPressed(KEY_F)) {
                    brushName = "Gradient (no blending)";
                    brushID = 5;
                    brushClickCount = 3;
                }
                if (IsKeyPressed(KEY_G)) {
                    brushName = "(G)radient (with blending)";
                    brushID = 6;
                    brushClickCount = 3;
                }
                if (IsKeyPressed(KEY_R)) {
                    brushName = "(R)agged";
                    brushID = 7;
                    brushClickCount = 3;
                }
                if (IsKeyPressed(KEY_S)) {
                    brushName = "(S)elect";
                    brushID = 8;
                    brushClickCount = 100000;
                }
                if (IsKeyPressed(KEY_T)) {
                    brushName = "Replace / (T)ransition";
                    brushID = 9;
                    brushClickCount = 1;
                }
                if (IsKeyPressed(KEY_W)) {
                    brushName = "Add Placeholder Entities";
                    brushID = 10;
                    brushClickCount = 1;
                }
                if (IsKeyPressed(KEY_H)) {
                    brushName = "Adjust (H)eight and Width";
                    brushID = 11;
                    brushClickCount = 1;
                }
                if (IsKeyPressed(KEY_X) && density > 0) {
                    if (IsKeyDown(KEY_LEFT_SHIFT)) {
                        density -= 0.05;
                    }
                    else {
                        density -= 0.005;
                    }
                }
                if (IsKeyPressed(KEY_C) && density < 1) {
                     if (IsKeyDown(KEY_LEFT_SHIFT)) {
                        density += 0.05;
                    }
                    else {
                        density += 0.005;
                    }
                }
                if (IsKeyPressed(KEY_Z)) {
                    if (IsKeyDown(KEY_LEFT_SHIFT)) {
                        density = 0;
                    }
                    else {
                        density = 1;
                    }
                }
                if (IsKeyPressed(KEY_A)) {
                    absoluteBrush = !absoluteBrush;
                }
            }
            else {

                tickCounter++;

                //Meta: Undo, redo, save

                if (IsKeyDown(KEY_LEFT_CONTROL)) {
                    if (IsKeyPressed(KEY_Z)) {
                        if (mousePos.size() > 0) {
                            mousePos.clear();
                            markers.clear();
                        }
                        else if ((*thisLayer) -> getIsEditable()) {
                            (*thisLayer) -> undo();
                            mayNeedToSave = true;
                        }
                    }
                    if (IsKeyPressed(KEY_Y) && (*thisLayer) -> getIsEditable()) {
                        (*thisLayer) -> redo();
                        mayNeedToSave = true;
                    }
                    if (IsKeyPressed(KEY_S)) {
                        level.writeEntities(layers, background);
                        mayNeedToSave = false;
                    }
                    if (IsKeyPressed(KEY_X) && (*thisLayer) -> getIsEditable() && mousePos.size() > 1) {
                        clipboard = (*thisLayer) -> cut(mousePos);
                        markers.clear();
                        mousePos.clear();
                        mayNeedToSave = true;
                    }
                    if (IsKeyPressed(KEY_C) && (*thisLayer) -> getIsEditable() && mousePos.size() > 1) {
                        clipboard = (*thisLayer) -> copy(mousePos);
                        markers.clear();
                        mousePos.clear();
                    }
                    if (IsKeyPressed(KEY_V) && (*thisLayer) -> getIsEditable()) {
                        if (mousePos.size() == 0) {
                            mousePos.push_back((Vector2){0, 0});    //Paste in upper right by default
                        }
                        (*thisLayer) -> paste(mousePos, clipboard);
                        markers.clear();
                        mousePos.clear();
                        mayNeedToSave = true;
                    }
                    if (IsKeyPressed(KEY_A) && (*thisLayer) -> getIsEditable()) {
                        brushName = "Select";
                        brushID = 8;
                        brushClickCount = 100000;
                        mousePos.push_back((Vector2){0, 0});
                        mousePos.push_back((Vector2){(*thisLayer) -> getCols() - 1, (*thisLayer) -> getRows() - 1});
                    }
                    if (IsKeyDown(KEY_LEFT_SHIFT)) {
                        auto endCheck = thisLayer;
                        if (++endCheck != layers.end()) {   //Don't allow the user to change collider (at last pos)
                            if (IsKeyPressed(KEY_DELETE)) {
                                delete *thisLayer;
                                thisLayer = layers.erase(thisLayer);
                            }
                            if (IsKeyPressed(KEY_X)) {
                                delete layerClipboard;
                                layerClipboard = *thisLayer;
                                thisLayer = layers.erase(thisLayer);
                            }
                            if (IsKeyPressed(KEY_C)) {
                                delete layerClipboard;
                                layerClipboard = new editableLayer(**thisLayer);
                            }
                            if (IsKeyPressed(KEY_V) && layerClipboard != NULL) {
                                layers.insert(thisLayer, new editableLayer(*layerClipboard));
                                (*thisLayer) -> deselect();
                                thisLayer++;
                                (*thisLayer) -> select();
                                (*thisLayer) -> flash();
                                markers.clear();
                                mousePos.clear();
                            }
                        }
                    }
                }
                else {  //Control is not down

                    theScreen -> moveCamera();

                    //Delete selection

                    if (IsKeyPressed(KEY_DELETE) && mousePos.size() >= 2) {
                        (*thisLayer) -> erase(mousePos);
                        markers.clear();
                        mousePos.clear();
                        mayNeedToSave = true;
                    }

                    //switch to prev layer, then flash

                    if (IsKeyPressed(KEY_Q)) {
                        (*thisLayer) -> deselect();
                        if (thisLayer != layers.begin()) {
                            thisLayer--;
                        }
                        (*thisLayer) -> select();
                        (*thisLayer) -> flash();
                        markers.clear();
                        mousePos.clear();
                        colliderSelected = 0;
                    }

                    //switch to next layer, then flash

                    if (IsKeyPressed(KEY_E)) {
                        (*thisLayer) -> deselect();
                        if (++thisLayer == layers.end()) {
                            thisLayer--;
                        }
                        if (++thisLayer == layers.end()) {
                            colliderSelected = 1;
                        }
                        thisLayer--;
                        (*thisLayer) -> select();
                        (*thisLayer) -> flash();
                        markers.clear();
                        mousePos.clear();
                    }

                    //hide layer

                    if (IsKeyPressed(KEY_H)) {
                        (*thisLayer) -> toggleHide();
                    }

                    //select brush character

                    for (int i = 0; i < 12; i++) {
                        if (IsKeyPressed(KEY_F1 + i)) {
                            paletteSelection = i;
                        }
                    }
                    for (int i = 0; i < 10; i++) {
                        if (IsKeyPressed(KEY_ZERO + i)) {
                            paletteSelection = 12 + i;
                        }
                    }

                    //Change lighting
                    if (IsKeyPressed(KEY_L)) {
                        theScreen -> changeLighting();
                    }

                    //Adjusting color
                    //accessing Color object like a uint8_t array

                    int componentIndex = -1;
                    if (IsKeyDown(KEY_R)) {
                        componentIndex = 0;
                    }
                    else if (IsKeyDown(KEY_G)) {
                        componentIndex = 1;
                    }
                    else if (IsKeyDown(KEY_B)) {
                        componentIndex = 2;
                    }
                    else if (IsKeyDown(KEY_T)) {
                        componentIndex = 3;
                    }
                    if (componentIndex != -1) {
                        if (colliderSelected) {
                            uint8_t* componentPtr = &((uint8_t*)&background)[componentIndex];
                            if (IsKeyDown(KEY_LEFT_SHIFT) && *componentPtr > 0) {
                                (*componentPtr)--;
                            }
                            else if (*componentPtr < 255) {
                                (*componentPtr)++;
                            }
                            (*thisLayer) -> setColor((Color){255 - background.r, 255 - background.g, 255 - background.b, 255 - background.a});
                            theScreen -> setColor(background);
                        }
                        else {
                            Color newTint = (*thisLayer) -> getColor();
                            uint8_t* componentPtr = &((uint8_t*)&newTint)[componentIndex];
                            if (IsKeyDown(KEY_LEFT_SHIFT) && *componentPtr > 0) {
                                (*componentPtr)--;
                            }
                            else if (*componentPtr < 255) {
                                (*componentPtr)++;
                            }
                            (*thisLayer) -> setColor(newTint);
                        }
                        mayNeedToSave = true;
                    }

                    //Adjusting size
                    
                    if (IsKeyPressed(KEY_V)) {
                        if (IsKeyDown(KEY_LEFT_SHIFT)) {
                            (*thisLayer) -> setSizeFactor((*thisLayer) -> getSizeFactor() + 0.05);
                        }
                        else {
                            (*thisLayer) -> setSizeFactor((*thisLayer) -> getSizeFactor() - 0.05d);
                        }
                        mayNeedToSave = true;
                    }

                    //Output clipboard in format to be added to a textureCharFill, and create new texture

                    if (IsKeyPressed(KEY_ENTER)) {
                        cout << "{\n";
                        for (vector<int> row : clipboard) {
                            cout << "{";
                            for(int i = 0; i < row.size(); i++) {
                                cout << hex << "0x" << row[i] << ", ";
                            }
                            cout << hex << "0x" << row[row.size() - 1] << "}\n";
                        }
                        cout << "}";
                        charFills.push_back(new textureCharFill(clipboard));
                    }

                    //mouse input: Right click to sample character

                    if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) && (*thisLayer) -> getIsEditable()) {
                        intVector2 tile = (*thisLayer) -> getMouseTile();
                        palette[paletteSelection + 22 * colliderSelected] = theScreen -> myGetGlyphIndex((*thisLayer) -> sample(tile.x, tile.y));
                    }

                    //Left click to add a brush input point or shift to select topmost object

                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if (IsKeyDown(KEY_LEFT_SHIFT)) {
                            auto listIter = thisLayer;
                            listIter++;
                            if (listIter == layers.end()) {
                                listIter = layers.begin();
                            }
                            while ((*listIter) != (*thisLayer) && !(*listIter) -> mouseOn()) {
                                listIter++;
                                if (listIter == layers.end()) {
                                    listIter = layers.begin();
                                }
                            }
                            (*thisLayer) -> deselect();
                            thisLayer = listIter;
                            if (++thisLayer == layers.end()) {
                                colliderSelected = 1;
                            }
                            else {
                                colliderSelected = 0;
                            }
                            thisLayer--;
                            (*thisLayer) -> select();
                            (*thisLayer) -> flash();
                            markers.clear();
                            mousePos.clear();
                        }

                        else {
                            if (brushID == 10) {
                                intVector2 tile = theScreen -> getMouseRelativeTo(0, 0, 1);
                                Value& newJson = level.getNewEntity(tile.x, tile.y, (*thisLayer) -> getSizeFactor(), placeHoldersAdded);
                                editableLayer* newEntity = new editableLayer(tile.x, tile.y, {0, 0, 255, 255}, (*thisLayer) -> getSizeFactor(), false, false, "", 'A' + placeHoldersAdded, 1, 1, newJson);
                                placeHoldersAdded++;
                                layers.push_back(newEntity);
                                thisLayer = layers.end();
                                thisLayer--;
                                mayNeedToSave = true;
                            }
                            else if (brushID == 11) {
                                (*thisLayer) -> setArea();
                            }
                            else if ((*thisLayer) -> getIsEditable() || brushID == 3) {
                                intVector2 tile = (*thisLayer) -> getMouseTile();
                                mousePos.push_back(tile);
                                markers.addEntity(new dummyEntity(tile.x + (*thisLayer) -> getX(), tile.y + (*thisLayer) -> getY(), {255, 0, 0, 255}, (*thisLayer) -> getSizeFactor(), 'X'));
                            }
                            else {
                                intVector2 tile = (*thisLayer) -> getMouseTile();
                                markers.addEntity(new dummyEntity(tile.x + (*thisLayer) -> getX(), tile.y + (*thisLayer) -> getY(), {255, 0, 0, 255}, (*thisLayer) -> getSizeFactor(), '?', 30));
                            }
                        }
                    }

                    //If the current stroke is complete

                    if (mousePos.size() >= brushClickCount) {
                        if (brushID == 3) {
                            (*thisLayer) -> move(mousePos);
                            mayNeedToSave = true;
                        }
                        else if ((*thisLayer) -> getIsEditable()) {
                            (*thisLayer) -> leftBrush(mousePos, brushID, charFills[palette[paletteSelection + 22 * colliderSelected]], density, absoluteBrush);
                            mayNeedToSave = true;
                        }
                        markers.clear();
                        mousePos.clear();
                    }
                }

                //display the world

                theScreen -> start(false);
                for (auto printIter = layers.begin(); printIter != layers.end(); printIter++) {
                    (*printIter) -> print();
                }

                //display brush palette

                for (int i = 0; i < 12; i++) {
                    int codePointToDisplay = charFills[palette[i + 22 * colliderSelected]] -> get(-1, -1);
                    char* temp = TextToUtf8(&codePointToDisplay, 1);
                    if (i == paletteSelection) {
                        theScreen -> drawHud(i * 2 + 1, 1, (Color){255, 0, 0, 255}, temp);
                    }
                    else {
                        theScreen -> drawHud(i * 2 + 1, 1, (Color){255, 255, 255, 255}, temp);
                    }
                    free(temp);
                }

                for (int i = 12; i < 22; i++) {
                    int codePointToDisplay = charFills[palette[i + 22 * colliderSelected]] -> get(-1, -1);
                    char* temp = TextToUtf8(&codePointToDisplay, 1);
                    if (i == paletteSelection) {
                        theScreen -> drawHud(1 + (i - 12) * 2, 3, (Color){255, 0, 0, 255}, temp);
                    }
                    else {
                        theScreen -> drawHud(1 + (i - 12) * 2, 3, (Color){255, 255, 255, 255}, temp);
                    }
                    free(temp);
                }

                //display cursor markers

                markers.print();
                theScreen -> end();

            }
        }
        if (mayNeedToSave) {
            BeginDrawing();
            ClearBackground(UIBACKGROUND);
            theScreen -> drawHud(1, 1, UIFOREGROUND, "You didn't save the level. Do you want to save? Y/S or N/ESC.");
            EndDrawing();
            if (IsKeyPressed(KEY_Y) || IsKeyPressed(KEY_S)) {
                level.writeEntities(layers, background);
                mayNeedToSave = false;
            }
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_N)) {
                mayNeedToSave = false;
            }
        }
        else {
            break;
        }
    }

    CloseWindow();
}

