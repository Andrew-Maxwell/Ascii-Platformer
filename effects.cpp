#include "effects.hpp"


/******************************************************************************/
//Explosion()
//Spawn particles moving outwards in a circle
/******************************************************************************/

    void explosion( collider& col, entityList& entities, int count, float x, float y, Color tint,
                    float newSizeFactor, float speed, char c, int lifespan, float elasticity) {
        for (float angle = 0; angle < 2 * M_PI; angle += (2 * M_PI / count)) {
            physicalParticle* p = new physicalParticle(x, y, tint, newSizeFactor, cos(angle) * speed, sin(angle) * speed, c, lifespan, elasticity);
            entities.addEntity(p);
            col.addParticle(p);
        }
    }

/******************************************************************************/
//DamageIndicator()
//Spawns a little number that floats up when damage is taken/dealt
/******************************************************************************/

void damageIndicator(entityList& entities, int damage, float x, float y, Color tint, float newSizeFactor) {
    float leftEnd = x - 1 - floor(log10(abs(damage))) / 2;
    string digits = to_string(damage);
    for (int i = 0; i < digits.length(); i++) {
        entities.addEntity (new particle (leftEnd + i, y, tint, newSizeFactor, 0, -0.1, digits[i], 60));
    }
}
