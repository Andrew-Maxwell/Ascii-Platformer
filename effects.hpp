#ifndef EFFECTS_HPP
#define EFFECTS_HPP

#include "col.hpp"
#include "entitymeta.hpp"
#include "meta.hpp"
#include "particles.hpp"

using namespace rapidjson;

/******************************************************************************/
//Explosion()
//Spawn particles moving outwards in a circle
/******************************************************************************/

void explosion(collider& col, entityList& entities, int count, float x, float y, Color tint,
                float newSizeFactor, float speed, char c, int lifespan, float elasticity);

/******************************************************************************/
//DamageIndicator()
//Spawns a little number that floats up when damage is taken/dealt
/******************************************************************************/

void damageIndicator(entityList& entities, int damage, float x, float y, Color tint, float newSizeFactor);

#endif //EFFECTS_HPP
