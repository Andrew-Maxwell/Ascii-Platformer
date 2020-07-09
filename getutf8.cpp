#include <iostream>
#include <cstring>
#include <stdio.h>

using namespace std;

// Encode codepoint into utf8 text (char array length returned as parameter)
const char *CodepointToUtf8(int codepoint, int *byteLength)
{
    static char utf8[6] = { 0 };
    int length = 0;

    if (codepoint <= 0x7f)
    {
        utf8[0] = (char)codepoint;
        length = 1;
    }
    else if (codepoint <= 0x7ff)
    {
        utf8[0] = (char)(((codepoint >> 6) & 0x1f) | 0xc0);
        utf8[1] = (char)((codepoint & 0x3f) | 0x80);
        length = 2;
    }
    else if (codepoint <= 0xffff)
    {
        utf8[0] = (char)(((codepoint >> 12) & 0x0f) | 0xe0);
        utf8[1] = (char)(((codepoint >>  6) & 0x3f) | 0x80);
        utf8[2] = (char)((codepoint & 0x3f) | 0x80);
        length = 3;
    }
    else if (codepoint <= 0x10ffff)
    {
        utf8[0] = (char)(((codepoint >> 18) & 0x07) | 0xf0);
        utf8[1] = (char)(((codepoint >> 12) & 0x3f) | 0x80);
        utf8[2] = (char)(((codepoint >>  6) & 0x3f) | 0x80);
        utf8[3] = (char)((codepoint & 0x3f) | 0x80);
        length = 4;
    }

    *byteLength = length;

    return utf8;
}



// Encode text codepoint into utf8 text (memory must be freed!)
char *TextToUtf8(int *codepoints, int length)
{
    // We allocate enough memory fo fit all possible codepoints
    // NOTE: 5 bytes for every codepoint should be enough
    char *text = (char *)calloc(length*5, 1);
    const char *utf8 = NULL;
    int size = 0;

    for (int i = 0, bytes = 0; i < length; i++)
    {
        utf8 = CodepointToUtf8(codepoints[i], &bytes);
        memcpy(text + size, utf8, bytes);
        size += bytes;
    }
    void *ptr = realloc(text, size + 1);
    if (ptr != NULL) text = (char*)ptr;

    return text;
}


int main(int argc, char** argv) {
    int* values = new int[argc - 1];
    for (int i = 1; i < argc; i++) {
        values[i -1] = stoi(argv[i], 0, 16);
    }
    char* result = TextToUtf8(values, argc - 1);
    printf("\"");
    int j = 0;
    while (result[j] != '\0' && j < 10) {
        printf("\\x%02hhX", result[j]);
        j++;
    }
    printf("\"\n\n\n");
}
