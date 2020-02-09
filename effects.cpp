#include "effects.hpp"


/******************************************************************************/
//Explosion()
//Spawn particles moving outwards in a circle
/******************************************************************************/

    void explosion( collider& col, entityList* entities, int count, float x, float y, Color tint,
                    float newSizeFactor, float speed, int c, int lifespan, float elasticity) {
        for (float angle = 0; angle < 2 * M_PI; angle += (2 * M_PI / count)) {
            physicalParticle* p = new physicalParticle(x, y, tint, newSizeFactor, c, elasticity, cos(angle) * speed, sin(angle) * speed, 3, GRAVITY, FRICTION, lifespan);
            entities -> addEntity(p);
            col.addParticle(p);
        }
    }

    void splash( collider& col, entityList* entities, int count, float x, float y, Color tint,
                    float newSizeFactor, float speed, int c, int lifespan, float elasticity) {
        for (float angle = 0; angle < 2 * M_PI; angle += (2 * M_PI / count)) {
            drop* p = new drop(x, y, tint, newSizeFactor, c, elasticity, cos(angle) * speed, sin(angle) * speed, 3, GRAVITY, FRICTION, lifespan);
            entities -> addEntity(p);
            col.addParticle(p);
        }
    }


/******************************************************************************/
//DamageIndicator()
//Spawns a little number that floats up when damage is taken/dealt
/******************************************************************************/

void damageIndicator(entityList* entities, int damage, float x, float y, Color tint, float newSizeFactor) {
    float leftEnd = x - 1 - floor(log10(abs(damage))) / 2;
    string digits = to_string(damage);
    for (int i = 0; i < digits.length(); i++) {
        entities -> addEntity (new particle (leftEnd + i, y, tint, newSizeFactor, 0, -0.1, digits[i], 60));
    }
}

/******************************************************************************/
//drawBar...()
//Draws a bar of length n pixels starting at x, y (level coordinates) in the direction specified.
//Due to character limitations, for down and left, rounds to four pixels.
/******************************************************************************/

void drawBarLeft(float cameraX, float cameraY, Font displayFont, float x, float y, Color tint, float sizeFactor, int length) {
    int partBlock = ' ', fullBlock = 0x2588;
    if ((length + 2) / 4 % 2 == 1) {
        partBlock = 0x2590;
    }
    for (int i = 0; i < (length + 2) / 8; i++) {
        myDrawText(displayFont, TextToUtf8(&fullBlock, 1), {
        (SCREENCOLS / sizeFactor / 2 - cameraX + x - i) * FONTSIZE * sizeFactor,
        (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE, 0, tint);
    }
    myDrawText(displayFont, TextToUtf8(&partBlock, 1), {
    (SCREENCOLS / sizeFactor / 2 - cameraX + x - (length + 2) / 8) * FONTSIZE * sizeFactor,
    (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE, 0, tint);
}

void drawBarRight(float cameraX, float cameraY, Font displayFont, float x, float y, Color tint, float sizeFactor, int length) {
    int partBlock = ' ', fullBlock = 0x2588;
    if (length % 8 != 0) {
        partBlock = 0x2590 - length % 8;
    }
    for (int i = 0; i < length / 8; i++) {
        myDrawText(displayFont, TextToUtf8(&fullBlock, 1), {
        (SCREENCOLS / sizeFactor / 2 - cameraX + x + i) * FONTSIZE * sizeFactor,
        (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE, 0, tint);
    }
    myDrawText(displayFont, TextToUtf8(&partBlock, 1), {
    (SCREENCOLS / sizeFactor / 2 - cameraX + x + length / 8) * FONTSIZE * sizeFactor,
    (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE, 0, tint);
}


void drawBarDown(float cameraX, float cameraY, Font displayFont, float x, float y, Color tint, float sizeFactor, int length) {
    int partBlock = ' ', fullBlock = 0x2588;
    if ((length + 2) / 4 % 2 == 1) {
        partBlock = 0x2580;
    }
    for (int i = 0; i < (length + 2) / 8; i++) {
        myDrawText(displayFont, TextToUtf8(&fullBlock, 1), {
        (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor,
        (SCREENROWS / sizeFactor / 2 - cameraY + y + i) * FONTSIZE * sizeFactor }, FONTSIZE, 0, tint);
    }
    myDrawText(displayFont, TextToUtf8(&partBlock, 1), {
    (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor,
    (SCREENROWS / sizeFactor / 2 - cameraY + y + (length + 2) / 8) * FONTSIZE * sizeFactor }, FONTSIZE, 0, tint);
}

void drawBarUp(float cameraX, float cameraY, Font displayFont, float x, float y, Color tint, float sizeFactor, int length) {
    int partBlock = ' ', fullBlock = 0x2588;
    if (length % 8 != 0) {
        partBlock = 0x2580 + length % 8;
    }
    for (int i = 0; i < length / 8; i++) {
        myDrawText(displayFont, TextToUtf8(&fullBlock, 1), {
        (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor,
        (SCREENROWS / sizeFactor / 2 - cameraY + y - i) * FONTSIZE * sizeFactor }, FONTSIZE, 0, tint);
    }
    myDrawText(displayFont, TextToUtf8(&partBlock, 1), {
    (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor,
    (SCREENROWS / sizeFactor / 2 - cameraY + y - length / 8) * FONTSIZE * sizeFactor }, FONTSIZE, 0, tint);
}

/******************************************************************************/
//drawHudBar...()
//Draws a bar of length n pixels starting at x, y (screen coordinates) in the direction specified.
//Due to character limitations, for down and left, rounds to four pixels.
/******************************************************************************/

void drawHudBarLeft (Font displayFont, int x, int y, Color tint, int length) {
    int partBlock = ' ', fullBlock = 0x2588;
    if ((length + 2) / 4 % 2 == 1) {
        partBlock = 0x2590;
    }
    for (int i = 0; i < (length + 2) / 8; i++) {
        myDrawText(displayFont, TextToUtf8(&fullBlock, 1), { x - i * FONTSIZE, y }, FONTSIZE, 0, tint);
    }
    myDrawText(displayFont, TextToUtf8(&partBlock, 1), { x - (length + 2) / 8 * FONTSIZE, y}, FONTSIZE, 0, tint);
}

void drawHudBarRight (Font displayFont, int x, int y, Color tint, int length) {
    int partBlock = ' ', fullBlock = 0x2588;
    if (length % 8 != 0) {
        partBlock = 0x2590 - length % 8;
    }
    for (int i = 0; i < length / 8; i++) {
        myDrawText(displayFont, TextToUtf8(&fullBlock, 1), { x + i * FONTSIZE, y}, FONTSIZE, 0, tint);
    }
    myDrawText(displayFont, TextToUtf8(&partBlock, 1), { x + length / 8 * FONTSIZE, y}, FONTSIZE, 0, tint);
}


void drawHudBarDown (Font displayFont, int x, int y, Color tint, int length) {
    int partBlock = ' ', fullBlock = 0x2588;
    if ((length + 2) / 4 % 2 == 1) {
        partBlock = 0x2580;
    }
    for (int i = 0; i < (length + 2) / 8; i++) {
        myDrawText(displayFont, TextToUtf8(&fullBlock, 1), {x, y + i * FONTSIZE }, FONTSIZE, 0, tint);
    }
    myDrawText(displayFont, TextToUtf8(&partBlock, 1), {x, y + (length + 2) / 8 * FONTSIZE }, FONTSIZE, 0, tint);
}

void drawHudBarUp (Font displayFont, int x, int y, Color tint, int length) {
    int partBlock = ' ', fullBlock = 0x2588;
    if (length % 8 != 0) {
        partBlock = 0x2580 + length % 8;
    }
    for (int i = 0; i < length / 8; i++) {
        myDrawText(displayFont, TextToUtf8(&fullBlock, 1), { x, y - i * FONTSIZE }, FONTSIZE, 0, tint);
    }
    myDrawText(displayFont, TextToUtf8(&partBlock, 1), { x, y - length / 8 * FONTSIZE }, FONTSIZE, 0, tint);
}

