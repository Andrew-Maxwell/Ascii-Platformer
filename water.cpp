#include "water.hpp"
#include "effects.hpp"

#define PULSESPEED 0.2

/*****************************************************************************/
//Water
//Animates the surface of water
/*****************************************************************************/

    water:: water(float newX, float newY, Color newTint, float newSizeFactor,  int newWidth, float newDepth, float newWavelength, float newAmplitude) :
        entity (newX, newY, newTint, newSizeFactor),
        width (newWidth),
        depth (newDepth),
        wavelength (newWavelength),
        amplitude (newAmplitude) {
            
            surface.resize(width, 0.0);
            k = 2 * PI / wavelength;
            omega = sqrt(GRAVITY * k * tanh(k * depth));
            type = WATERTYPE;
        }

    bool water::doesCollide(float otherX, float otherY, int type) {
        return (otherX >= x && otherX <= x + width && otherY >= y - surface[otherX - x] && otherY <= y + depth);
    }

    collision water::getCollision(float otherX, float otherY, int otherType) {
        return collision(WATERTYPE, 0, amplitude * (y + depth - otherY) / depth * sin(k * (otherX - x) + omega * time) * -1 * omega * copysign(1, wavelength),
                                       amplitude * (y + depth - otherY) / depth * cos(k * (otherX - x) + omega * time) * -1 * omega);
    }

    bool water::stopColliding() {
        return false;
    }

    void water::tickSet() {
        time++;
    }

    void water::tickGet() {
        for (int i = 0; i < width - 1; i++) {
            surface[i] = amplitude * sin(k * i + omega * time);
        }

        list<collision>::iterator colIter = collisions.begin();
        while (colIter != collisions.end()) {
            splashTimes.push_back(time);
            splashPositions.push_back(colIter -> xVal - x);
            splashSizes.push_back(colIter -> yVal);
            splash(40 * abs(colIter -> yVal), colIter -> xVal, y - surface[colIter -> xVal - x], tint, sizeFactor, abs(colIter -> yVal) * 0.5, 0, 100, 0.5);
            colIter = collisions.erase(colIter);
        }
        for (int i = 0; i < splashTimes.size(); i++) {
            sTime = splashTimes[i];
            if (time - sTime > width / PULSESPEED) {
                splashTimes.pop_front();
                splashPositions.pop_front();
                splashSizes.pop_front();
            }
            else {
                location = splashPositions[i];
                size = splashSizes[i];
                for (int i = 0; i < width - 1; i++) {
                    surface[i] += ( -1 / (pow (i - location - PULSESPEED * (time - sTime), 2) + 0.9 / abs(size)) +
                                    -1 / (pow (i - location + PULSESPEED * (time - sTime), 2) + 0.9 / abs(size)) +
                                    1 / (pow (i - location - PULSESPEED * (time - sTime) - 1.5, 2) + 0.9 / abs(size)) +
                                    1 / (pow (i - location + PULSESPEED * (time - sTime) + 1.5, 2) + 0.9 / abs(size)));
                }
            }
        }
    }

    bool water::finalize() {
        return false;
    }

    void water::print(float cameraX, float cameraY, Font displayFont) {
        for (int i = 0; i < width - 1; i++) {
            drawBarUp(cameraX, cameraY, displayFont, x + i, y + depth, tint, sizeFactor, depth * FONTSIZE / 2 + surface[i] * 8);
        }
    }
