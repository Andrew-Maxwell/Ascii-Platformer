#include "effects.hpp"


/******************************************************************************/
//Explosion()
//Spawn particles moving outwards in a circle
/******************************************************************************/

    void explode(int count, float x, float y, Color tint,
                    float newSizeFactor, float speed, int c, int lifespan, float elasticity) {
        if (count > 0) {
            for (float angle = 0; angle < 2 * M_PI; angle += (2 * M_PI / count)) {
                physicalParticle* p = new physicalParticle(x + cos(angle) / 1000, y + sin(angle) / 1000, tint, newSizeFactor, c, elasticity, cos(angle) * speed, sin(angle) * speed, 3, GRAVITY, FRICTION, lifespan);
                world -> addParticle(p);
            }
        }
    }

    void splash(int count, float x, float y, Color tint,
                    float newSizeFactor, float speed, int c, int lifespan, float elasticity) {
        if (count > 0) {
            for (float angle = 0; angle < 2 * M_PI; angle += (2 * M_PI / count)) {
                drop* p = new drop(x + cos(angle) / 1000, y + sin(angle) / 1000, tint, newSizeFactor, c, elasticity, cos(angle) * speed, sin(angle) * speed, 3, GRAVITY, FRICTION, lifespan);
                world -> addParticle(p);
            }
        }
    }


/******************************************************************************/
//DamageIndicator()
//Spawns a little number that floats up when damage is taken/dealt
/******************************************************************************/

void damageIndicator(int damage, float x, float y, Color tint, float newSizeFactor) {
    float leftEnd = x - 1 - floor(log10(abs(damage))) / 2;
    string digits = to_string(damage);
    for (int i = 0; i < digits.length(); i++) {
        world -> addEntity(new particle (leftEnd + i, y, tint, newSizeFactor, 0, -0.1, digits[i], 60));
    }
}

