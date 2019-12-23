void MyDrawTextEx(Font font, const char* text, Vector2 position, float fontSize, float spacing, Color tint)
{
    int length = strlen(text);      // Total length in bytes of the text, scanned by codepoints in loop

    int textOffsetY = 0;            // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f;       // Offset X to next character to draw

    float scaleFactor = fontSize/font.baseSize;     // Character quad scaling factor

    for (int i = 0; i < length; i++) {
        int ind = text[i];//GetGlyphIndex(font, text[i]);
        Rectangle rec = { position.x + textOffsetX + font.chars[ind].offsetX*scaleFactor,
                          position.y + textOffsetY + font.chars[ind].offsetY*scaleFactor, 
                          font.recs[ind].width*scaleFactor, 
                          font.recs[ind].height*scaleFactor };

        DrawTexturePro(font.texture, font.recs[ind], rec, (Vector2){ 0, 0 }, 0.0f, tint);
        if (font.chars[ind].advanceX == 0) {
            textOffsetX += ((float)font.recs[ind].width*scaleFactor + spacing);
        }
        else {
            textOffsetX += ((float)font.chars[ind].advanceX*scaleFactor + spacing);
        }
    }
}

