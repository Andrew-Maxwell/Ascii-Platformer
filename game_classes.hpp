#ifndef GAME_CLASSES_HPP
#define GAME_CLASSES_HPP

using namespace std;

//Forward declarations, not sure why this fixes things but it does.

/*****************************************************************************/
//Door
//Triggers a transition to another room when interacted with.
/*****************************************************************************/

class door : public collideable {

    float destinationX, destinationY;
    string destinationRoom;

    public:

    explicit door(  float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                    float newSizeFactor, string newDestinationRoom, float newDestX, float newDestY);

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision();

    bool stopColliding();

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};

/*****************************************************************************/
//SavePoint
//Saves the game when interacted with.
/*****************************************************************************/

class savePoint : public collideable {

    bool savedGame = false;
    entityList myParticles;

    public:

    explicit savePoint(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor);

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision();

    bool stopColliding();

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};

/*****************************************************************************/
//forceField
//Attracts or repels physical entities within its influende
/*****************************************************************************/

class forceField : public collideable {

    bool isOn;
    float power, range;
    entityList myParticles;
    collision nextCollision;
    int tickCount;

    public:

    explicit forceField(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, float newPower, float newRange);

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision();

    bool stopColliding();

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};

/*****************************************************************************/
//endingGate
//an imposing gate that spawns particles that float slowly upwards.
/*****************************************************************************/

class endingGate : public collideable {

    entityList myParticles;
    int width, height;
    vector<string> toPrint;

    public:

    explicit endingGate(float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                        float newSizeFactor, int newWidth, int newHeight);

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision();

    bool stopColliding();

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};

/******************************************************************************/
//physicalParticle
//A particle to which physics applies.
/******************************************************************************/

class physicalParticle : public particle, public lightPhysicalEntity, public collideable {

    bool shouldDelete = false;
    bool dynamicChar;

    public:

    physicalParticle(   float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                        float newSizeFactor, float newXSpeed, float newYSpeed, char c, int newLifetime,
                        float newElasticity);

    bool doesCollide(float otherX, float otherY, int otherType);

    collision getCollision();

    bool stopColliding();

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);
};


/******************************************************************************/
//rain
//Constantly spawns particles above the top of the screen
/******************************************************************************/

class rain : public entity {

    entityList raindrops;
    float dropsPerTick, dropBuffer = 0;
    bool firstTick;

    public:

    rain(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, float newDropsPerTick);

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);

};

/******************************************************************************/
//Explosion()
//Spawn particles moving outwards in a circle
/******************************************************************************/

void explosion(collider& col, entityList& entities, int count, float x, float y, uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                float newSizeFactor, float speed, char c, int lifespan, float elasticity);

/******************************************************************************/
//PlayerEntity
//what it sounds like, I guess.
/******************************************************************************/

class playerEntity : public realPhysicalEntity, virtual public collideable {

    entityList localEntities;
    int lastMovedX, lastMovedY;
    float bulletSpeed = 1;
    float bulletDamage = 1;

    public:

    string nextRoom;
    bool shouldChangeRooms;

    int won = 0;

    //Constructor

    explicit playerEntity(  float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, string newNextRoom);

    //Special accessors because playerEntity must read data from save file as well

    void setColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A);

    void setSizeFactor(float newSizeFactor);

    float getSizeFactor();

    //Collision functions

    bool doesCollide(float otherX, float otherY, int type);

    collision getCollision();

    bool stopColliding();

    //Tick functions

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);

};

/*****************************************************************************/
//Bullet
//Also self-explanatory
/*****************************************************************************/

class bullet : public collideable, public particle {

    int damage;
    float xMomentum, yMomentum;
    float width = 0.8;
    bool hit = false;
    bool exploded = false;
    entityList collisionParticles;

    public:

    explicit bullet(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, float newXMomentum, float newYMomentum, int newDamage);

    bool doesCollide(float otherX, float otherY, int type);

    collision getCollision();

    bool stopColliding();

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);

};

#endif //GAME_CLASSES_HPP
