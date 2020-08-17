#include "particles.hpp"


/******************************************************************************/
//Particle
//Entity represented by any char that moves in a predefined direction until its lifetime runs out.
//If the character passed to constructor is 0, then a character is chosen based on direction.
/******************************************************************************/

    particle::particle(  float newX, float newY, Color newTint,
                        float newScale, float newXSpeed, float newYSpeed, int c, int newLifetime, bool newFade) :
                        entity(newX, newY, newTint, newScale),
                        xSpeed(newXSpeed),
                        ySpeed(newYSpeed),
                        lifetime(newLifetime),
                        startLifetime(newLifetime),
                        startA(tint.a),
                        fade(newFade) {
        if (c == 0) {
            setDirection();
        }
        else {
            char* newDisplay = TextToUtf8(&c, 1);
            for (int i = 0; i < 4; i++) {
                display[i] = newDisplay[i];
            }
            free(newDisplay);
        }
    }

    unsigned int particle::type() {
        return PARTICLETYPE;
    }

    void particle::setDirection() {
        if (xSpeed == 0) {
            if (ySpeed == 0) {
                display[0] = '.';
            }
            else {
                display[0] = '|';
            }
        }
        else {
            if (ySpeed / xSpeed < -1.09) {
                display[0] = '|';
            }
            else if (ySpeed / xSpeed < -0.383) {
                display[0] = '/';
            }
            else if (ySpeed / xSpeed < 0.383) {
                display[0] = '-';
            }
            else if (ySpeed / xSpeed < 1.09) {
                display[0] = '\\';
            }
            else {
                display[0] = '|';
            }
        }
    }

    void particle::tickSet() {
        x += xSpeed;
        y += ySpeed;
        lifetime--;
        if (fade) {
            tint.a = startA * (float)lifetime/(float)startLifetime;
        }
    }

    void particle::tickGet() {}

    bool particle::finalize() {
        return lifetime == 0;
    }

    void particle::print() {
        theScreen -> draw(x, y, tint, scale, display, doLighting, doHighlight);
    }
