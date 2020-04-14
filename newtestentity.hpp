#ifndef NEWTESTENTITY_HPP
#define NEWTESTENTITY_HPP

#include "world.hpp"
#include "effects.hpp"
#include "entity.hpp"
#include "meta.hpp"

using namespace rapidjson;

class newTestEntity : protected physicalEntity, virtual public collideable {

    int health, hurtTimer = 0;
    int damage;
    bool isDead = false;

    public:

    explicit newTestEntity(  float newX, float newY, Color newTint, float newSizeFactor, int displayChar, float elasticity, float newXMomentum,
                                float newYMomentum, float newMaxSpeed, float newGravity, float newFriction, int maxHealth, int newDamage);

    unsigned int type();

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision(float otherX, float otherY, int otherType);

    bool stopColliding();

    void tickSet();

    void tickGet();

    bool finalize();

    void print();
};

#endif //NEWTESTENTITY_HPP
