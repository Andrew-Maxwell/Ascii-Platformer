#include "effects.hpp"


/******************************************************************************/
//Explosion()
//Spawn particles moving outwards in a circle
/******************************************************************************/

    void explode(int count, float x, float y, Color tint, float newScale, float speed, int c, int lifespan, float elasticity, list<entity*>::iterator zPosition, bool doLighting) {
        if (count > 0) {
            for (float angle = 0; angle < 2 * PI; angle += (2 * PI / count)) {
                float dX = cos(angle);
                float dY = sin(angle);
                if (!(world -> isSolid(x + dX + 0.5, y + dY + 0.5))) {
                    physicalParticle* p = new physicalParticle(x + dX / 1000, y + dY / 1000, tint, newScale, c, elasticity, dX * speed, dY * speed, 3, GRAVITY, FRICTION, lifespan);
                    p -> setDoLighting(doLighting);
                    world -> addParticle(p, zPosition);
                }
            }
        }
    }

    void splash(int count, float x, float y, Color tint, float newScale, float speed, int c, int lifespan, float elasticity, list<entity*>::iterator zPosition) {
        if (count > 0) {
            for (float angle = 0; angle < 2 * PI; angle += (2 * PI / count)) {
                drop* p = new drop(x + cos(angle) / 1000, y + sin(angle) / 1000, tint, newScale, c, elasticity, cos(angle) * speed, sin(angle) * speed, 3, GRAVITY, FRICTION, lifespan);
                world -> addParticle(p, zPosition);
            }
        }
    }

    void broadcast(int distance, float x, float y, Color tint, float scale, bool high) {
        if (!high) {
            tint.a *= 0.5;
        }
        for (float angle = 0; angle < 2 * PI; angle += (2 * PI / (distance + 20))) {
            particle* p = new particle(x + cos(angle) * 2, y + sin(angle) * 2, tint, scale, cos(angle) / 3.0, sin(angle) / 3.0, '~', distance * 3, false);
            p -> setDoLighting(false);
            world -> addEntity(p);
        }
    }

/******************************************************************************/
//DamageIndicator()
//Spawns a little number that floats up when damage is taken/dealt
/******************************************************************************/

void damageIndicator(int damage, float x, float y, Color tint, float newScale) {
    float leftEnd = x - 1 - floor(log10(abs(damage))) / 2;
    string digits = to_string(damage);
    for (int i = 0; i < digits.length(); i++) {
        particle* p = new particle (leftEnd + i, y, tint, newScale, 0, -0.1, digits[i], 60);
        p -> setDoLighting(false);
        world -> addEntity(p);
    }
}

