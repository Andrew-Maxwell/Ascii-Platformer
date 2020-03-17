#ifndef EFFECTS_HPP
#define EFFECTS_HPP

#include "rain.hpp" //for drop
#include "entity.hpp"
#include "world.hpp"
#include "meta.hpp"
#include "physicalentities.hpp"

using namespace rapidjson;

/******************************************************************************/
//Explosion()
//Spawn particles moving outwards in a circle
//Splash() spawns drops specifically
/******************************************************************************/

void explode(int count, float x, float y, Color tint,
                float newSizeFactor, float speed, int c, int lifespan, float elasticity);


void splash(int count, float x, float y, Color tint,
                float newSizeFactor, float speed, int c, int lifespan, float elasticity);


/******************************************************************************/
//DamageIndicator()
//Spawns a little number that floats up when damage is taken/dealt
/******************************************************************************/

void damageIndicator(int damage, float x, float y, Color tint, float newSizeFactor);

#endif //EFFECTS_HPP
