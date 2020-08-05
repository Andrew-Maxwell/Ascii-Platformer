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
            char* newToPrint = TextToUtf8(&c, 1);
            toPrint[0] = newToPrint[0];
            free(newToPrint);
        }
    }

    unsigned int particle::type() {
        return PARTICLETYPE;
    }

    void particle::setDirection() {
        if (xSpeed == 0) {
            if (ySpeed == 0) {
                toPrint[0] = '.';
            }
            else {
                toPrint[0] = '|';
            }
        }
        else {
            if (ySpeed / xSpeed < -1.09) {
                toPrint[0] = '|';
            }
            else if (ySpeed / xSpeed < -0.383) {
                toPrint[0] = '/';
            }
            else if (ySpeed / xSpeed < 0.383) {
                toPrint[0] = '-';
            }
            else if (ySpeed / xSpeed < 1.09) {
                toPrint[0] = '\\';
            }
            else {
                toPrint[0] = '|';
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
        theScreen -> draw(x, y, tint, scale, toPrint, doLighting);
    }
