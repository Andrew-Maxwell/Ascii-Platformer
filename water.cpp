#include "water.hpp"
#include "effects.hpp"

#define PULSESPEED 0.2

/*****************************************************************************/
//Water
//Animates the surface of water
/*****************************************************************************/

    water:: water(float newX, float newY, Color newTint, float newSizeFactor, entityList* newEList, int newWidth, float newDepth, float newWavelength, float newAmplitude) :
        entity (newX, newY, newTint, newSizeFactor),
        width (newWidth),
        depth (newDepth),
        wavelength (newWavelength),
        amplitude (newAmplitude) {
            eList = newEList;
            surface.resize(width, 0.0);
            k = 2 * PI / wavelength;
            omega = sqrt(GRAVITY * k * tanh(k * depth));
            type = 'w';
        }

    bool water::doesCollide(float otherX, float otherY, int type) {
        return (otherX >= x && otherX <= x + width && otherY >= y + surface[otherX] / 8 && otherY <= y + depth);
    }

    collision water::getCollision() {
        return collision('w');
    }

    bool water::stopColliding() {
        return false;
    }

    void water::tickSet(collider& col) {
        time++;
    }

    void water::tickGet(collider& col) {
        list<collision>::iterator colIter = collisions.begin();
        while (colIter != collisions.end()) {
            splashTimes.push_back(time);
            splashPositions.push_back(colIter -> xVal - x);
            splashSizes.push_back(colIter -> yVal);
            splash(col, eList, 40 * abs(colIter -> yVal), colIter -> xVal, y + surface[colIter -> xVal] / 8, tint, sizeFactor, abs(colIter -> yVal) * 0.5, 0, 100, 0.5);
            colIter = collisions.erase(colIter);
        }
    }

    bool water::finalize() {
        return false;
    }

    void water::print(float cameraX, float cameraY, Font displayFont) {
        for (int i = 0; i < width - 1; i++) {
            surface[i] = amplitude * sin(k * i + omega * time);
        }
        for (int i = 0; i < splashTimes.size(); i++) {
            float sTime = splashTimes[i];
            if (time - sTime > width / PULSESPEED) {
                splashTimes.pop_front();
                splashPositions.pop_front();
                splashSizes.pop_front();
            }
            else {
                float location = splashPositions[i];
                float size = splashSizes[i];
                for (int i = 0; i < width - 1; i++) {
                    surface[i] += ( -1 / (pow (i - location - PULSESPEED * (time - sTime), 2) + 0.1 / abs(size)) +
                                    -1 / (pow (i - location + PULSESPEED * (time - sTime), 2) + 0.1 / abs(size)) +
                                    1 / (pow (i - location - PULSESPEED * (time - sTime) - 1.5, 2) + 0.1 / abs(size)) +
                                    1 / (pow (i - location + PULSESPEED * (time - sTime) + 1.5, 2) + 0.1 / abs(size)));
                }
            }
        }
        for (int i = 0; i < width - 1; i++) {
            drawBarUp(cameraX, cameraY, displayFont, x + i, y + depth, tint, sizeFactor, depth * FONTSIZE / 2 + surface[i]);
        }
    }
