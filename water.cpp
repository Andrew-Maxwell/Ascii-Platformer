#include "water.hpp"
#include "rain.hpp"
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
        }

    unsigned int water::type() {
        return WATERTYPE;
    }

    bool water::doesCollide(float otherX, float otherY, int otherType) {
        return (otherX >= x && otherX <= x + width && otherY >= y - surface[otherX - x] && otherY <= y + depth);
    }

    collision water::getCollision(float otherX, float otherY, int otherType) {
        return collision(WATERTYPE, 0, amplitude * (y + depth - otherY) / depth * sin(k * (otherX - x) + omega * tickCounter) * -1 * omega * copysign(1, wavelength),
                                       amplitude * (y + depth - otherY) / depth * cos(k * (otherX - x) + omega * tickCounter) * -1 * omega);
    }

    bool water::stopColliding() {
        return false;
    }

    void water::tickSet() {}

    void water::tickGet() {

        cout << "splashes: " << splashSizes.size() << endl;
        for (int i = 0; i < width - 1; i++) {
            surface[i] = amplitude * sin(k * i + omega * tickCounter);
        }

        list<collision>::iterator colIter = collisions.begin();
        while (colIter != collisions.end()) {
            if (colIter -> type == FORCEFIELDTYPE) {
                float range = colIter -> damage;
                float power = colIter -> magnitude;
                float otherX = colIter -> xVal, otherY = colIter -> yVal;
                if (abs(y - otherY) <= range && otherX - x > 0 && otherX - x < width) {
                    float xRange = pow(pow(range, 2) - pow(y - otherY, 2), 0.5);
                    for (int i = otherX - x - xRange + 1; i < otherX - x + xRange; i++) {
                        surface[i] += 100 * power * (1 - pow((i + x - otherX) / xRange, 2));
                    }
                    if (y > otherY && power > 0) {
                        for (int i = 0; i < power * 40; i++) {
                            world -> addParticle(new drop(GetRandomValue(otherX - xRange, otherX + xRange),
                                                          y - surface[otherX - x], tint, sizeFactor, 0, 0, 0,
                                                          power * -5, 100, GRAVITY, FRICTION, 100));
                        }
                    }
                }
            }
            else if (colIter -> type = REALPHYSICALENTITYTYPE && abs(colIter -> yVal) > 0.05) {
                splashTimes.push_back(tickCounter);
                splashPositions.push_back(colIter -> xVal - x);
                splashSizes.push_back(colIter -> yVal);
                splash(40 * abs(colIter -> yVal), colIter -> xVal, y - surface[colIter -> xVal - x], tint, sizeFactor, abs(colIter -> yVal) * 0.5, 0, 100, 0.5);
            }
            colIter = collisions.erase(colIter);
        }
        for (int i = 0; i < splashTimes.size(); i++) {
            int ticksPassed = tickCounter - splashTimes[i];
            if (ticksPassed > width / PULSESPEED) {
                splashTimes.pop_front();
                splashPositions.pop_front();
                splashSizes.pop_front();
            }
            else {
                location = splashPositions[i];
                size = abs(splashSizes[i]);
                
                for (int i = 0; i < width - 1; i++) {
                    surface[i] += ( -2 * size * ticksPassed / (pow (i - location - PULSESPEED * ticksPassed, 2) + ticksPassed) +
                                    -2 * size * ticksPassed / (pow (i - location + PULSESPEED * ticksPassed, 2) + ticksPassed) +
                                     2 * size * ticksPassed / (pow (i - location - PULSESPEED * ticksPassed - 1.5, 2) + ticksPassed) +
                                     2 * size * ticksPassed / (pow (i - location + PULSESPEED * ticksPassed + 1.5, 2) + ticksPassed));
                }
            }
        }
    }

    bool water::finalize() {
        return false;
    }

    void water::print() {
        Vector2 camera = theCanvas -> getCamera();
        for (int i = max(0, (int)(camera.x - x - theCanvas -> getScreenCols() / sizeFactor / 2.0f));
                i < min(width - 1, (int)(camera.x - x + theCanvas -> getScreenCols() / sizeFactor / 2.0f + 1));
                i++) {
            theCanvas -> drawBarUp(x + i, y + depth + 1, tint, sizeFactor, depth + surface[i]);
        }
    }
