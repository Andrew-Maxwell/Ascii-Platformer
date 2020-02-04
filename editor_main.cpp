#include "meta.hpp"
#include "editor_classes.hpp"

using namespace rapidjson;

/******************************************************************************/
//Read in a list of entities from a file
/******************************************************************************/

void readEntities(entityList& el, list<editableLayer*>& layers, editableCollider*& col, Color& background, string& fileName, Document& doc) {

    //Read into a json object

    FILE* entityFile = getLevelFileP(fileName);
    fseek (entityFile, 0, SEEK_END);
    char * buffer = new char[ftell (entityFile)];
    fseek (entityFile, 0, SEEK_SET);
    FileReadStream entityReadStream(entityFile, buffer, sizeof(buffer));
    if (doc.ParseStream(entityReadStream).HasParseError()) {
        cerr << "Error parsing json.\n" << endl;
    }
    fclose(entityFile);

    //Read level background color

    int backgroundR = doc.HasMember("R") ? doc["R"].GetInt() : 255;
    int backgroundG = doc.HasMember("G") ? doc["G"].GetInt() : 255;
    int backgroundB = doc.HasMember("B") ? doc["B"].GetInt() : 255;
    int backgroundA = doc.HasMember("A") ? doc["A"].GetInt() : 255;
    string colFileName = doc.HasMember("collider") ? doc["collider"].GetString() : "test_collider.txt";
    background = {backgroundR, backgroundG, backgroundB, backgroundA};

    //Get the list of entities

    Value& entities = doc["entities"];
    assert(entities.IsArray());

    for (SizeType i = 0; i < entities.Size(); i++) {

        Value& entity = entities[i];
        assert(entity.IsObject());

        //Read in generic data values (almost all entities will have these)

        string type = entity.HasMember("type") ? entity["type"].GetString() : "?";
        cout << "Reading entity " << type << endl;
        float x = entity.HasMember("x") ? entity["x"].GetFloat() : 0.0;
        float y = entity.HasMember("y") ? entity["y"].GetFloat() : 0.0;
        int R = entity.HasMember("R") ? entity["R"].GetInt() : 0;
        int G = entity.HasMember("G") ? entity["G"].GetInt() : 0;
        int B = entity.HasMember("B") ? entity["B"].GetInt() : 0;
        int A = entity.HasMember("A") ? entity["A"].GetInt() : 0;
        float sizeFactor = entity.HasMember("sizeFactor") ? entity["sizeFactor"].GetFloat() : 1.0;

        string fileName = entity.HasMember("fileName") ? entity["fileName"].GetString() : "No filename found.";
        editableLayer * L = new editableLayer(x, y, R, G, B, A, sizeFactor, (type == "layer"), fileName, toupper(type[0]), &entity);
        layers.push_back(L);
        el.addEntity(L);
    }
    col = new editableCollider(0.0, 0.0, 0, 0, 0, 80, 1, true, colFileName, '!', NULL);
    el.addEntity(col);
    layers.push_back(col);
}

//Save all the data

void writeEntities(entityList& el, list<editableLayer*>& layers, const string fileName, Document& doc) {
    cout << "Saving file...\n";
    
    //Save layer files
    
    Value& entities = doc["entities"];
    assert(entities.IsArray());
    list<editableLayer*>::iterator saveIter = layers.begin();
    while (saveIter != layers.end()) {
        (*saveIter) -> save();
        saveIter++;
    }
    
    cout << "Finished saving layers.\n";

    //Save entity metadata (to json)
    
    FILE* entityFile = fopen(fileName.c_str(), "wb");
    char buffer[65536];
    FileWriteStream entityWriteStream(entityFile, buffer, sizeof(buffer));
    PrettyWriter<FileWriteStream> writer(entityWriteStream);
    doc.Accept(writer);
    fclose(entityFile);
}

/******************************************************************************/
//Main loop
/******************************************************************************/

int main(int argc, char** argv) {

    //Initialize raylib

    InitWindow(SCREENWIDTH, SCREENHEIGHT, "ASCII Platformer - level editor");
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    SetTargetFPS(60);
    SetExitKey(KEY_RIGHT_CONTROL);

    Font displayFont = LoadFontEx(FONTNAME, 8, FONTCHARS, NUMCHARS);

    //Camera initializations

    float cameraX = SCREENROWS / 2;
    float cameraY = SCREENCOLS / 2;

    //Mouse interface variables

    vector<tuple<int, int>> mousePos;
    entityList markers;
    float oldMouseX, oldMouseY, oldCameraX, oldCameraY;

    //Clipboard

    vector<vector<int>> clipboard({{}});

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
    int paletteSwitch = 0;

    //Brush variables - analogous to brush shape, size, etc. in an image editor

    int brushID = 0;
    int brushClickCount = 1;
    string brushName = "(P)encil";
    float density = 1;

    //mayNeedToSave?

    bool mayNeedToSave = false;
    bool exit = false;

    //Level data

    list<editableLayer*> layers;
    entityList entities;
    editableCollider* col;
    Color background;
    string fileName(argv[1]);
    Document json;
    
    cout << "Starting loading entities...\n";
    
    readEntities(entities, layers, col, background, fileName, json);
    list<editableLayer*>::iterator thisLayer = layers.begin();
    (*thisLayer) -> select();
    float speedMult;
    
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
                BeginDrawing();
                ClearBackground(DARKGRAY);

                //Print out all available charFills

                for (int i = 0; i < charFills.size(); i++) {
                    int codePointToDisplay =  charFills[i] -> get(-1, -1);
                    myDrawText(displayFont, TextToUtf8(&codePointToDisplay, 1), (Vector2){(i % (SCREENCOLS / 2)) * 2 * FONTSIZE, i / (SCREENCOLS / 2) * 2 * FONTSIZE}, FONTSIZE, 0, WHITE);
                }

                //Print out currently selected brush

                myDrawText(displayFont, (brushName + " Density: " + to_string(density)).c_str(), (Vector2){FONTSIZE, SCREENHEIGHT - FONTSIZE * 2}, FONTSIZE, 0, WHITE);

                EndDrawing();

                //If a character is clicked, add it to the palette

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    Vector2 mouse = GetMousePosition();
                    int selection = (int)mouse.y / (2 * FONTSIZE) * (SCREENCOLS / 2) + (int)mouse.x / (2 * FONTSIZE);
                    if (selection > 0 && selection < charFills.size()) {
                        palette[paletteSelection + 22 * paletteSwitch] = selection;
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
            }
            else {

                //Meta: Undo, redo, save

                if (IsKeyDown(KEY_LEFT_CONTROL)) {
                    if (IsKeyPressed(KEY_Z)) {
                        if (mousePos.size() > 0) {
                            mousePos.clear();
                            markers.clear();
                        }
                        else if ((*thisLayer) -> getIsLayer()) {
                            (*thisLayer) -> undo();
                            mayNeedToSave = true;
                        }
                    }
                    if (IsKeyPressed(KEY_Y) && (*thisLayer) -> getIsLayer()) {
                        (*thisLayer) -> redo();
                        mayNeedToSave = true;
                    }
                    if (IsKeyPressed(KEY_S)) {
                        writeEntities(entities, layers, fileName, json);
                        mayNeedToSave = false;
                    }
                    if (IsKeyPressed(KEY_X) && (*thisLayer) -> getIsLayer() && mousePos.size() > 1) {
                        clipboard = (*thisLayer) -> cut(mousePos);
                        markers.clear();
                        mousePos.clear();
                        mayNeedToSave = true;
                    }
                    if (IsKeyPressed(KEY_C) && (*thisLayer) -> getIsLayer() && mousePos.size() > 1) {
                        clipboard = (*thisLayer) -> copy(mousePos);
                        markers.clear();
                        mousePos.clear();
                    }
                    if (IsKeyPressed(KEY_V) && (*thisLayer) -> getIsLayer()) {
                        if (mousePos.size() == 0) {
                            mousePos.push_back(make_tuple(0, 0));    //Paste in upper right by default
                        }
                        (*thisLayer) -> paste(mousePos, clipboard);
                        markers.clear();
                        mousePos.clear();
                        mayNeedToSave = true;
                    }
                    if (IsKeyPressed(KEY_A) && (*thisLayer) -> getIsLayer()) {
                        brushName = "Select";
                        brushID = 8;
                        brushClickCount = 100000;
                        mousePos.push_back(make_tuple(0, 0));
                        mousePos.push_back(make_tuple((*thisLayer) -> getCols() - 1, (*thisLayer) -> getRows() - 1));
                    }
                }
                
                else {  //Control is not down

                    //Camera movement

                    if (IsKeyDown(KEY_LEFT_SHIFT)) {
                        speedMult = 8;
                    }
                    else {
                        speedMult = 1;
                    }
                    if (IsKeyDown(KEY_D)) {
                        cameraX += CAMERASPEED * speedMult;
                    }
                    if (IsKeyDown(KEY_A)) {
                        cameraX -= CAMERASPEED * speedMult;
                    }
                    if (IsKeyDown(KEY_W)) {
                        cameraY -= CAMERASPEED * speedMult;
                    }
                    if (IsKeyDown(KEY_S)) {
                        cameraY += CAMERASPEED * speedMult;
                    }

                    //Click and drag camera movement

                    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {     //When the drag begins
                        oldCameraX = cameraX;
                        oldCameraY = cameraY;
                        Vector2 mouse = GetMousePosition();
                        oldMouseX = mouse.x;
                        oldMouseY = mouse.y;
                    }

                    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {        //While dragging
                        Vector2 mouse = GetMousePosition();
                        cameraX = oldCameraX - (mouse.x - oldMouseX) / FONTSIZE;
                        cameraY = oldCameraY - (mouse.y - oldMouseY) / FONTSIZE;
                    }

                    //switch to prev layer, then flash

                    if (IsKeyPressed(KEY_X)) {
                        (*thisLayer) -> deselect();
                        if (thisLayer != layers.begin()) {
                            thisLayer--;
                        }
                        (*thisLayer) -> select();
                        (*thisLayer) -> flash();
                        paletteSwitch = 0;
                        markers.clear();
                        mousePos.clear();
                    }

                    //switch to next layer, then flash

                    if (IsKeyPressed(KEY_C)) {
                        (*thisLayer) -> deselect();
                        if (++thisLayer == layers.end()) {
                            thisLayer--;
                        }
                        if (++thisLayer == layers.end()) {
                            paletteSwitch = 1;
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

                    if (IsKeyPressed(KEY_F1)) {
                        paletteSelection = 0;
                    }
                    if (IsKeyPressed(KEY_F2)) {
                        paletteSelection = 1;
                    }
                    if (IsKeyPressed(KEY_F3)) {
                        paletteSelection = 2;
                    }
                    if (IsKeyPressed(KEY_F4)) {
                        paletteSelection = 3;
                    }
                    if (IsKeyPressed(KEY_F5)) {
                        paletteSelection = 4;
                    }
                    if (IsKeyPressed(KEY_F6)) {
                        paletteSelection = 5;
                    }
                    if (IsKeyPressed(KEY_F7)) {
                        paletteSelection = 6;
                    }
                    if (IsKeyPressed(KEY_F8)) {
                        paletteSelection = 7;
                    }
                    if (IsKeyPressed(KEY_F9)) {
                        paletteSelection = 8;
                    }
                    if (IsKeyPressed(KEY_F10)) {
                        paletteSelection = 9;
                    }
                    if (IsKeyPressed(KEY_F11)) {
                        paletteSelection = 10;
                    }
                    if (IsKeyPressed(KEY_F12)) {
                        paletteSelection = 11;
                    }
                    if (IsKeyPressed(KEY_ONE)) {
                        paletteSelection = 12;
                    }
                    if (IsKeyPressed(KEY_TWO)) {
                        paletteSelection = 13;
                    }
                    if (IsKeyPressed(KEY_THREE)) {
                        paletteSelection = 14;
                    }
                    if (IsKeyPressed(KEY_FOUR)) {
                        paletteSelection = 15;
                    }
                    if (IsKeyPressed(KEY_FIVE)) {
                        paletteSelection = 16;
                    }
                    if (IsKeyPressed(KEY_SIX)) {
                        paletteSelection = 17;
                    }
                    if (IsKeyPressed(KEY_SEVEN)) {
                        paletteSelection = 18;
                    }
                    if (IsKeyPressed(KEY_EIGHT)) {
                        paletteSelection = 19;
                    }
                    if (IsKeyPressed(KEY_NINE)) {
                        paletteSelection = 20;
                    }
                    if (IsKeyPressed(KEY_ZERO)) {
                        paletteSelection = 21;
                    }

                    //Adjusting color

                    if (IsKeyDown(KEY_R)) {
                        Color newTint = (*thisLayer) -> getColor();
                        if (IsKeyDown(KEY_LEFT_SHIFT) && newTint.r < 255) {
                            newTint.r++;
                        }
                        else if (newTint.r > 0){
                            newTint.r--;
                        }
                        (*thisLayer) -> setColor(newTint);
                        mayNeedToSave = true;
                    }

                    if (IsKeyDown(KEY_G)) {
                        Color newTint = (*thisLayer) -> getColor();
                        if (IsKeyDown(KEY_LEFT_SHIFT) && newTint.g < 255) {
                            newTint.g++;
                        }
                        else if (newTint.g > 0){
                            newTint.g--;
                        }
                        (*thisLayer) -> setColor(newTint);
                        mayNeedToSave = true;
                    }

                    if (IsKeyDown(KEY_B)) {
                        Color newTint = (*thisLayer) -> getColor();
                        if (IsKeyDown(KEY_LEFT_SHIFT) && newTint.b < 255) {
                            newTint.b++;
                        }
                        else if (newTint.b > 0) {
                            newTint.b--;
                        }
                        (*thisLayer) -> setColor(newTint);
                        mayNeedToSave = true;
                    }

                    if (IsKeyDown(KEY_T)) {
                        Color newTint = (*thisLayer) -> getColor();
                        if (IsKeyDown(KEY_LEFT_SHIFT) && newTint.a < 255) {
                            newTint.a++;
                        }
                        else if (newTint.a > 0) {
                            newTint.a--;
                        }
                        (*thisLayer) -> setColor(newTint);
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
                    /*
                    if (IsKeyPressed(KEY_ENTER)) {
                        cout << "{\n";
                        for (vector<int> row : clipboard) {
                            cout << "{";
                            for(int i = 0; i < row.size() - 1; i++) {
                                cout << hex << row[i] << ", ";
                            }
                            cout << hex << row[row.size() - 1] << "}\n";
                        }
                        cout << "}";
                        charFills.push_back(new textureCharFill(clipboard));
                    }
                    */
                    //mouse input: Right click to sample character

                    if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) && (*thisLayer) -> getIsLayer()) {
                        Vector2 mouse = GetMousePosition();
                        int tileX1 = (*thisLayer) -> getTileX(cameraX, mouse.x);
                        int tileY1 = (*thisLayer) -> getTileY(cameraY, mouse.y);
                        palette[paletteSelection + 22 * paletteSwitch] = GetGlyphIndex(displayFont, (*thisLayer) -> sample(tileX1, tileY1));
                    }

                    //Left click to add a brush input point

                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && ((*thisLayer) -> getIsLayer() || brushID == 3)) {
                        Vector2 mouse = GetMousePosition();
                        int tileX1 = (*thisLayer) -> getTileX(cameraX, mouse.x);
                        int tileY1 = (*thisLayer) -> getTileY(cameraY, mouse.y);
                        mousePos.push_back(make_tuple(tileX1, tileY1));
                        markers.addEntity(new dummyEntity(tileX1 - (*thisLayer) -> getX(), tileY1 - (*thisLayer) -> getY(), 255, 0, 0, 255, (*thisLayer) -> getSizeFactor(), 'X'));
                    }

                    //If the current stroke is complete

                    if (mousePos.size() >= brushClickCount) {
                        if (brushID == 3) {
                            (*thisLayer) -> move(mousePos);
                            mayNeedToSave = true;
                        }
                        else if ((*thisLayer) -> getIsLayer()) {
                            (*thisLayer) -> leftBrush(mousePos, brushID, charFills[palette[paletteSelection + 22 * paletteSwitch]], density);
                            mayNeedToSave = true;
                        }
                        markers.clear();
                        mousePos.clear();
                    }
                }

                //display the world

                BeginDrawing();
                ClearBackground(background);
                entities.print(cameraX, cameraY, displayFont);

                //display brush palette

                for (int i = 0; i < 12; i++) {
                    int codePointToDisplay = charFills[palette[i + 22 * paletteSwitch]] -> get(-1, -1);
                    if (i == paletteSelection) {
                        myDrawText(displayFont, TextToUtf8(&codePointToDisplay, 1), (Vector2){10 + i * 20, 10}, FONTSIZE, 0, {255, 0, 0, 255});
                    }
                    else {
                        myDrawText(displayFont, TextToUtf8(&codePointToDisplay, 1), (Vector2){10 + i * 20, 10}, FONTSIZE, 0, {255, 255, 255, 255});
                    }
                }

                for (int i = 12; i < 22; i++) {
                    int codePointToDisplay = charFills[palette[i + 22 * paletteSwitch]] -> get(-1, -1);
                    if (i == paletteSelection) {
                        myDrawText(displayFont, TextToUtf8(&codePointToDisplay, 1), (Vector2){10 + (i - 12) * 20, 30}, FONTSIZE, 0, {255, 0, 0, 255});
                    }
                    else {
                        myDrawText(displayFont, TextToUtf8(&codePointToDisplay, 1), (Vector2){10 + (i - 12) * 20, 30}, FONTSIZE, 0, {255, 255, 255, 255});
                    }
                }

                //display cursor markers

                markers.print(cameraX, cameraY, displayFont);

                EndDrawing();
            }
        }
        if (mayNeedToSave) {
            BeginDrawing();
            ClearBackground(BLACK);
            myDrawText(displayFont, "You didn't save the level. Do you want to save? Y/S or N/ESC.", (Vector2){FONTSIZE, FONTSIZE}, FONTSIZE, 0, WHITE);
            EndDrawing();
            if (IsKeyPressed(KEY_Y) || IsKeyPressed(KEY_S)) {
                writeEntities(entities, layers, fileName, json);
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

