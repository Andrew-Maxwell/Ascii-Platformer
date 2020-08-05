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

void explode(int count, float x, float y, Color tint, float newScale, float speed, int c, int lifespan,
             float elasticity, list<entity*>::iterator zPosition, bool doLighting = false);


void splash(int count, float x, float y, Color tint, float newScale, float speed, int c, int lifespan,
            float elasticity, list<entity*>::iterator zPosition);

/******************************************************************************/
//Broadcast()
//Spawns an expanding non-physical bubble of radio waves
/******************************************************************************/

void broadcast(int count, float x, float y, Color tint, float scale, bool high = true);

/******************************************************************************/
//DamageIndicator()
//Spawns a little number that floats up when damage is taken/dealt
/******************************************************************************/

void damageIndicator(int damage, float x, float y, Color tint, float newScale);

#endif //EFFECTS_HPP
