#ifndef EFFECTS_HPP
#define EFFECTS_HPP

#include "rain.hpp" //for drop
#include "col.hpp"
#include "entitymeta.hpp"
#include "meta.hpp"
#include "physicalentities.hpp"

using namespace rapidjson;

/******************************************************************************/
//Explosion()
//Spawn particles moving outwards in a circle
//Splash() spawns drops specifically
/******************************************************************************/

void explosion(collider& col, entityList* entities, int count, float x, float y, Color tint,
                float newSizeFactor, float speed, int c, int lifespan, float elasticity);


void splash(collider& col, entityList* entities, int count, float x, float y, Color tint,
                float newSizeFactor, float speed, int c, int lifespan, float elasticity);


/******************************************************************************/
//DamageIndicator()
//Spawns a little number that floats up when damage is taken/dealt
/******************************************************************************/

void damageIndicator(entityList* entities, int damage, float x, float y, Color tint, float newSizeFactor);

/******************************************************************************/
//drawBar...()
//Draws a bar of length n pixels starting at x, y (level coordinates) in the direction specified.
//Due to character limitations, for down and left, rounds to four pixels.
/******************************************************************************/

void drawBarLeft (float cameraX, float cameraY, Font displayFont, float x, float y, Color tint, float sizeFactor, int length);

void drawBarRight (float cameraX, float cameraY, Font displayFont, float x, float y, Color tint, float sizeFactor, int length);

void drawBarUp (float cameraX, float cameraY, Font displayFont, float x, float y, Color tint, float sizeFactor, int length);

void drawBarDown (float cameraX, float cameraY, Font displayFont, float x, float y, Color tint, float sizeFactor, int length);

/******************************************************************************/
//drawHudBar...()
//Draws a bar of length n pixels starting at x, y (screen coordinates) in the direction specified
//Due to character limitations, for down and left, roounds to four pixels.
/******************************************************************************/

void drawHudBarLeft (Font displayFont, int x, int y, Color tint, int length);

void drawHudBarRight (Font displayFont, int x, int y, Color tint, int length);

void drawHudBarUp (Font displayFont, int x, int y, Color tint, int length);

void drawHudBarDown (Font displayFont, int x, int y, Color tint, int length);


#endif //EFFECTS_HPP
