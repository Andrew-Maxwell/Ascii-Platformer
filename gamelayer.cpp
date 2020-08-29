#include "gamelayer.hpp"

/******************************************************************************/
/*gameLayer
Implements layer functionality (e.g. animations) which the editor doesn't use.*/
/******************************************************************************/

    gameLayer::gameLayer( float newx, float newy, Color newTint, float newScale, string newFileName) :
        entity(newx, newy, newTint, newScale),
        layer(newx, newy, newTint, newScale, newFileName) {
            layer::readLayer();
            tex = LoadRenderTexture(getCols() * 8, getRows() * 8);
            layer::render();
            baseColor = tint;
        }

    gameLayer::gameLayer(gameLayer& other) :
        entity(other.x, other.y, other.tint, other.scale),
        layer(other.x, other.y, other.tint, other.scale, other.fileName) {
            layer::readLayer();
            tex = LoadRenderTexture(getCols() * 8, getRows() * 8);
            layer::render();
            baseColor = other.baseColor;
            baseDoLighting = other.baseDoLighting;
            baseDoHighlight = other.baseDoHighlight;
            colorEffects = other.colorEffects;
            movementEffects = other.movementEffects;
    }

    gameLayer::~gameLayer() {
        UnloadRenderTexture(tex);
        cout << "gameLayer destroyed\n";
    }

    void gameLayer::setLighting(bool newDoLighting, bool newDoHighlight) {
        doLighting = baseDoLighting = newDoLighting;
        doHighlight = baseDoHighlight = newDoHighlight;
    }

    void gameLayer::addColorEffect(int channel, Color tint, bool doLighting, bool doHighlight) {
        colorEffect c;
        c.channel = channel;
        c.tint = tint;
        c.doLighting = doLighting;
        c.doHighlight = doHighlight;
        colorEffects.push_back(c);;
    }

    void gameLayer::addMovementEffect(int channel, int type, bool x, bool y, bool z, int period, float value) {
        movementEffect m;
        m.channel = channel;
        m.type = type;
        m.x = x;
        m.y = y;
        m.z = z;
        m.period = period;
        m.value = value;
        movementEffects.push_back(m);
    }

    float movement(movementEffect m) {
        float move = 0;
        switch(m.type) {
            case 1: //Random
                move = GetRandomValue(m.value * -1000000, m.value * 1000000) / 1000000.0;
                break;
            case 2: //Sawtooth wave
                move = (tickCounter % m.period) * m.value / m.period;
                break;
            case 3: //Sine wave
                move = sin(tickCounter / m.period) * m.value;
                break;
            case 4: //cos (can use to make circular effects)
                move = cos(tickCounter / m.period) * m.value;
                break;
            case 5: //Square wave
                if ((tickCounter / m.period) % 2 == 0) {
                    move = m.value;
                }
                break;
            default:
                cout << "Bad gamelayer movement type: " << m.type << endl;
        }
        return move;
    }

    void gameLayer::tickGet() {
        tint = baseColor;
        doLighting = baseDoLighting;
        doHighlight = baseDoHighlight;
        offsetX = offsetY = offsetZ = 0;
        for (colorEffect c : colorEffects) {
            if (world -> getChannel(c.channel)) {
                tint = c.tint;
                doLighting = c.doLighting;
                doHighlight = c.doHighlight;
            }
        }
        for (movementEffect m : movementEffects) {
            if (world -> getChannel(m.channel)) {
                if (m.x) {
                    offsetX += movement(m);
                }
                if (m.y) {
                    offsetY += movement(m);
                }
                if (m.z) {
                    offsetZ += movement(m);
                }
            }
        }
    }

    //scale = 1 / distance

    void gameLayer::print() {
        theScreen -> drawLayer(x + offsetX, y + offsetY, tint, 1 / (1 / scale + offsetZ), tex.texture, doLighting, doHighlight);
    }

    unsigned int gameLayer::type() {
        return GAMELAYERTYPE;
    }
