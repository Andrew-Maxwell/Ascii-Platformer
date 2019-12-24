#include "platformer.h"

#define CAMERASPEED 1

using namespace std;

/*****************************************************************************/
//charFill
//Brush defines shape, charFill defines "color": what characters the brush area should be
//filled with.
/*****************************************************************************/

struct charFill {

    int codepoint;

    charFill(int newCodepoint) {
        codepoint = newCodepoint;
    }

    int get(int x, int y) {
        return codepoint;
    }
};

struct randomCharFill : public charFill {

    vector<int> codepoints;

    int get(int x, int y) {
        return codepoints[GetRandomValue(0, codepoints.size() - 1)];
    }
};

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
//editableLayer
//A layer designed to be modified, then mayNeedToSave.
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

    Color getColor() {
        return tint;
    }

    void setColor(Color newColor) {
        tint = newColor;
        original = newColor;
        cout << fileName << " color set to " << (int)tint.r << " " << (int)tint.g << " " << (int)tint.b << " " << (int)tint.a << endl;
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

    //Set the layer to be displayed in red for a brief period.

    void flash() {
        tint = {255, 0, 0, 255};
        flashCount = 10;
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

    void leftBrush(vector<tuple<int, int>> mousePos, int brushID, charFill F) {

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
                intCanvas[get<1>(mousePos[0])][get<0>(mousePos[0])] = F.get(get<0>(mousePos[0]), get<1>(mousePos[0]));
                break;
            }
            case 1: {   //Square brush
                int tileX1 = get<0>(mousePos[0]);
                int tileY1 = get<1>(mousePos[0]);
                int tileX2 = get<0>(mousePos[1]);
                int tileY2 = get<1>(mousePos[1]);
                for (int i = max(0, min(tileY1, tileY2)); i != min((int)intCanvas.size() - 1, max(tileY1, tileY2)) + 1; i++) {
                    for (int j = max(0, min(tileX1, tileX2)); j != min(knownWidth, max(tileX1, tileX2)) + 1; j++) {
                        intCanvas[i][j] = F.get(j, i);
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
                            intCanvas[tileY1 + i][j] = F.get(j, tileY1 + i);
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
                            intCanvas[j][tileX1 + i] = F.get(tileX1 + i, j);
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
//Main loop
/******************************************************************************/

int main(int argc, char** argv) {

    //Initialize raylib

    InitWindow(SCREENWIDTH, SCREENHEIGHT, "ASCII Platformer - level editor");
    SetTargetFPS(60);
    SetExitKey(KEY_RIGHT_CONTROL);

    Font displayFont = LoadFontEx(FONTNAME, 8, FONTCHARS, NUMCHARS);

    //Camera initializations

    float cameraX = SCREENROWS / 2;
    float cameraY = SCREENCOLS / 2;

    //Mouse interface variables

    vector<tuple<int, int>> mousePos;
    entityList markers;

    //charFill variables - analogous to color in an image editor

    vector<charFill> charFills;
    for (int i = 0; i < NUMCHARS; i++) {  //Populate all single-character charFills
        charFills.push_back(charFill(FONTCHARS[i]));
    }
    int palette[44] = {767, 768, 769, 770, 771, 772, 773, 774,775, 776, 777, 778, 779, 780, 781, 782, 783, 784, 785, 786, 787, 788, 115, 46, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57};
    int paletteSelection = 0;
    int paletteSwitch = 0;

    //Brush variables - analogous to brush shape, size, etc. in an image editor

    int brushID = 0;
    int brushClickCount = 1;
    string brushName = "Pencil";

    //mayNeedToSave?

    bool mayNeedToSave = false;
    bool exit = false;

    //Level data

    list<editableLayer*> layers;
    entityList entities;
    editableLayer* col;
    Color background;
    string fileName(argv[1]);
    readEntities(entities, layers, col, background, fileName);
    list<editableLayer*>::iterator thisLayer = layers.begin();
    float speedMult;

    //Main loop

    while (!WindowShouldClose() || mayNeedToSave) {


        while (!(WindowShouldClose() || exit)) {

            if (IsKeyPressed(KEY_ESCAPE)) {
                exit = true;
            }

            //Character selector

            if (IsKeyDown(KEY_TAB)) {
                BeginDrawing();
                ClearBackground(DARKGRAY);

                //Print out all available charFills

                for (int i = 0; i < charFills.size(); i++) {
                    int codePointToDisplay =  charFills[i].get(-1, -1);
                    DrawTextEx(displayFont, TextToUtf8(&codePointToDisplay, 1), (Vector2){(i % (SCREENCOLS / 2)) * 2 * FONTSIZE, i / (SCREENCOLS / 2) * 2 * FONTSIZE}, FONTSIZE, 0, WHITE);
                }

                //Print out currently selected brush

                DrawTextEx(displayFont, brushName.c_str(), (Vector2){FONTSIZE, SCREENHEIGHT - FONTSIZE * 2}, FONTSIZE, 0, WHITE);

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
                    brushName = "Pencil";
                    brushID = 0;
                    brushClickCount = 1;
                }
                if (IsKeyPressed(KEY_S)) {
                    brushName = "Square";
                    brushID = 1;
                    brushClickCount = 2;
                }
                if (IsKeyPressed(KEY_D)) {
                    brushName = "Diamond";
                    brushID = 2;
                    brushClickCount = 2;
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
                        else {
                            (*thisLayer) -> undo();
                        }
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
                        mayNeedToSave = false;
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
                        paletteSwitch = 0;
                        markers.clear();
                        mousePos.clear();
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
                            paletteSwitch = 1;
                        }
                        thisLayer--;
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

                    if (IsKeyPressed(KEY_R)) {
                        Color newTint = (*thisLayer) -> getColor();
                        if (IsKeyDown(KEY_LEFT_SHIFT)) {
                            newTint.r++;
                        }
                        else {
                            newTint.r--;
                        }
                        (*thisLayer) -> setColor(newTint);
                    }

                    if (IsKeyPressed(KEY_G)) {
                        Color newTint = (*thisLayer) -> getColor();
                        if (IsKeyDown(KEY_LEFT_SHIFT)) {
                            newTint.g++;
                        }
                        else {
                            newTint.g--;
                        }
                        (*thisLayer) -> setColor(newTint);
                    }

                    if (IsKeyPressed(KEY_B)) {
                        Color newTint = (*thisLayer) -> getColor();
                        if (IsKeyDown(KEY_LEFT_SHIFT)) {
                            newTint.b++;
                        }
                        else {
                            newTint.b--;
                        }
                        (*thisLayer) -> setColor(newTint);
                    }

                    if (IsKeyPressed(KEY_A)) {
                        Color newTint = (*thisLayer) -> getColor();
                        if (IsKeyDown(KEY_LEFT_SHIFT)) {
                            newTint.a++;
                        }
                        else {
                            newTint.a--;
                        }
                        (*thisLayer) -> setColor(newTint);
                    }

                    //mouse input: Right click to sample character

                    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                        Vector2 mouse = GetMousePosition();
                        int tileX1 = (*thisLayer) -> getTileX(cameraX, mouse.x);
                        int tileY1 = (*thisLayer) -> getTileY(cameraY, mouse.y);
                        palette[paletteSelection + 22 * paletteSwitch] = (*thisLayer) -> sample(tileX1, tileY1);
                    }

                    //Left click to add a brush input point

                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        Vector2 mouse = GetMousePosition();
                        int tileX1 = (*thisLayer) -> getTileX(cameraX, mouse.x);
                        int tileY1 = (*thisLayer) -> getTileY(cameraY, mouse.y);
                        mousePos.push_back(make_tuple(tileX1, tileY1));
                        markers.addEntity(new dummyEntity(tileX1 - (*thisLayer) -> getX(), tileY1 - (*thisLayer) -> getY(), 255, 0, 0, 255, (*thisLayer) -> getSizeFactor(), 'X'));
                    }

                    //If the current stroke is complete

                    if (mousePos.size() >= brushClickCount) {
                        (*thisLayer) -> leftBrush(mousePos, brushID, charFills[palette[paletteSelection + 22 * paletteSwitch]]);
                        markers.clear();
                        mousePos.clear();
                        mayNeedToSave = true;
                    }
                }

                //display the world

                BeginDrawing();
                ClearBackground(background);
                entities.print(cameraX, cameraY, displayFont);

                //display brush palette

                for (int i = 0; i < 12; i++) {
                    int codePointToDisplay = charFills[palette[i + 22 * paletteSwitch]].get(-1, -1);
                    if (i == paletteSelection) {
                        DrawTextEx(displayFont, TextToUtf8(&codePointToDisplay, 1), (Vector2){10 + i * 20, 10}, FONTSIZE, 0, {255, 0, 0, 255});
                    }
                    else {
                        DrawTextEx(displayFont, TextToUtf8(&codePointToDisplay, 1), (Vector2){10 + i * 20, 10}, FONTSIZE, 0, {255, 255, 255, 255});
                    }
                }

                for (int i = 12; i < 22; i++) {
                    int codePointToDisplay = charFills[palette[i + 22 * paletteSwitch]].get(-1, -1);
                    if (i == paletteSelection) {
                        DrawTextEx(displayFont, TextToUtf8(&codePointToDisplay, 1), (Vector2){10 + (i - 12) * 20, 30}, FONTSIZE, 0, {255, 0, 0, 255});
                    }
                    else {
                        DrawTextEx(displayFont, TextToUtf8(&codePointToDisplay, 1), (Vector2){10 + (i - 12) * 20, 30}, FONTSIZE, 0, {255, 255, 255, 255});
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
            DrawTextEx(displayFont, "You didn't save the level. Do you want to save? Y/S or N/ESC.", (Vector2){FONTSIZE, FONTSIZE}, FONTSIZE, 0, WHITE);
            EndDrawing();
            if (IsKeyPressed(KEY_Y) || IsKeyPressed(KEY_S)) {
                list<editableLayer*>::iterator saveIter = layers.begin();
                while (saveIter != layers.end()) {
                    (*saveIter) -> save();
                    saveIter++;
                }
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

