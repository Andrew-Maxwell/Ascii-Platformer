#include "charfills.hpp"
#include "meta.hpp"

using namespace rapidjson;


/*****************************************************************************/
//charFill
//Brush defines shape, charFill defines "color": what characters the brush area should be
//filled with.
/*****************************************************************************/

    charFill::charFill() {}

    charFill::charFill(int newCodepoint) {
        codepoint = newCodepoint;
    }

    int charFill::get(int x, int y) {
        return codepoint;
    }

    int charFill::display() {
        return codepoint;
    }

/*****************************************************************************/
//randomCharFill()
//Fills the shape with chars picked randomly from a set
/*****************************************************************************/

    randomCharFill::randomCharFill() {}

    randomCharFill::randomCharFill(vector<int> newCodepoints) {
        codepoints = newCodepoints;
    }

    int randomCharFill::get(int x, int y) {
        return codepoints[GetRandomValue(0, codepoints.size() - 1)];
    }

    int randomCharFill::display() {
        return codepoints[GetRandomValue(0, codepoints.size() - 1)];
    }

/*****************************************************************************/
//pipeCharFill
//Fills the shape with random pipes. Interesting programming problem, haven't done it yet.
/*****************************************************************************/

    int pipeCharFill::get(int x, int y) {
/*        uint8_t up, down, left, right;
        up = (int)(sin((float)x * (y % 4)) * 1000) % 3;
        left = (int)(cos((float)y * (x % 5)) * 100) % 3;
        down = (int)(sin((float)x * ((y + 1) % 4)) * 1000) % 3;
        right = (int)(cos((float)y * ((x + 1) % 5)) * 100) % 3;
*/
        return 0;
    }

/*****************************************************************************/
//gridCharFill
//Fills the shape with different grids of different sizes.
/*****************************************************************************/

    gridCharFill::gridCharFill(int newSize, int newHoriz, int newVert, int newCross) :
        size(newSize),
        horiz(newHoriz),
        vert(newVert),
        cross(newCross) {}

    int gridCharFill::get(int x, int y) {
        if (x % size == 0) {
            if (y % size == 0) {
                return cross;
            }
            return vert;
        }
        else if (y % size == 0) {
            return horiz;
        }
        else {
            return ' ';
        }
    }

    int gridCharFill::display() {
        if (displayCount++ % 60 < 30) {
            return cross;
        }
        else {
            return 48 + size;
        }
    }



/*****************************************************************************/
//diagGridCharFill
//Fills the shape with a diagonal grid of varying size
/*****************************************************************************/

    diagGridCharFill::diagGridCharFill(int newSize, int newUpRight, int newDownRight, int newCross) :
        size(newSize),
        upRight(newUpRight),
        downRight(newDownRight),
        cross(newCross) {}

    int diagGridCharFill::get(int x, int y) {
        if ((x + y) % size == 0) {
            if ((x -y) % size == 0) {
                return cross;
            }
            return upRight;
        }
        else if ((x - y) % size == 0) {
            return downRight;
        }
        else {
            return ' ';
        }
    }

    int diagGridCharFill::display() {
        if (displayCount++ % 60 < 30) {
            return cross;
        }
        else {
            return 48 + size;
        }
    }



/*****************************************************************************/
//textureCharFill
//fills the shape with a predefined repeating texture.
/*****************************************************************************/

    textureCharFill::textureCharFill(vector<vector<int>> newTexture) :
        texture(newTexture) {}

    int textureCharFill::get(int x, int y) {
        return texture[y % texture.size()][x % texture[0].size()];
    }

    int textureCharFill::display() {
        return 'T';
    }


