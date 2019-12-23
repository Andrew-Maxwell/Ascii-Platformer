#include "platformer.h"

#define CAMERASPEED 1

using namespace std;

/*****************************************************************************/
//dummyEntity
//An entity that doesn't do anything. Used to display locations of actual entities in the editor.
/*****************************************************************************/

class dummyEntity : public entity {

    char toPrint[2] = " ";

    public:

    dummyEntity(  float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                    float newSizeFactor, char newC) :
        entity(newX, newY, R, G, B, A, newSizeFactor) { 
            toPrint[0] = newC;
        }

    void tickSet(collider& col) {}

    void tickGet(collider& col) {}

    bool finalize() {return false;}

    void print(float cameraX, float cameraY, Font displayFont)  {
        DrawTextEx(displayFont, toPrint, (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
    }
};


/*****************************************************************************/
//cursor
//Used to show the first location clicked.
/*****************************************************************************/

class cursor : public entity {

    char toPrint[2] = "X";
    bool visible = false;

    public:

    cursor(  float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                    float newSizeFactor) :
        entity(newX, newY, R, G, B, A, newSizeFactor) {}

    void show(int tileX1, int tileY1, float newSizeFactor) {
        x = tileX1;
        y = tileY1;
        sizeFactor = newSizeFactor;
        visible = true;
    }

    void hide() {
        visible = false;
    }

    void tickSet(collider& col) {}

    void tickGet(collider& col) {}

    bool finalize() {return false;}

    void print(float cameraX, float cameraY, Font displayFont)  {
        if (visible) {
            DrawTextEx(displayFont, toPrint, (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
        }
    }
};

/*****************************************************************************/
//editableLayer
//A layer designed to be modified, then saved.
/*****************************************************************************/

class editableLayer : public layer {

    int flashCount = -1;
    bool visible = true;
    Color original;
    vector<vector<int*>> frames;
    int width, knownWidth;
    int tileX1, tileY1, tileX2, tileY2;
    int currentFrame = 0, lastFrame = 0;

    public:

    editableLayer( float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, string newFileName) :
        layer(newX, newY, R, G, B, A, newSizeFactor, newFileName),
        entity(newX, newY, R, G, B, A, newSizeFactor) {

        original = {R, G, B, A};
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

    //Accessors

    float getX() {
        return x;
    }

    float getY() {
        return y;
    }

    float getSizeFactor() {
        return sizeFactor;
    }

    //Propogate changes from intCanvas to the ordinary canvas (which is only used for display.)

    void update() {
        canvas.clear();
        for (int i = 0; i < frames[currentFrame].size(); i++) {
            char* lineC = TextToUtf8(frames[currentFrame][i], knownWidth);
            canvas.push_back(lineC);
         //  cout << "\t" << intCanvas[i] << endl;
        }
    }

    //Set the variable to be displayed in red for a brief period.

    void flash() {
        tint = {255, 0, 0, 255};
        flashCount = 15;
    }

    //Toggle hiding.

    void toggleHide() {
        visible = !visible;
    }

    //Determine which tile a given mouse position on the screen is in.

    float getTileX(float cameraX, float mouseX) {
        int tileX = mouseX / FONTSIZE / sizeFactor + cameraX - SCREENCOLS / sizeFactor / 2 + x;
        if (tileX < 0) {
            return 0;
        }
        if (tileX > knownWidth - 1) {
            return knownWidth - 1;
        }
        return tileX;
    }

    float getTileY(float cameraY, float mouseY) {
        int tileY = mouseY / FONTSIZE / sizeFactor + cameraY - SCREENROWS / sizeFactor / 2 + y;
        if (tileY < 0) {
            return 0;
        }
        if (tileY > frames[0].size() - 1) {
            return frames[0].size() - 1;
        }
        return tileY;
    }

    //Apply changes using left mouse button.

    void leftBrush(int tileX1, int tileY1, int tileX2, int tileY2, int brush) {
        vector<int*> intCanvas;
        for (int* line : frames[currentFrame]) {
            int* newLine = new int[knownWidth];
            for (int i = 0; i < knownWidth; i++) {
                newLine[i] = line[i];
            }
            intCanvas.push_back(newLine);
        }
        for (int i = max(0, min(tileY1, tileY2)); i != min((int)intCanvas.size() - 1, max(tileY1, tileY2)) + 1; i++) {
            for (int j = max(0, min(tileX1, tileX2)); j != min(knownWidth, max(tileX1, tileX2)) + 1; j++) {
                intCanvas[i][j] = brush;
            }
        }
        while (frames.size() > currentFrame + 1) {
            frames.pop_back();
        }
        currentFrame++;
        frames.push_back(intCanvas);
        update();
    }

    //Select a tile to the palette (using right mouse button.

    int sample(int tileX, int tileY) {
        return frames[currentFrame][tileY][tileX];
    }

    //Undo and redo

    void undo() {
        if (currentFrame > 0) {
            currentFrame--;
        }
        update();
    }

    void redo() {
        if (currentFrame < frames.size() - 1) {
            currentFrame++;
        }
        update();
    }

    //Save

    void save() {
        cout << "Saving layer " << fileName << endl;
        ofstream outfile;
        outfile.open(fileName);
        for (string line : canvas) {
            outfile << line << endl;
        }
        outfile.close();
    }

    //Display

    void print(float cameraX, float cameraY, Font displayFont) {
        if (flashCount-- == 0) {
            tint = original;
        }
        if (visible) {
            layer::print(cameraX, cameraY, displayFont);
        }
    }
};

/******************************************************************************/
//Read in a list of entities from a file
/******************************************************************************/

void readEntities(entityList& el, list<editableLayer*>& layers, editableLayer*& col, Color& background, string fileName) {
    ifstream entityFile;
    entityFile.open(fileName);
    if (!entityFile) {
        cerr << "Error opening entity file. Attempting to open" << fileName;
        exit(EXIT_FAILURE);
    }
    char c;
    float x, y, sizeFactor;
    int R, G, B, A;
    string colliderFileName;
    entityFile >> colliderFileName >> R >> G >> B >> A;
    background = {R, G, B, A};
    while (entityFile >> c >> x >> y >> R >> G >> B >> A >> sizeFactor) {
        cout << "Read in a " << c << endl;
        if (c == 'L') {
            string fileName;
            entityFile >> fileName;
            editableLayer * L = new editableLayer(x, y, R, G, B, A, sizeFactor, fileName);
            el.addEntity(L);
            layers.push_back(L);
        }
        else {
            dummyEntity* e = new dummyEntity(x, y, R, G, B, A, sizeFactor, c);
            el.addEntity(e);
        }
        entityFile.ignore(10000, '\n');
    }
    col = new editableLayer(0.0, 0.0, 0, 0, 0, 80, 1, colliderFileName);
    el.addEntity(col);
    layers.push_back(col);
    entityFile.close();
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int main(int argc, char** argv) {

    //Initialize raylib

    InitWindow(SCREENWIDTH, SCREENHEIGHT, "ASCII Platformer - level editor");
    SetTargetFPS(60);

    Font displayFont = LoadFontEx("PxPlus_IBM_BIOS_extended.ttf", 8, FONTCHARS, NUMCHARS);

    //Camera initializations

    float cameraX = SCREENROWS / 2;
    float cameraY = SCREENCOLS / 2;

    //Mouse interface variables

    int tileX1, tileY1, tileX2, tileY2;
    bool mouseLeftDown = false;
    cursor* marker = new cursor(0, 0, 255, 0, 0, 255, 1);
    int brushChar[44] = {0x2581, 0x2582, 0x2583, 0x2584, 0x2585, 0x2586, 0x2587, 0x2588, 0x2591, 0x2592, 0x2593, 0x25e2, 0x25e3, 0x25e4, 0x25e5, 0x2215, 0x007c, 0x005b, 0x005c, 0x005d, 0x0, 0x0, 's', 'w', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', '2', '3', '4', '5', '6', '7', '8', ' ', ' '};
    int selectedBrush = 0;
    int selectedPalette = 0;

    while (!WindowShouldClose()) {

        list<editableLayer*> layers;
        entityList entities;
        editableLayer* col;
        Color background;
        string fileName(argv[1]);
        readEntities(entities, layers, col, background, fileName);
        entities.addEntity(marker);
        list<editableLayer*>::iterator thisLayer = layers.begin();
        float speedMult;

        while (!WindowShouldClose() || IsKeyPressed(KEY_R)) {

            //Character selector

            if (IsKeyDown(KEY_TAB)) {
                BeginDrawing();
                ClearBackground(DARKGRAY);
                int j = 0;
                for (int i = 0; i < NUMCHARS; i++) {
                    DrawTextEx(displayFont, TextToUtf8(&FONTCHARS[i], 1), (Vector2){(i % (SCREENCOLS / 2)) * 2 * FONTSIZE, i / (SCREENCOLS / 2) * 2 * FONTSIZE}, FONTSIZE, 0, WHITE);
                }
                EndDrawing();
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    Vector2 mouse = GetMousePosition();
                    int selection = (int)mouse.y / (2 * FONTSIZE) * (SCREENCOLS / 2) + (int)mouse.x / (2 * FONTSIZE);
                    if (selection < NUMCHARS) {
                        brushChar[selectedBrush + 22 * selectedPalette] = FONTCHARS[selection];
                    }
                }
            }
            else {

                //Meta: Undo, redo, save

                if (IsKeyDown(KEY_LEFT_CONTROL)) {
                    if (IsKeyPressed(KEY_Z)) {
                        (*thisLayer) -> undo();
                    }
                    if (IsKeyPressed(KEY_Y)) {
                        (*thisLayer) -> redo();
                    }
                    if (IsKeyPressed(KEY_S)) {
                        list<editableLayer*>::iterator saveIter = layers.begin();
                        while (saveIter != layers.end()) {
                            (*saveIter) -> save();
                            saveIter++;
                        }
                    }
                }
                else {

                    //Camera movement

                    if (IsKeyDown(KEY_LEFT_SHIFT)) {
                        speedMult = 8;
                    }
                    else {
                        speedMult = 1;
                    }
                    if (IsKeyDown(KEY_RIGHT)) {
                        cameraX += CAMERASPEED * speedMult;
                    }
                    if (IsKeyDown(KEY_LEFT)) {
                        cameraX -= CAMERASPEED * speedMult;
                    }
                    if (IsKeyDown(KEY_UP)) {
                        cameraY -= CAMERASPEED * speedMult;
                    }
                    if (IsKeyDown(KEY_DOWN)) {
                        cameraY += CAMERASPEED * speedMult;
                    }

                    //switch to prev layer, then flash

                    if (IsKeyPressed(KEY_X)) {
                        if (thisLayer != layers.begin()) {
                            thisLayer--;
                        }
                        (*thisLayer) -> flash();
                        selectedPalette = 0;
                    }

                    //flash current layer
            /*
                    if (IsKeyPressed(KEY_X)) {
                        (*thisLayer) -> flash();
                    }
            */
                    //switch to next layer, then flash

                    if (IsKeyPressed(KEY_C)) {
                        if (++thisLayer == layers.end()) {
                            thisLayer--;
                        }
                        if (++thisLayer == layers.end()) {
                            selectedPalette = 1;
                        }
                        thisLayer--;
                        (*thisLayer) -> flash();
                    }

                    //hide layer

                    if (IsKeyPressed(KEY_H)) {
                        (*thisLayer) -> toggleHide();
                    }

                    //select brushes

                    if (IsKeyPressed(KEY_F1)) {
                        selectedBrush = 0;
                    }
                    if (IsKeyPressed(KEY_F2)) {
                        selectedBrush = 1;
                    }
                    if (IsKeyPressed(KEY_F3)) {
                        selectedBrush = 2;
                    }
                    if (IsKeyPressed(KEY_F4)) {
                        selectedBrush = 3;
                    }
                    if (IsKeyPressed(KEY_F5)) {
                        selectedBrush = 4;
                    }
                    if (IsKeyPressed(KEY_F6)) {
                        selectedBrush = 5;
                    }
                    if (IsKeyPressed(KEY_F7)) {
                        selectedBrush = 6;
                    }
                    if (IsKeyPressed(KEY_F8)) {
                        selectedBrush = 7;
                    }
                    if (IsKeyPressed(KEY_F9)) {
                        selectedBrush = 8;
                    }
                    if (IsKeyPressed(KEY_F10)) {
                        selectedBrush = 9;
                    }
                    if (IsKeyPressed(KEY_F11)) {
                        selectedBrush = 10;
                    }
                    if (IsKeyPressed(KEY_F12)) {
                        selectedBrush = 11;
                    }
                    if (IsKeyPressed(KEY_ONE)) {
                        selectedBrush = 12;
                    }
                    if (IsKeyPressed(KEY_TWO)) {
                        selectedBrush = 13;
                    }
                    if (IsKeyPressed(KEY_THREE)) {
                        selectedBrush = 14;
                    }
                    if (IsKeyPressed(KEY_FOUR)) {
                        selectedBrush = 15;
                    }
                    if (IsKeyPressed(KEY_FIVE)) {
                        selectedBrush = 16;
                    }
                    if (IsKeyPressed(KEY_SIX)) {
                        selectedBrush = 17;
                    }
                    if (IsKeyPressed(KEY_SEVEN)) {
                        selectedBrush = 18;
                    }
                    if (IsKeyPressed(KEY_EIGHT)) {
                        selectedBrush = 19;
                    }
                    if (IsKeyPressed(KEY_NINE)) {
                        selectedBrush = 20;
                    }
                    if (IsKeyPressed(KEY_ZERO)) {
                        selectedBrush = 21;
                    }

                    //mouse input

                    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                        Vector2 mouse = GetMousePosition();
                        tileX1 = (*thisLayer) -> getTileX(cameraX, mouse.x);
                        tileY1 = (*thisLayer) -> getTileY(cameraY, mouse.y);
                        brushChar[selectedBrush + 22 * selectedPalette] = (*thisLayer) -> sample(tileX1, tileY1);
                    }

                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        Vector2 mouse = GetMousePosition();
                        tileX1 = (*thisLayer) -> getTileX(cameraX, mouse.x);
                        tileY1 = (*thisLayer) -> getTileY(cameraY, mouse.y);
                        marker -> show(tileX1 - (*thisLayer) -> getX(), tileY1 - (*thisLayer) -> getY(), (*thisLayer) -> getSizeFactor());
                    }

                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                        Vector2 mouse = GetMousePosition();
                        tileX2 = (*thisLayer) -> getTileX(cameraX, mouse.x);
                        tileY2 = (*thisLayer) -> getTileY(cameraY, mouse.y);
                        marker -> hide();
                        (*thisLayer) -> leftBrush(tileX1, tileY1, tileX2, tileY2, brushChar[selectedBrush + 22 * selectedPalette]);
                    }
                }

                //display the world

                BeginDrawing();
                ClearBackground(background);
                entities.print(cameraX, cameraY, displayFont);

                //display brush palette

                for (int i = 0; i < 12; i++) {
                    if (i == selectedBrush) {
                        DrawTextEx(displayFont, TextToUtf8(&brushChar[i + 22 * selectedPalette], 1), (Vector2){10 + i * 20, 10}, FONTSIZE, 0, {255, 0, 0, 255});
                    }
                    else {
                        DrawTextEx(displayFont, TextToUtf8(&brushChar[i + 22 * selectedPalette], 1), (Vector2){10 + i * 20, 10}, FONTSIZE, 0, {255, 255, 255, 255});
                    }
                }

                for (int i = 12; i < 22; i++) {
                    if (i == selectedBrush) {
                        DrawTextEx(displayFont, TextToUtf8(&brushChar[i + 22 * selectedPalette], 1), (Vector2){10 + (i - 12) * 20, 30}, FONTSIZE, 0, {255, 0, 0, 255});
                    }
                    else {
                        DrawTextEx(displayFont, TextToUtf8(&brushChar[i + 22 * selectedPalette], 1), (Vector2){10 + (i - 12) * 20, 30}, FONTSIZE, 0, {255, 255, 255, 255});
                    }
                }

                EndDrawing();
            }
        }
    }

    //final clean up

    CloseWindow();
}

