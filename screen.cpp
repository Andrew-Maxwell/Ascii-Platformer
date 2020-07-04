#include "screen.hpp"

    screen::screen() {
        displayFont = LoadFontEx(FONTNAME, 8, FONTCHARS, NUMCHARS);
    }

    void screen::setParams(int newWorldRows, int newWorldCols, int newGameFontSize, int newHudFontSize, float newPlayerSizeFactor, int newDayLength) {
        worldRows = newWorldRows;
        worldCols = newWorldCols;
        fontSize = newGameFontSize;
        hudFontSize = newHudFontSize;
        playerSizeFactor = newPlayerSizeFactor;
        dayLength = newDayLength;

        //Camera initializations

        cameraX = worldCols / 2;
        cameraY = worldRows / 2;
        screenRows = GetScreenHeight() / fontSize;
        screenCols = GetScreenWidth() / fontSize;
        moveCameraX = (worldCols > screenCols / playerSizeFactor);
        moveCameraY = (worldRows > screenRows / playerSizeFactor);
        cameraLagX = screenCols / playerSizeFactor * 3 / 16;
        cameraLagY = screenRows / playerSizeFactor * 3 / 16;
    }

    Color screen::getColor() {
        return background;
    }

    void screen::setColor(Color newColor) {
        background = newColor;
    }

    void screen::setAllColors(Color newDayLight, Color newSunsetLight, Color newNightLight, Color newDawnLight, Color newDayBackground, Color newSunsetBackground, Color newNightBackground, Color newDawnBackground) {
        dayLight = newDayLight;
        sunsetLight = newSunsetLight;
        nightLight = newNightLight;
        dawnLight = newDawnLight;
        dayBackground = newDayBackground;
        sunsetBackground = newSunsetBackground;
        nightBackground = newNightBackground;
        dawnBackground = newDawnBackground;
    }

//Optimized functions
//myGetGlyphIndex is long, so placed at bottom of the file

    void screen::myDrawTexture(Texture2D& texture, Rectangle& sourceRec, Rectangle& destRec, Vector2 origin, float rotation, Color tint)
    {
        // Check if texture is valid
        if (texture.id > 0)
        {
            float width = (float)texture.width;
            float height = (float)texture.height;

            bool flipX = false;

            if (sourceRec.width < 0) { flipX = true; sourceRec.width *= -1; }
            if (sourceRec.height < 0) sourceRec.y -= sourceRec.height;

            rlEnableTexture(texture.id);

            rlPushMatrix();
                rlTranslatef(destRec.x, destRec.y, 0.0f);
    //            rlRotatef(rotation, 0.0f, 0.0f, 1.0f);
    //            rlTranslatef(-origin.x, -origin.y, 0.0f);

                rlBegin(RL_QUADS);
                    rlColor4ub(tint.r, tint.g, tint.b, tint.a);
                    rlNormal3f(0.0f, 0.0f, 1.0f);                          // Normal vector pointing towards viewer

                    // Bottom-left corner for texture and quad
                    if (flipX) rlTexCoord2f((sourceRec.x + sourceRec.width)/width, sourceRec.y/height);
                    else rlTexCoord2f(sourceRec.x/width, sourceRec.y/height);
                    rlVertex2f(0.0f, 0.0f);

                    // Bottom-right corner for texture and quad
                    if (flipX) rlTexCoord2f((sourceRec.x + sourceRec.width)/width, (sourceRec.y + sourceRec.height)/height);
                    else rlTexCoord2f(sourceRec.x/width, (sourceRec.y + sourceRec.height)/height);
                    rlVertex2f(0.0f, destRec.height);

                    // Top-right corner for texture and quad
                    if (flipX) rlTexCoord2f(sourceRec.x/width, (sourceRec.y + sourceRec.height)/height);
                    else rlTexCoord2f((sourceRec.x + sourceRec.width)/width, (sourceRec.y + sourceRec.height)/height);
                    rlVertex2f(destRec.width, destRec.height);

                    // Top-left corner for texture and quad
                    if (flipX) rlTexCoord2f(sourceRec.x/width, sourceRec.y/height);
                    else rlTexCoord2f((sourceRec.x + sourceRec.width)/width, sourceRec.y/height);
                    rlVertex2f(destRec.width, 0.0f);
                rlEnd();
            rlPopMatrix();

            rlDisableTexture();
        }
    }

    void screen::myDrawText(const char *text, Vector2 position, float fontSize, float spacing, Color tint) {
        DrawTextEx(displayFont, text, position, fontSize, spacing, tint);
/*
        int length = strlen(text);      // Total length in bytes of the text, scanned by codepoints in loop

        int textOffsetY = 0;            // Offset between lines (on line break '\n')
        float textOffsetX = 0.0f;       // Offset X to next character to draw
        
        float scaleFactor = fontSize/displayFont.baseSize;     // Character quad scaling factor

        for (int i = 0; i < length; i++)
        {
            // Get next codepoint from byte string and glyph index in font
            int codepointByteCount = 0;
            int codepoint = GetNextCodepoint(&text[i], &codepointByteCount);
            int index = myGetGlyphIndex(codepoint);

            // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
            // but we need to draw all of the bad bytes using the '?' symbol moving one byte
            if (codepoint == 0x3f) codepointByteCount = 1;

            if (codepoint == '\n')
            {
                // NOTE: Fixed line spacing of 1.5 line-height
                // TODO: Support custom line spacing defined by user
                textOffsetY += (int)((displayFont.baseSize + displayFont.baseSize/2)*scaleFactor);
                textOffsetX = 0.0f;
            }
            else
            {
                if ((codepoint != ' ') && (codepoint != '\t')) 
                {
                    Rectangle rec = { position.x + textOffsetX + displayFont.chars[index].offsetX*scaleFactor,
                                      position.y + textOffsetY + displayFont.chars[index].offsetY*scaleFactor, 
                                      displayFont.recs[index].width*scaleFactor, 
                                      displayFont.recs[index].height*scaleFactor };
        
                    myDrawTexture(displayFont.texture, displayFont.recs[index], rec, (Vector2){ 0, 0 }, 0.0f, tint);
                }

                if (displayFont.chars[index].advanceX == 0) textOffsetX += ((float)displayFont.recs[index].width*scaleFactor + spacing);
                else textOffsetX += ((float)displayFont.chars[index].advanceX*scaleFactor + spacing);
            }
            
            i += (codepointByteCount - 1);   // Move text bytes counter to next codepoint
        }*/
    }


    //Handle world lighting (e.g. for time of day...)

    float screen::dayLevel() {
        int time = tickCounter % dayLength;
        if (time > dayLength / 3 && time < dayLength * 2 / 3) {
            return 1;
        }
        else if (time > dayLength / 4 && time < dayLength * 3 / 4) {
            return (1 + cos(PI * tickCounter / (dayLength / 12.0))) / 2.0;
        }
        return 0;
    }

    float screen::sunsetLevel() {
        int time = tickCounter % dayLength;
        if (time > dayLength * 2 / 3 && time < dayLength * 5 / 6) {
            return (1 - cos(PI * tickCounter / (dayLength / 12.0))) / 2.0;
        }
        return 0;
    }

    float screen::nightLevel() {
        int time = tickCounter % dayLength;
        if (time > dayLength / 4 && time < dayLength * 3 / 4) {
            return 0;
        }
        else if (time > dayLength / 6 && time < dayLength * 5 / 6) {
            return (1 + cos(PI * tickCounter / (dayLength / 12.0))) / 2.0;
        }
        return 1;
    }

    float screen::dawnLevel() {
        int time = tickCounter % dayLength;
        if (time > dayLength / 6 && time < dayLength / 3) {
            return (1 - cos(PI * tickCounter / (dayLength / 12.0))) / 2.0;
        }
        return 0;
    }

    Color addColor(Color c1, Color c2) {
        return (Color){c1.r + c2.r, c1.g + c2.g, c1.b + c2.b, c1.a + c2.a};
    }

    Color scaleColor(Color c, float x) {
        return (Color){c.r * x, c.g * x, c.b * x, c.a * x};
    }

    void screen::calculateLighting() {
        light = addColor(addColor(addColor(scaleColor(dayLight, dayLevel()), scaleColor(nightLight, nightLevel())), scaleColor(sunsetLight, sunsetLevel())), scaleColor(dawnLight, dawnLevel()));
        background = addColor(addColor(addColor(scaleColor(dayBackground, dayLevel()), scaleColor(nightBackground, nightLevel())), scaleColor(sunsetBackground, sunsetLevel())), scaleColor(dawnBackground, dawnLevel()));
    }

    Color screen::lighting(Color c) {
        return (Color){c.r * light.r / 255.0, c.g * light.g / 255.0, c.b * light.b / 255.0, c.a * light.a / 255.0};
    }

    void screen::checkerboardTransition(int amountBlack) {
        int horizontal = 0x2581 + amountBlack;
        int vertical = 0x258f - amountBlack;
        int horizontalCheckInt[9], verticalCheckInt[9];
        for (int i = 0; i < 8; i++) {
            horizontalCheckInt[i] = horizontal;
            verticalCheckInt[i] = vertical;
        }
        horizontalCheckInt[8] = '\n';
        verticalCheckInt[8] = '\n';
        char* verticalCheck = TextToUtf8(verticalCheckInt, 9);
        char* horizontalCheck = TextToUtf8(horizontalCheckInt, 9);
        for (int i = 0; i < getHudRows() / 8 + 1; i++) {
            for (int j = 0; j < getHudCols() / 8 + 1; j++) {
                for (int k = 0; k < 8; k++) {
                    if ((j + i) % 2) {
                        drawHud(j * 8, i * 8 + k, BLACK, verticalCheck);
                    }
                    else {
                        drawHud(j * 8, i * 8 + k, BLACK, horizontalCheck);
                    }
                }
            }
        }
    }

    void screen::start(float playerX, float playerY, bool tabScreen) {

        //Update camera; round value to nearest pixel

        if (moveCameraX) {
            if (playerX > cameraX + cameraLagX) {
                cameraX = min(playerX - cameraLagX, worldCols - screenCols / 2.0f);
            }
            else if (playerX < cameraX - cameraLagX) {
                cameraX = max(playerX + cameraLagX, screenCols / 2.0f);
            }
        }
        if (moveCameraY) {
            if (playerY > cameraY + cameraLagY) {
                cameraY = min(playerY - cameraLagY, worldRows - screenRows / 2.0f);
            }
            else if (playerY < cameraY - cameraLagY) {
                cameraY = max(playerY + cameraLagY, screenRows / 2.0f);
            }
        }

        start(tabScreen);
    }

    void screen::start(bool tabScreen) {
        if (IsWindowResized()) {
            screenRows = GetScreenHeight() / fontSize;
            screenCols = GetScreenWidth() / fontSize;
            moveCameraX = (worldCols > screenCols / playerSizeFactor);
            moveCameraY = (worldRows > screenRows / playerSizeFactor);
            cameraLagX = screenCols / playerSizeFactor * 3 / 16;
            cameraLagY = screenRows / playerSizeFactor * 3 / 16;
       }

        BeginDrawing();
        if (tabScreen) {
            ClearBackground(UIBACKGROUND);
        }
        else {
            ClearBackground(background);
        }
    }

    void screen::end() {
        if (DRAWFPS) {
            myDrawText(to_string(GetFPS()).c_str(), (Vector2){0, 0}, 16, 0, WHITE);
        }
        EndDrawing();
    }

    void screen::draw(float x, float y, Color tint, float sizeFactor, string text, bool doLight) {
        if (doLight) {
            tint = lighting(tint);
        }
        myDrawText(text.c_str(),
            (Vector2){ (screenCols / sizeFactor / 2 - cameraX + x) * fontSize * sizeFactor,
            (screenRows / sizeFactor / 2 - cameraY + y) * fontSize * sizeFactor },
            fontSize * sizeFactor, 0, tint);
    }

    void screen::drawHud(float x, float y, Color tint, string text) {
        myDrawText(text.c_str(), (Vector2){x * hudFontSize, y * hudFontSize}, hudFontSize, 0, tint);
    }

    void screen::drawScaleTest(float x, float y, Color tint, string text) {
        myDrawText(text.c_str(), (Vector2){
            x * hudFontSize,
            y * hudFontSize + 0.5 * (hudFontSize - fontSize)},
            fontSize, 1, tint);
    }

    void screen::drawLayer(float x, float y, Color tint, float sizeFactor, Texture2D& t, bool doLight) {
        if (doLight) {
            tint = lighting(tint);
        }
        Rectangle sourceRec = {0.0f, 0.0f, (float)t.width, -1 * (float)t.height};
        Vector2 origin = { (screenCols / sizeFactor / 2 - cameraX + x) * fontSize * sizeFactor,
            (screenRows / sizeFactor / 2 - cameraY + y) * fontSize * sizeFactor };
        Rectangle destRec = {origin.x, origin.y, t.width * fontSize / 8 * sizeFactor, t.height * fontSize / 8 * sizeFactor};
        myDrawTexture(t, sourceRec, destRec, (Vector2){0.0f, 0.0f}, 0.0f, tint);
    }

    /******************************************************************************/
    //drawBar...()
    //Draws a bar of length n pixels starting at x, y (level coordinates) in the direction specified.
    //Due to character limitations, for down and left, rounds to four pixels.
    /******************************************************************************/

    void screen::drawBarLeft(float x, float y, Color tint, float sizeFactor, float length, bool doLight) {
        if (doLight) {
            tint = lighting(tint);
        }
        x = roundTo8th(x);
        y = roundTo8th(y);
        length = roundTo8th(length);

        //Using double fixes some alignment errors (esp. visible in water)

        double xPixel = (screenCols / sizeFactor / 2.0 - cameraX + x) * fontSize * sizeFactor;
        double yPixel = (screenRows / sizeFactor / 2.0 - cameraY + y) * fontSize * sizeFactor;
        double width = length * fontSize * sizeFactor + xPixel - floor(xPixel);
        double height = fontSize * sizeFactor + yPixel - floor(yPixel);
        //cout << xPixel << " + " << width << " = " << xPixel + width << endl;
        DrawRectangle(xPixel - width, yPixel, width, height, tint);
    }

    void screen::drawBarRight(float x, float y, Color tint, float sizeFactor, float length, bool doLight) {
        if (doLight) {
            tint = lighting(tint);
        }
        x = roundTo8th(x);
        y = roundTo8th(y);
        length = roundTo8th(length);
        double xPixel = (screenCols / sizeFactor / 2.0 - cameraX + x) * fontSize * sizeFactor;
        double yPixel = (screenRows / sizeFactor / 2.0 - cameraY + y) * fontSize * sizeFactor;
        double width = length * fontSize * sizeFactor + xPixel - floor(xPixel);
        double height = fontSize * sizeFactor + yPixel - floor(yPixel);
        //cout << xPixel << " + " << width << " = " << xPixel + width << endl;
        DrawRectangle(xPixel, yPixel, width, height, tint);
    }


    void screen::drawBarDown(float x, float y, Color tint, float sizeFactor, float length, bool doLight) {
        if (doLight) {
            tint = lighting(tint);
        }
        x = roundTo8th(x);
        y = roundTo8th(y);
        length = roundTo8th(length);
        double xPixel = (screenCols / sizeFactor / 2.0 - cameraX + x) * fontSize * sizeFactor;
        double yPixel = (screenRows / sizeFactor / 2.0 - cameraY + y) * fontSize * sizeFactor;
        double width = fontSize * sizeFactor + xPixel - floor(xPixel);
        double height = length * fontSize * sizeFactor + yPixel - floor(yPixel);
        //cout << xPixel << " + " << width << " = " << xPixel + width << endl;
        DrawRectangle(xPixel, yPixel, width, height, tint);
    }

    void screen::drawBarUp(float x, float y, Color tint, float sizeFactor, float length, bool doLight) {
        if (doLight) {
            tint = lighting(tint);
        }
        length = roundTo8th(length);
        double xPixel = (screenCols / sizeFactor / 2.0 - cameraX + x) * fontSize * sizeFactor;
        double yPixel = (screenRows / sizeFactor / 2.0 - cameraY + y) * fontSize * sizeFactor;
        double width = fontSize * sizeFactor + xPixel - floor(xPixel);
        double height = length * fontSize * sizeFactor + yPixel - floor(yPixel);
        //cout << xPixel << " + " << width << " = " << xPixel + width << endl;
        DrawRectangle(xPixel, yPixel - height, width, height, tint);
    }

/******************************************************************************/
//drawHudBar...()
//Draws a bar of length n pixels starting at x, y (screen coordinates) in the direction specified.
/******************************************************************************/

    void screen::drawHudBarLeft (float x, float y, Color tint, float length) {
        x = roundTo8th(x);
        y = roundTo8th(y);
        length = roundTo8th(length);
        DrawRectangle((x - length) * hudFontSize, y * hudFontSize, length * hudFontSize, hudFontSize, tint);
    }

    void screen::drawHudBarRight (float x, float y, Color tint, float length) {
        x = roundTo8th(x);
        y = roundTo8th(y);
        length = roundTo8th(length);
        DrawRectangle(x * hudFontSize, y * hudFontSize, length * hudFontSize, hudFontSize, tint);
    }

    void screen::drawHudBarDown (float x, float y, Color tint, float length) {
        x = roundTo8th(x);
        y = roundTo8th(y);
        length = roundTo8th(length);
        DrawRectangle(x * hudFontSize, y * hudFontSize, hudFontSize, length * hudFontSize, tint);
    }

    void screen::drawHudBarUp (float x, float y, Color tint, float length) {
        x = roundTo8th(x);
        y = roundTo8th(y);
        length = roundTo8th(length);
        DrawRectangle(x * hudFontSize, (y - length) * hudFontSize, hudFontSize, length * hudFontSize, tint);
    }

/******************************************************************************/
//Canvas access functions
/******************************************************************************/

    Vector2 screen::getCamera() {
        return (Vector2){cameraX, cameraY};
    }

    Vector2 screen::getMouseRelativeTo(float x, float y, float sizeFactor) {
        Vector2 mouse = GetMousePosition();
        return (Vector2){(mouse.x / fontSize - screenCols / 2.0) / sizeFactor + cameraX - x,
                         (mouse.y / fontSize - screenRows / 2.0) / sizeFactor + cameraY - y};
    }

    int screen::getScreenRows() {
        return screenRows;
    }

    int screen::getScreenCols() {
        return screenCols;
    }

    int screen::getHudRows() {
        return GetScreenHeight() / hudFontSize;
    }

    int screen::getHudCols() {
        return GetScreenWidth() / hudFontSize;
    }

    int screen::getFontSize() {
        return fontSize;
    }

    int screen::getHudFontSize() {
        return hudFontSize;
    }

    void screen::tweakHudScale() {
        hudFontSize += 4;
        if (hudFontSize * MINWINDOWCOLS > GetMonitorWidth(0) || hudFontSize * MINWINDOWROWS > GetMonitorHeight(0)) {
            hudFontSize = 8;
        }
        SetWindowMinSize(hudFontSize * MINWINDOWCOLS, hudFontSize * MINWINDOWROWS);
    }

    void screen::tweakGameScale() {
        fontSize += 4;
        if (fontSize * MINWINDOWCOLS > GetMonitorWidth(0) || fontSize * MINWINDOWROWS > GetMonitorHeight(0)) {
            fontSize = 8;
        }
        screenRows = GetScreenHeight() / fontSize;
        screenCols = GetScreenWidth() / fontSize;
        moveCameraX = (worldCols > screenCols / playerSizeFactor);
        moveCameraY = (worldRows > screenRows / playerSizeFactor);
        cameraLagX = screenCols / playerSizeFactor * 3 / 16;
        cameraLagY = screenRows / playerSizeFactor * 3 / 16;
    }

/******************************************************************************/
//editableCanvas
/******************************************************************************/

    editableCanvas::editableCanvas() : screen() {}

    void editableCanvas::changeLighting() {
        lightingSelection++;
        switch (lightingSelection % 4) {
            case 0: {
                light = dayLight;
                background = dayBackground;
                break;
            }
            case 1: {
                light = sunsetLight;
                background = sunsetBackground;
                break;
            }
            case 2: {
                light = nightLight;
                background = nightBackground;
                break;
            }
            case 3: {
                light = dawnLight;
                background = dawnBackground;
                break;
            }
        }
    }

    void editableCanvas::moveCamera() {

        //Camera movement

        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            cameraSpeed = 8;
        }
        else {
            cameraSpeed = 1;
        }
        if (IsKeyDown(KEY_D)) {
            cameraX += cameraSpeed;
        }
        if (IsKeyDown(KEY_A)) {
            cameraX -= cameraSpeed;
        }
        if (IsKeyDown(KEY_W)) {
            cameraY -= cameraSpeed;
        }
        if (IsKeyDown(KEY_S)) {
            cameraY += cameraSpeed;
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
            cameraX = oldCameraX - (mouse.x - oldMouseX) / theScreen -> getFontSize();
            cameraY = oldCameraY - (mouse.y - oldMouseY) / theScreen -> getFontSize();
        }

        //Zoom

        fontSize += GetMouseWheelMove();

    }

    void editableCanvas::drawLayerEditor(float x, float y, Color tint, float sizeFactor, Texture2D& t, bool selected, bool doLight) {
        if (doLight) {
            tint = lighting(tint);
        }
        Rectangle sourceRec = {0.0f, 0.0f, (float)t.width, -1 * (float)t.height};
        Vector2 origin = { (screenCols / sizeFactor / 2 - cameraX + x) * fontSize * sizeFactor,
            (screenRows / sizeFactor / 2 - cameraY + y) * fontSize * sizeFactor };
        Rectangle destRec = {origin.x, origin.y, t.width * fontSize / 8 * sizeFactor, t.height * fontSize / 8 * sizeFactor};
        myDrawTexture(t, sourceRec, destRec, (Vector2){0.0f, 0.0f}, 0.0f, tint);
        if (selected) {
            DrawRectangleLines(destRec.x, destRec.y, destRec.width, destRec.height, RED);
        }
    }

//I don't know how the compiler is actually doing this, but
//this seems like the fastest implementation, ugly as it is.

    int screen::myGetGlyphIndex(int codepoint) {
        switch(codepoint) {
            case 0: return 0;
            case 1: return 1;
            case 2: return 2;
            case 3: return 3;
            case 4: return 4;
            case 5: return 5;
            case 6: return 6;
            case 7: return 7;
            case 8: return 8;
            case 9: return 9;
            case 10: return 10;
            case 11: return 11;
            case 12: return 12;
            case 13: return 13;
            case 14: return 14;
            case 15: return 15;
            case 16: return 16;
            case 17: return 17;
            case 18: return 18;
            case 19: return 19;
            case 20: return 20;
            case 21: return 21;
            case 22: return 22;
            case 23: return 23;
            case 24: return 24;
            case 25: return 25;
            case 26: return 26;
            case 27: return 27;
            case 28: return 28;
            case 29: return 29;
            case 30: return 30;
            case 31: return 31;
            case 32: return 32;
            case 33: return 33;
            case 34: return 34;
            case 35: return 35;
            case 36: return 36;
            case 37: return 37;
            case 38: return 38;
            case 39: return 39;
            case 40: return 40;
            case 41: return 41;
            case 42: return 42;
            case 43: return 43;
            case 44: return 44;
            case 45: return 45;
            case 46: return 46;
            case 47: return 47;
            case 48: return 48;
            case 49: return 49;
            case 50: return 50;
            case 51: return 51;
            case 52: return 52;
            case 53: return 53;
            case 54: return 54;
            case 55: return 55;
            case 56: return 56;
            case 57: return 57;
            case 58: return 58;
            case 59: return 59;
            case 60: return 60;
            case 61: return 61;
            case 62: return 62;
            case 63: return 63;
            case 64: return 64;
            case 65: return 65;
            case 66: return 66;
            case 67: return 67;
            case 68: return 68;
            case 69: return 69;
            case 70: return 70;
            case 71: return 71;
            case 72: return 72;
            case 73: return 73;
            case 74: return 74;
            case 75: return 75;
            case 76: return 76;
            case 77: return 77;
            case 78: return 78;
            case 79: return 79;
            case 80: return 80;
            case 81: return 81;
            case 82: return 82;
            case 83: return 83;
            case 84: return 84;
            case 85: return 85;
            case 86: return 86;
            case 87: return 87;
            case 88: return 88;
            case 89: return 89;
            case 90: return 90;
            case 91: return 91;
            case 92: return 92;
            case 93: return 93;
            case 94: return 94;
            case 95: return 95;
            case 96: return 96;
            case 97: return 97;
            case 98: return 98;
            case 99: return 99;
            case 100: return 100;
            case 101: return 101;
            case 102: return 102;
            case 103: return 103;
            case 104: return 104;
            case 105: return 105;
            case 106: return 106;
            case 107: return 107;
            case 108: return 108;
            case 109: return 109;
            case 110: return 110;
            case 111: return 111;
            case 112: return 112;
            case 113: return 113;
            case 114: return 114;
            case 115: return 115;
            case 116: return 116;
            case 117: return 117;
            case 118: return 118;
            case 119: return 119;
            case 120: return 120;
            case 121: return 121;
            case 122: return 122;
            case 123: return 123;
            case 124: return 124;
            case 125: return 125;
            case 126: return 126;
            case 127: return 127;
            case 160: return 128;
            case 161: return 129;
            case 162: return 130;
            case 163: return 131;
            case 164: return 132;
            case 165: return 133;
            case 166: return 134;
            case 167: return 135;
            case 168: return 136;
            case 169: return 137;
            case 170: return 138;
            case 171: return 139;
            case 172: return 140;
            case 173: return 141;
            case 174: return 142;
            case 175: return 143;
            case 176: return 144;
            case 177: return 145;
            case 178: return 146;
            case 179: return 147;
            case 180: return 148;
            case 181: return 149;
            case 182: return 150;
            case 183: return 151;
            case 184: return 152;
            case 185: return 153;
            case 186: return 154;
            case 187: return 155;
            case 188: return 156;
            case 189: return 157;
            case 190: return 158;
            case 191: return 159;
            case 192: return 160;
            case 193: return 161;
            case 194: return 162;
            case 195: return 163;
            case 196: return 164;
            case 197: return 165;
            case 198: return 166;
            case 199: return 167;
            case 200: return 168;
            case 201: return 169;
            case 202: return 170;
            case 203: return 171;
            case 204: return 172;
            case 205: return 173;
            case 206: return 174;
            case 207: return 175;
            case 208: return 176;
            case 209: return 177;
            case 210: return 178;
            case 211: return 179;
            case 212: return 180;
            case 213: return 181;
            case 214: return 182;
            case 215: return 183;
            case 216: return 184;
            case 217: return 185;
            case 218: return 186;
            case 219: return 187;
            case 220: return 188;
            case 221: return 189;
            case 222: return 190;
            case 223: return 191;
            case 224: return 192;
            case 225: return 193;
            case 226: return 194;
            case 227: return 195;
            case 228: return 196;
            case 229: return 197;
            case 230: return 198;
            case 231: return 199;
            case 232: return 200;
            case 233: return 201;
            case 234: return 202;
            case 235: return 203;
            case 236: return 204;
            case 237: return 205;
            case 238: return 206;
            case 239: return 207;
            case 240: return 208;
            case 241: return 209;
            case 242: return 210;
            case 243: return 211;
            case 244: return 212;
            case 245: return 213;
            case 246: return 214;
            case 247: return 215;
            case 248: return 216;
            case 249: return 217;
            case 250: return 218;
            case 251: return 219;
            case 252: return 220;
            case 253: return 221;
            case 254: return 222;
            case 255: return 223;
            case 256: return 224;
            case 257: return 225;
            case 258: return 226;
            case 259: return 227;
            case 260: return 228;
            case 261: return 229;
            case 262: return 230;
            case 263: return 231;
            case 264: return 232;
            case 265: return 233;
            case 266: return 234;
            case 267: return 235;
            case 268: return 236;
            case 269: return 237;
            case 270: return 238;
            case 271: return 239;
            case 272: return 240;
            case 273: return 241;
            case 274: return 242;
            case 275: return 243;
            case 276: return 244;
            case 277: return 245;
            case 278: return 246;
            case 279: return 247;
            case 280: return 248;
            case 281: return 249;
            case 282: return 250;
            case 283: return 251;
            case 284: return 252;
            case 285: return 253;
            case 286: return 254;
            case 287: return 255;
            case 288: return 256;
            case 289: return 257;
            case 290: return 258;
            case 291: return 259;
            case 292: return 260;
            case 293: return 261;
            case 294: return 262;
            case 295: return 263;
            case 296: return 264;
            case 297: return 265;
            case 298: return 266;
            case 299: return 267;
            case 300: return 268;
            case 301: return 269;
            case 302: return 270;
            case 303: return 271;
            case 304: return 272;
            case 305: return 273;
            case 306: return 274;
            case 307: return 275;
            case 308: return 276;
            case 309: return 277;
            case 310: return 278;
            case 311: return 279;
            case 312: return 280;
            case 313: return 281;
            case 314: return 282;
            case 315: return 283;
            case 316: return 284;
            case 317: return 285;
            case 318: return 286;
            case 319: return 287;
            case 320: return 288;
            case 321: return 289;
            case 322: return 290;
            case 323: return 291;
            case 324: return 292;
            case 325: return 293;
            case 326: return 294;
            case 327: return 295;
            case 328: return 296;
            case 329: return 297;
            case 330: return 298;
            case 331: return 299;
            case 332: return 300;
            case 333: return 301;
            case 334: return 302;
            case 335: return 303;
            case 336: return 304;
            case 337: return 305;
            case 338: return 306;
            case 339: return 307;
            case 340: return 308;
            case 341: return 309;
            case 342: return 310;
            case 343: return 311;
            case 344: return 312;
            case 345: return 313;
            case 346: return 314;
            case 347: return 315;
            case 348: return 316;
            case 349: return 317;
            case 350: return 318;
            case 351: return 319;
            case 352: return 320;
            case 353: return 321;
            case 354: return 322;
            case 355: return 323;
            case 356: return 324;
            case 357: return 325;
            case 358: return 326;
            case 359: return 327;
            case 360: return 328;
            case 361: return 329;
            case 362: return 330;
            case 363: return 331;
            case 364: return 332;
            case 365: return 333;
            case 366: return 334;
            case 367: return 335;
            case 368: return 336;
            case 369: return 337;
            case 370: return 338;
            case 371: return 339;
            case 372: return 340;
            case 373: return 341;
            case 374: return 342;
            case 375: return 343;
            case 376: return 344;
            case 377: return 345;
            case 378: return 346;
            case 379: return 347;
            case 380: return 348;
            case 381: return 349;
            case 382: return 350;
            case 383: return 351;
            case 402: return 352;
            case 417: return 353;
            case 439: return 354;
            case 506: return 355;
            case 507: return 356;
            case 508: return 357;
            case 509: return 358;
            case 510: return 359;
            case 511: return 360;
            case 536: return 361;
            case 537: return 362;
            case 538: return 363;
            case 539: return 364;
            case 593: return 365;
            case 632: return 366;
            case 710: return 367;
            case 711: return 368;
            case 713: return 369;
            case 728: return 370;
            case 729: return 371;
            case 730: return 372;
            case 731: return 373;
            case 732: return 374;
            case 733: return 375;
            case 894: return 376;
            case 900: return 377;
            case 901: return 378;
            case 902: return 379;
            case 903: return 380;
            case 904: return 381;
            case 905: return 382;
            case 906: return 383;
            case 908: return 384;
            case 910: return 385;
            case 911: return 386;
            case 912: return 387;
            case 913: return 388;
            case 914: return 389;
            case 915: return 390;
            case 916: return 391;
            case 917: return 392;
            case 918: return 393;
            case 919: return 394;
            case 920: return 395;
            case 921: return 396;
            case 922: return 397;
            case 923: return 398;
            case 924: return 399;
            case 925: return 400;
            case 926: return 401;
            case 927: return 402;
            case 928: return 403;
            case 929: return 404;
            case 931: return 405;
            case 932: return 406;
            case 933: return 407;
            case 934: return 408;
            case 935: return 409;
            case 936: return 410;
            case 937: return 411;
            case 938: return 412;
            case 939: return 413;
            case 940: return 414;
            case 941: return 415;
            case 942: return 416;
            case 943: return 417;
            case 944: return 418;
            case 945: return 419;
            case 946: return 420;
            case 947: return 421;
            case 948: return 422;
            case 949: return 423;
            case 950: return 424;
            case 951: return 425;
            case 952: return 426;
            case 953: return 427;
            case 954: return 428;
            case 955: return 429;
            case 956: return 430;
            case 957: return 431;
            case 958: return 432;
            case 959: return 433;
            case 960: return 434;
            case 961: return 435;
            case 962: return 436;
            case 963: return 437;
            case 964: return 438;
            case 965: return 439;
            case 966: return 440;
            case 967: return 441;
            case 968: return 442;
            case 969: return 443;
            case 970: return 444;
            case 971: return 445;
            case 972: return 446;
            case 973: return 447;
            case 974: return 448;
            case 976: return 449;
            case 1012: return 450;
            case 1024: return 451;
            case 1025: return 452;
            case 1026: return 453;
            case 1027: return 454;
            case 1028: return 455;
            case 1029: return 456;
            case 1030: return 457;
            case 1031: return 458;
            case 1032: return 459;
            case 1033: return 460;
            case 1034: return 461;
            case 1035: return 462;
            case 1036: return 463;
            case 1037: return 464;
            case 1038: return 465;
            case 1039: return 466;
            case 1040: return 467;
            case 1041: return 468;
            case 1042: return 469;
            case 1043: return 470;
            case 1044: return 471;
            case 1045: return 472;
            case 1046: return 473;
            case 1047: return 474;
            case 1048: return 475;
            case 1049: return 476;
            case 1050: return 477;
            case 1051: return 478;
            case 1052: return 479;
            case 1053: return 480;
            case 1054: return 481;
            case 1055: return 482;
            case 1056: return 483;
            case 1057: return 484;
            case 1058: return 485;
            case 1059: return 486;
            case 1060: return 487;
            case 1061: return 488;
            case 1062: return 489;
            case 1063: return 490;
            case 1064: return 491;
            case 1065: return 492;
            case 1066: return 493;
            case 1067: return 494;
            case 1068: return 495;
            case 1069: return 496;
            case 1070: return 497;
            case 1071: return 498;
            case 1072: return 499;
            case 1073: return 500;
            case 1074: return 501;
            case 1075: return 502;
            case 1076: return 503;
            case 1077: return 504;
            case 1078: return 505;
            case 1079: return 506;
            case 1080: return 507;
            case 1081: return 508;
            case 1082: return 509;
            case 1083: return 510;
            case 1084: return 511;
            case 1085: return 512;
            case 1086: return 513;
            case 1087: return 514;
            case 1088: return 515;
            case 1089: return 516;
            case 1090: return 517;
            case 1091: return 518;
            case 1092: return 519;
            case 1093: return 520;
            case 1094: return 521;
            case 1095: return 522;
            case 1096: return 523;
            case 1097: return 524;
            case 1098: return 525;
            case 1099: return 526;
            case 1100: return 527;
            case 1101: return 528;
            case 1102: return 529;
            case 1103: return 530;
            case 1104: return 531;
            case 1105: return 532;
            case 1106: return 533;
            case 1107: return 534;
            case 1108: return 535;
            case 1109: return 536;
            case 1110: return 537;
            case 1111: return 538;
            case 1112: return 539;
            case 1113: return 540;
            case 1114: return 541;
            case 1115: return 542;
            case 1116: return 543;
            case 1117: return 544;
            case 1118: return 545;
            case 1119: return 546;
            case 1168: return 547;
            case 1169: return 548;
            case 7451: return 549;
            case 7462: return 550;
            case 7464: return 551;
            case 7808: return 552;
            case 7809: return 553;
            case 7810: return 554;
            case 7811: return 555;
            case 7812: return 556;
            case 7813: return 557;
            case 7839: return 558;
            case 7922: return 559;
            case 7923: return 560;
            case 8208: return 561;
            case 8210: return 562;
            case 8211: return 563;
            case 8212: return 564;
            case 8213: return 565;
            case 8215: return 566;
            case 8216: return 567;
            case 8217: return 568;
            case 8218: return 569;
            case 8219: return 570;
            case 8220: return 571;
            case 8221: return 572;
            case 8222: return 573;
            case 8223: return 574;
            case 8224: return 575;
            case 8225: return 576;
            case 8226: return 577;
            case 8230: return 578;
            case 8231: return 579;
            case 8240: return 580;
            case 8242: return 581;
            case 8243: return 582;
            case 8245: return 583;
            case 8249: return 584;
            case 8250: return 585;
            case 8252: return 586;
            case 8254: return 587;
            case 8255: return 588;
            case 8256: return 589;
            case 8260: return 590;
            case 8276: return 591;
            case 8308: return 592;
            case 8309: return 593;
            case 8310: return 594;
            case 8311: return 595;
            case 8312: return 596;
            case 8313: return 597;
            case 8314: return 598;
            case 8315: return 599;
            case 8319: return 600;
            case 8321: return 601;
            case 8322: return 602;
            case 8323: return 603;
            case 8324: return 604;
            case 8325: return 605;
            case 8326: return 606;
            case 8327: return 607;
            case 8328: return 608;
            case 8329: return 609;
            case 8330: return 610;
            case 8331: return 611;
            case 8355: return 612;
            case 8356: return 613;
            case 8359: return 614;
            case 8362: return 615;
            case 8364: return 616;
            case 8453: return 617;
            case 8467: return 618;
            case 8470: return 619;
            case 8482: return 620;
            case 8486: return 621;
            case 8494: return 622;
            case 8528: return 623;
            case 8529: return 624;
            case 8531: return 625;
            case 8532: return 626;
            case 8533: return 627;
            case 8534: return 628;
            case 8535: return 629;
            case 8536: return 630;
            case 8537: return 631;
            case 8538: return 632;
            case 8539: return 633;
            case 8540: return 634;
            case 8541: return 635;
            case 8542: return 636;
            case 8592: return 637;
            case 8593: return 638;
            case 8594: return 639;
            case 8595: return 640;
            case 8596: return 641;
            case 8597: return 642;
            case 8616: return 643;
            case 8706: return 644;
            case 8709: return 645;
            case 8710: return 646;
            case 8712: return 647;
            case 8719: return 648;
            case 8721: return 649;
            case 8722: return 650;
            case 8725: return 651;
            case 8729: return 652;
            case 8730: return 653;
            case 8734: return 654;
            case 8735: return 655;
            case 8745: return 656;
            case 8747: return 657;
            case 8776: return 658;
            case 8800: return 659;
            case 8801: return 660;
            case 8804: return 661;
            case 8805: return 662;
            case 8857: return 663;
            case 8960: return 664;
            case 8962: return 665;
            case 8976: return 666;
            case 8992: return 667;
            case 8993: return 668;
            case 9121: return 669;
            case 9122: return 670;
            case 9123: return 671;
            case 9124: return 672;
            case 9125: return 673;
            case 9126: return 674;
            case 9472: return 675;
            case 9473: return 676;
            case 9474: return 677;
            case 9475: return 678;
            case 9484: return 679;
            case 9485: return 680;
            case 9486: return 681;
            case 9487: return 682;
            case 9488: return 683;
            case 9489: return 684;
            case 9490: return 685;
            case 9491: return 686;
            case 9492: return 687;
            case 9493: return 688;
            case 9494: return 689;
            case 9495: return 690;
            case 9496: return 691;
            case 9497: return 692;
            case 9498: return 693;
            case 9499: return 694;
            case 9500: return 695;
            case 9501: return 696;
            case 9502: return 697;
            case 9503: return 698;
            case 9504: return 699;
            case 9505: return 700;
            case 9506: return 701;
            case 9507: return 702;
            case 9508: return 703;
            case 9509: return 704;
            case 9510: return 705;
            case 9511: return 706;
            case 9512: return 707;
            case 9513: return 708;
            case 9514: return 709;
            case 9515: return 710;
            case 9516: return 711;
            case 9517: return 712;
            case 9518: return 713;
            case 9519: return 714;
            case 9520: return 715;
            case 9521: return 716;
            case 9522: return 717;
            case 9523: return 718;
            case 9524: return 719;
            case 9525: return 720;
            case 9526: return 721;
            case 9527: return 722;
            case 9528: return 723;
            case 9529: return 724;
            case 9530: return 725;
            case 9531: return 726;
            case 9532: return 727;
            case 9533: return 728;
            case 9534: return 729;
            case 9535: return 730;
            case 9536: return 731;
            case 9537: return 732;
            case 9538: return 733;
            case 9539: return 734;
            case 9540: return 735;
            case 9541: return 736;
            case 9542: return 737;
            case 9543: return 738;
            case 9544: return 739;
            case 9545: return 740;
            case 9546: return 741;
            case 9547: return 742;
            case 9552: return 743;
            case 9553: return 744;
            case 9554: return 745;
            case 9555: return 746;
            case 9556: return 747;
            case 9557: return 748;
            case 9558: return 749;
            case 9559: return 750;
            case 9560: return 751;
            case 9561: return 752;
            case 9562: return 753;
            case 9563: return 754;
            case 9564: return 755;
            case 9565: return 756;
            case 9566: return 757;
            case 9567: return 758;
            case 9568: return 759;
            case 9569: return 760;
            case 9570: return 761;
            case 9571: return 762;
            case 9572: return 763;
            case 9573: return 764;
            case 9574: return 765;
            case 9575: return 766;
            case 9576: return 767;
            case 9577: return 768;
            case 9578: return 769;
            case 9579: return 770;
            case 9580: return 771;
            case 9581: return 772;
            case 9582: return 773;
            case 9583: return 774;
            case 9584: return 775;
            case 9585: return 776;
            case 9586: return 777;
            case 9587: return 778;
            case 9588: return 779;
            case 9589: return 780;
            case 9590: return 781;
            case 9591: return 782;
            case 9592: return 783;
            case 9593: return 784;
            case 9594: return 785;
            case 9595: return 786;
            case 9596: return 787;
            case 9597: return 788;
            case 9598: return 789;
            case 9599: return 790;
            case 9600: return 791;
            case 9601: return 792;
            case 9602: return 793;
            case 9603: return 794;
            case 9604: return 795;
            case 9605: return 796;
            case 9606: return 797;
            case 9607: return 798;
            case 9608: return 799;
            case 9609: return 800;
            case 9610: return 801;
            case 9611: return 802;
            case 9612: return 803;
            case 9613: return 804;
            case 9614: return 805;
            case 9615: return 806;
            case 9616: return 807;
            case 9617: return 808;
            case 9618: return 809;
            case 9619: return 810;
            case 9620: return 811;
            case 9621: return 812;
            case 9622: return 813;
            case 9623: return 814;
            case 9624: return 815;
            case 9625: return 816;
            case 9626: return 817;
            case 9627: return 818;
            case 9628: return 819;
            case 9629: return 820;
            case 9630: return 821;
            case 9631: return 822;
            case 9632: return 823;
            case 9633: return 824;
            case 9634: return 825;
            case 9635: return 826;
            case 9636: return 827;
            case 9637: return 828;
            case 9638: return 829;
            case 9639: return 830;
            case 9640: return 831;
            case 9641: return 832;
            case 9642: return 833;
            case 9643: return 834;
            case 9644: return 835;
            case 9645: return 836;
            case 9646: return 837;
            case 9647: return 838;
            case 9648: return 839;
            case 9649: return 840;
            case 9650: return 841;
            case 9651: return 842;
            case 9652: return 843;
            case 9653: return 844;
            case 9658: return 845;
            case 9660: return 846;
            case 9668: return 847;
            case 9670: return 848;
            case 9674: return 849;
            case 9675: return 850;
            case 9679: return 851;
            case 9688: return 852;
            case 9689: return 853;
            case 9698: return 854;
            case 9699: return 855;
            case 9700: return 856;
            case 9701: return 857;
            case 9702: return 858;
            case 9744: return 859;
            case 9786: return 860;
            case 9787: return 861;
            case 9788: return 862;
            case 9792: return 863;
            case 9794: return 864;
            case 9824: return 865;
            case 9827: return 866;
            case 9829: return 867;
            case 9830: return 868;
            case 9834: return 869;
            case 9835: return 870;
            case 10003: return 871;
            case 64257: return 872;
            case 64258: return 873;
            case 65533: return 874;
            default: return 63;
        }
    }
