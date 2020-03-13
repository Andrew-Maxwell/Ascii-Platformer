#include "particles.hpp"


/******************************************************************************/
//Particle
//Entity represented by any char that moves in a predefined direction until its lifetime runs out.
//If the character passed to constructor is 0, then a character is chosen based on direction.
/******************************************************************************/

    particle::particle(  float newX, float newY, Color newTint,
                        float newSizeFactor, float newXSpeed, float newYSpeed, int c, int newLifetime) :
                        entity(newX, newY, newTint, newSizeFactor),
                        xSpeed(newXSpeed),
                        ySpeed(newYSpeed),
                        lifetime(newLifetime) {
        type = PARTICLETYPE;
        if (c == 0) {
            toPrint = new char[2];
            setDirection();
        }
        else {
            toPrint = TextToUtf8(&c, 1);
        }
    }

    particle::~particle() {
        delete toPrint;
    }

    void particle::setDirection() {
        toPrint[1] = '\0';
        if (xSpeed == 0) {
            toPrint[0] = '|';
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

    void particle::tickSet(collider& col) {
        x += xSpeed;
        y += ySpeed;
        lifetime--;
    }

    void particle::tickGet(collider& col) {}

    bool particle::finalize() {
        if (lifetime == 0) {
            return true;
        }
        else {
            return false;
        }
    }

    void particle::print(float cameraX, float cameraY, Font displayFont) {
        myDrawText(displayFont, toPrint, (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
    }
