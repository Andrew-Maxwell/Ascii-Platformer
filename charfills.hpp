#include "meta.hpp"

#ifndef CHARFILLS_HPP
#define CHARFILLS_HPP

using namespace rapidjson;


/*****************************************************************************/
//charFill
//Brush defines shape, charFill defines "color": what characters the brush area should be
//filled with.
/*****************************************************************************/

struct charFill {

    int codepoint;

    charFill();

    charFill(int newCodepoint);

    virtual int get(int x, int y);

    virtual int display();
};

/*****************************************************************************/
//randomCharFill()
//Fills the shape with chars picked randomly from a set
/*****************************************************************************/

struct randomCharFill : public charFill {

    vector<int> codepoints;

    randomCharFill();

    randomCharFill(vector<int> newCodepoints);

    int get(int x, int y) override;

    int display();
};

/*****************************************************************************/
//pipeCharFill
//Fills the shape with random pipes. Interesting programming problem, haven't done it yet.
/*****************************************************************************/

struct pipeCharFill : public charFill {

    int get(int x, int y);
};

/*****************************************************************************/
//gridCharFill
//Fills the shape with different grids of different sizes.
/*****************************************************************************/

struct gridCharFill : public charFill {

    int size, horiz, vert, cross, displayCount = 0;

    gridCharFill(int newSize, int newHoriz, int newVert, int newCross);

    int get(int x, int y);

    int display();
};

/*****************************************************************************/
//diagGridCharFill
//Fills the shape with a diagonal grid of varying size
/*****************************************************************************/

struct diagGridCharFill : public charFill {

    int size, upRight, downRight, cross, displayCount = 0;

    diagGridCharFill(int newSize, int newUpRight, int newDownRight, int newCross);

    int get(int x, int y);

    int display();
};

/*****************************************************************************/
//textureCharFill
//fills the shape with a predefined repeating texture.
/*****************************************************************************/

struct textureCharFill : public charFill {

    vector<vector<int>> texture;

    textureCharFill(vector<vector<int>> newTexture);

    int get(int x, int y);

    int display();
};

#endif //CHARFILLS_HPP
