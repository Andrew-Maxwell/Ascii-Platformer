#include "particles.hpp"

/******************************************************************************/
//Particle
//Entity represented by any char that moves in a predefined direction until its lifetime runs out.
//If the character passed to constructor is 0, then a character is chosen based on direction.
/******************************************************************************/

    particle::particle(  float newX, float newY, Color newTint,
                        float newScale, float newXSpeed, float newYSpeed, vector<int> newDisplayList, int newDisplayMix, int newLifetime, bool newFade) :
                        entity(newX, newY, newTint, newScale),
                        xSpeed(newXSpeed),
                        ySpeed(newYSpeed),
                        lifetime(newLifetime),
                        startLifetime(newLifetime),
                        startA(tint.a),
                        fade(newFade),
                        displayList(newDisplayList),
                        displayMix(newDisplayMix) {
        if (displayList.size() > 0) {   //Randomly switching displays
            changeDisplay();
        }
        else {
            setDirection();             //Dynamic particle (switches based on direction)
        }
    }

    particle::particle(  float newX, float newY, Color newTint,
                        float newScale, float newXSpeed, float newYSpeed, int newDisplay, int newLifetime, bool newFade) :
                        entity(newX, newY, newTint, newScale),
                        xSpeed(newXSpeed),
                        ySpeed(newYSpeed),
                        lifetime(newLifetime),
                        startLifetime(newLifetime),
                        startA(tint.a),
                        fade(newFade) {
                            if (newDisplay == 0) {
                                setDirection();
                            }
                            else {
                                display = utf8(newDisplay);
                            }
                        }

    //Static particle (displays a string)
    particle::particle(  float newX, float newY, Color newTint,
                        float newScale, float newXSpeed, float newYSpeed, string newDisplay, int newLifetime, bool newFade) :
                        entity(newX, newY, newTint, newScale),
                        xSpeed(newXSpeed),
                        ySpeed(newYSpeed),
                        lifetime(newLifetime),
                        startLifetime(newLifetime),
                        startA(tint.a),
                        fade(newFade),
                        display(newDisplay) {}

    unsigned int particle::type() {
        return PARTICLETYPE;
    }

    void particle::setDirection() {
        if (xSpeed == 0) {
            if (ySpeed == 0) {
                display = ".";
            }
            else {
                display = "|";
            }
        }
        else {
            if (ySpeed / xSpeed < -1.09) {
                display = "|";
            }
            else if (ySpeed / xSpeed < -0.383) {
                display = "/";
            }
            else if (ySpeed / xSpeed < 0.383) {
                display = "-";
            }
            else if (ySpeed / xSpeed < 1.09) {
                display = "\\";
            }
            else {
                display = "|";
            }
        }
    }

    Rectangle particle::changeDisplay() {
        int newDisplay = displayList[GetRandomValue(0, displayList.size() - 1)];
        display = utf8(newDisplay);
        Rectangle charRectangle = getCharRectangle(newDisplay);
        xOffset = -1 * charRectangle.x;
        yOffset = -1 * charRectangle.y;
        return charRectangle;
    };

    void particle::tickSet() {
        x += xSpeed;
        y += ySpeed;
        lifetime--;
        if (fade) {
            tint.a = max(0.0f, startA * (float)lifetime / (float)startLifetime);
        }
    }

    void particle::tickGet() {}

    bool particle::finalize() {
        return lifetime == 0;
    }

    void particle::print() {
        if (displayList.size() > 1 && GetRandomValue(0, displayMix) == 0) {
            changeDisplay();
        }
        theScreen -> draw(x + xOffset, y + yOffset, tint, scale, display, doLighting, doHighlight);
    }

/******************************************************************************/
//physicalParticle
//A physicalEntity with some particle characteristics
/******************************************************************************/

    physicalParticle::physicalParticle( float newX, float newY, Color newTint, float newScale, vector<int> newDisplayList, int newDisplayMix, float newElasticity, float newXMomentum,
        float newYMomentum, float newMaxSpeed, bool newWaterDelete, bool newHitDelete, bool newRestDelete, float newWobble, float newGravity, float newFriction, int newLifetime, bool newFade) :
        entity(newX, newY, newTint, newScale),
        physicalEntity(newX, newY, newTint, newScale, 0.5, 0.5, newElasticity, newXMomentum, newYMomentum, newMaxSpeed, newGravity, newFriction),
        particle(newX, newY, newTint, newScale, newXMomentum, newYMomentum, newDisplayList, newDisplayMix, newLifetime, newFade),
        dynamicChar(newDisplayList.size() == 0),
        lifetime(newLifetime),
        wobble(newWobble),
        waterDelete(newWaterDelete),
        hitDelete(newHitDelete),
        restDelete(newRestDelete) {
            Rectangle displayDimensions = particle::changeDisplay();
            width = displayDimensions.width;
            height = displayDimensions.height;
    }

    physicalParticle::physicalParticle( float newX, float newY, Color newTint, float newScale, string newDisplay, float newElasticity, float newXMomentum,
        float newYMomentum, float newMaxSpeed, bool newWaterDelete, bool newHitDelete, bool newRestDelete, float newWobble, float newGravity, float newFriction, int newLifetime, bool newFade) :
        entity(newX, newY, newTint, newScale),
        physicalEntity(newX, newY, newTint, newScale, 0.5, 0.5, newElasticity, newXMomentum, newYMomentum, newMaxSpeed, newGravity, newFriction),
        particle(newX, newY, newTint, newScale, newXMomentum, newYMomentum, newDisplay, newLifetime, newFade),
        dynamicChar(false),
        lifetime(newLifetime),
        wobble(newWobble),
        waterDelete(newWaterDelete),
        hitDelete(newHitDelete),
        restDelete(newRestDelete) {
            width = display.size();
            height = 1;
            xOffset = yOffset = 0;
    }

    physicalParticle::physicalParticle( float newX, float newY, Color newTint, float newScale, int newDisplay, float newElasticity, float newXMomentum,
        float newYMomentum, float newMaxSpeed, bool newWaterDelete, bool newHitDelete, bool newRestDelete, float newWobble, float newGravity, float newFriction, int newLifetime, bool newFade) :
        entity(newX, newY, newTint, newScale),
        physicalEntity(newX, newY, newTint, newScale, 0.5, 0.5, newElasticity, newXMomentum, newYMomentum, newMaxSpeed, newGravity, newFriction),
        particle(newX, newY, newTint, newScale, newXMomentum, newYMomentum, newDisplay, newLifetime, newFade),
        dynamicChar(newDisplay == 0),
        lifetime(newLifetime),
        wobble(newWobble),
        waterDelete(newWaterDelete),
        hitDelete(newHitDelete),
        restDelete(newRestDelete) {
            Rectangle displayDimensions = getCharRectangle(newDisplay);
            width = displayDimensions.width;
            height = displayDimensions.height;
            xOffset = -1 * displayDimensions.x;
            yOffset = -1 * displayDimensions.y;
    }

    unsigned int physicalParticle::type() {
        return PHYSICALPARTICLETYPE;
    }

    bool physicalParticle::doesCollide(float otherX, float otherY, int otherType, unsigned int otherID) {
        return false;
    }

    bool physicalParticle::stopColliding() {
        return shouldDelete;
    }

    void physicalParticle::tickSet() {
        float speed = abs(xMomentum) + abs(yMomentum);
        xMomentum += GetRandomValue(-1 * speed * wobble * 1000000, speed * wobble * 1000000) / 1000000.0;
        yMomentum += GetRandomValue(-1 * speed * wobble * 1000000, speed * wobble * 1000000) / 1000000.0;
    }

    void physicalParticle::tickGet() {
        physicalEntity::tickGet();
        //Delete if in a wall, or timed out, or stopped moving, or underwater and waterDelete, or fell out of level.
        if (world -> isSolid(x + width / 2, y + height / 2) ||
            lifetime-- < 0 ||
            (restDelete && !isUnderWater && xMomentum < 0.01 && xMomentum > -0.01 && yMomentum < 0.01 && yMomentum > -0.01) ||
            (waterDelete && isUnderWater) ||
            (hitDelete && (hitX || hitY)) ||
            y > world -> getRows() + 100) {
                shouldDelete = true;
        }
        if (fade) {
            tint.a = max(0.0f, startA * (float)lifetime / (float)startLifetime);
        }
    }

    bool physicalParticle::finalize() {
        return shouldDelete;
    }

    void physicalParticle::print() {
        if (dynamicChar) {
            xSpeed = xMomentum;
            ySpeed = yMomentum;
            particle::setDirection();
        }
        if (displayList.size() > 1 && GetRandomValue(0, displayMix) == 0) {
            Rectangle newCharDimensions = changeDisplay();
            width = newCharDimensions.width;
            height = newCharDimensions.height;
        }
        theScreen -> draw(x + xOffset, y + yOffset, tint, scale, display, doLighting, doHighlight);
    }


/******************************************************************************/
//particleSpawner
//Constantly spawns particles above the top of the screen
/******************************************************************************/

//Consider adding randomized colors within a given range

    particleSpawner::particleSpawner(
        float newX, float newY, Color newTint, float newScale, float newWidth, float newHeight, 
        float newDepth, int newChannel, vector<int>& newDisplayChars, int newMix, float newElasticity, float newXMomentum, 
        float newYMomentum, float newMaxSpeed, bool newWaterDelete, bool newHitDelete, bool newRestDelete, float newWobble,
        float newGravity, float newFriction, int newLifetime, float newSpawnsPerTick, bool newFade) :
            entity(newX, newY, newTint, newScale),
            width(newWidth),
            height(newHeight),
            depth(newDepth),
            channel(newChannel),
            displayChars(newDisplayChars),
            mix(newMix),
            elasticity(newElasticity),
            xMomentum(newXMomentum),
            yMomentum(newYMomentum),
            maxSpeed(newMaxSpeed),
            waterDelete(newWaterDelete),
            hitDelete(newHitDelete),
            restDelete(newRestDelete),
            wobble(newWobble),
            gravity(newGravity),
            friction(newFriction),
            lifetime(newLifetime),
            spawnsPerTick(newSpawnsPerTick),
            fade(newFade) {
            
            if (channel == -1) {
                for (int i = 0; i < min(int(world -> getRows() / newMaxSpeed), lifetime); i++) {
                    spawnBuffer += spawnsPerTick;
                    while (spawnBuffer > 1) {
                        spawnBuffer--;
                        float newX = GetRandomValue(x * 1000000, (x + width) * 1000000) / 1000000.0;
                        float newY = GetRandomValue(y * 1000000, (y + height) * 1000000) / 1000000.0;
                        float newScale = GetRandomValue(scale * 1000000, (scale + depth) * 1000000) / 1000000.0;
                        physicalParticle* p = new physicalParticle(
                            newX, newY, tint, newScale, displayChars, mix, elasticity,
                            xMomentum, yMomentum, maxSpeed, waterDelete, hitDelete, restDelete,
                            wobble, gravity, friction, lifetime, fade);
                        initParticles.push_back(p);
                    }
                    for (auto eachParticle : initParticles) {
                        eachParticle -> tickSet();
                    }
                    for (auto eachParticle : initParticles) {
                        eachParticle -> tickGet();
                    }
                    auto partIter = initParticles.begin();
                    while (partIter != initParticles.end()) {
                        if ((*partIter) -> finalize()) {
                            delete *partIter;
                            *partIter = NULL;
                            partIter = initParticles.erase(partIter);
                        }
                        else {
                            partIter++;
                        }
                    }
                }
            }
        }

    unsigned int particleSpawner::type() {
        return PARTICLESPAWNERTYPE;
    }

    void particleSpawner::tickSet() {
        for (physicalParticle* eachParticle : initParticles) {
            world -> addParticle(eachParticle, zPosition);
        }
        initParticles.clear();
        if (world -> getChannel(channel) || channel == -1) {
            spawnBuffer += spawnsPerTick;
            while (spawnBuffer > 1) {
                spawnBuffer--;
                float newX = GetRandomValue(x * 1000000, (x + width) * 1000000) / 1000000.0;
                float newY = GetRandomValue(y * 1000000, (y + height) * 1000000) / 1000000.0;
                float newScale = GetRandomValue(scale * 1000000, (scale + depth) * 1000000) / 1000000.0;
                physicalParticle* p = new physicalParticle(
                    newX, newY, tint, newScale, displayChars, mix, elasticity,
                    xMomentum, yMomentum, maxSpeed, waterDelete, hitDelete, restDelete,
                    wobble, gravity, friction, lifetime, fade);
                world -> addParticle(p, zPosition);
            }
        }
        else {
            spawnBuffer = 0;
        }
    }

    void particleSpawner::tickGet() {}

    bool particleSpawner::finalize() {
        return false;
    }

    void particleSpawner::print() {}

