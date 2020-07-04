#include "water.hpp"
#include "rain.hpp"
#include "effects.hpp"

/*****************************************************************************/
//Water
//Animates the surface of water
/*****************************************************************************/

    water:: water(float newX, float newY, Color newTint, float newSizeFactor,  int newWidth, float newheight, float newWavelength, float newAmplitude) :
        entity(newX, newY, newTint, newSizeFactor),
        width(newWidth),
        height(newheight),
        wavelength(newWavelength),
        amplitude(newAmplitude) {
            surface.resize(width + 1, 0.0);
            lastSurface.resize(width + 1, 0.0);
            k = 2 * PI / wavelength;
            omega = sqrt(GRAVITY * k * tanh(k * height));
            pulseSpeed = omega / k;
        }

    unsigned int water::type() {
        return WATERTYPE;
    }

    float water::calculateSurface(float atX, int tickOffset) {

        //We start with a sine wave to represent regular waves

        float surface = amplitude * sin(k * atX + omega * (tickCounter + tickOffset));

        //Calculate waves caused by splashes
        //Each splash wave is represented by wave pulses of the form
        //      size * spread / (x^2 + spread)
        //size represents height of wave pulse, spread width
        //x is adjusted for time, spread increases with time
        //There are four pulses, two in each direction, one up, one down

        for (int i = 0; i < splashTimes.size(); i++) {
            int ticksPassed = tickCounter - splashTimes[i] + tickOffset;
            float spread = (ticksPassed + 10) / 4.0f;
            float location = splashPositions[i];
            float size = 2.0f * splashSizes[i];
            surface += (-1 * size * spread / (pow (atX - location - pulseSpeed * ticksPassed, 2) + spread) +
                        -1 * size * spread / (pow (atX - location + pulseSpeed * ticksPassed, 2) + spread) +
                         1 * size * spread / (pow (atX - location - pulseSpeed * ticksPassed - 1.5, 2) + spread) +
                         1 * size * spread / (pow (atX - location + pulseSpeed * ticksPassed + 1.5, 2) + spread));
        }
        return surface;
    }


    bool water::doesCollide(float otherX, float otherY, int otherType) {
        return (otherX >= x && otherX < x + width && otherY >= y - surface[otherX - x] - 0.5 && otherY <= y + height);
    }

    collision water::getCollision(float otherX, float otherY, int otherType) {

        /*We calculate y force as d/dt (surface) so that objects move up and down
        more or less as fast as the water surface does.
        force is d/dx (d/dt (surface)).
        Also, the forces are smaller at deeper heights.*/

        float heightFactor = min(1.0, pow(2, (y - otherY - 1) / abs(wavelength) * 9));
        float yForce1 = -1 * heightFactor * (surface[otherX - x] - lastSurface[otherX - x]);
        float yForce2 = -1 * heightFactor * (surface[otherX - x + 1] - lastSurface[otherX - x + 1]);
        float xForce = 3 * (yForce1 - yForce2);

        return collision(WATERTYPE, 0, xForce, (yForce1 + yForce2) / 2);

    }

    bool water::stopColliding() {
        return false;
    }

    void water::tickSet() {}

    void water::tickGet() {

        lastSurface = surface;

        /*Turn collisions into splashes.
        Each splash is represented visually by a wave pulse function
        All of the wave pulse functions are added together (superposition
        to produce the final surface.*/

        list<collision>::iterator colIter = collisions.begin();
        while (colIter != collisions.end()) {
            if (colIter -> type == FORCEFIELDTYPE) {
                float range = colIter -> damage;
                float otherX = colIter -> xVal, otherY = colIter -> yVal;
                if (abs(y - otherY) <= range && otherX - x > 0 && otherX - x < width) {
                    splashTimes.push_back(tickCounter);
                    splashPositions.push_back(colIter -> xVal - x);
                    splashSizes.push_back(-4 * copysign(1, wavelength) * colIter -> magnitude);

                    //If appropriate, spawn drops on surface of water

                    float power = colIter -> magnitude;
                    float xRange = pow(pow(range, 2) - pow(y - otherY, 2), 0.5);
                    if (y > otherY && power > 0) {
                        for (int i = 0; i < power * 40; i++) {
                            world -> addParticle(new drop(GetRandomValue(colIter -> xVal - xRange, colIter -> xVal + xRange),
                                y - surface[otherX - x], tint, sizeFactor, 0, 0, 0,
                                power * -5, 100, GRAVITY, FRICTION, 100), zPosition);
                        }
                    }
                }
            }
            else if (colIter -> type == PHYSICALENTITYTYPE) {
                splashTimes.push_back(tickCounter - 6);
                splashPositions.push_back(colIter -> xVal - x);
                splashSizes.push_back(-2 * colIter -> yVal);

                //Spawn some particles (this is splash() from effects.cpp)

                splash(40 * abs(colIter -> yVal), colIter -> xVal, y - surface[colIter -> xVal - x], tint, sizeFactor, abs(colIter -> yVal) * 0.5, 0, 100, 0.5, zPosition);
            }
            colIter = collisions.erase(colIter);
        }

        //Remove old splashes that have traveled past the boundaries of the water

        while (splashTimes.size() > 0 && tickCounter - splashTimes[0] > abs(width / pulseSpeed)) {
            splashTimes.pop_front();
            splashPositions.pop_front();
            splashSizes.pop_front();
        }

        //Calculate the surface

        for (int j = 0; j < width; j++) {
            surface[j] = calculateSurface(j, 0);
        }
    }

    bool water::finalize() {
        return false;
    }

    void water::print() {
        Vector2 camera = theScreen -> getCamera();
        for (int i = max(0, (int)(camera.x - x - theScreen -> getScreenCols() / sizeFactor / 2.0f));
                i < min(width, (int)(camera.x - x + theScreen -> getScreenCols() / sizeFactor / 2.0f + 1));
                i++) {
            theScreen -> drawBarUp(x + i, y + height, tint, sizeFactor, height + surface[i], doLighting);
        }
    }
