#ifndef GAME_CLASSES_HPP
#define GAME_CLASSES_HPP

using namespace std;

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

    int channel;
    bool isOn;
    float power, range;
    entityList myParticles;
    collision nextCollision;
    int tickCount;

    public:

    explicit forceField(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newChannel, float newPower, float newRange);

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

    public:

    bool dynamicChar;

    physicalParticle(   float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                        float newSizeFactor, float newXSpeed, float newYSpeed, int c, int newLifetime,
                        float newElasticity, float newMaxSpeed = 100, float newGravity = GRAVITY, float newFriction = FRICTION);

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
    float dropsPerTick, dropBuffer = 0, xMomentum;
    bool firstTick;
    bool isSnow;

    public:

    rain(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, float newDropsPerTick, float newXMomentum, bool isSnow);

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
//DamageIndicator()
//Spawns a little number that floats up when damage is taken/dealt
/******************************************************************************/

void damageIndicator(entityList& entities, int damage, float x, float y, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor);

/******************************************************************************/
//PlayerEntity
//what it sounds like, I guess.
/******************************************************************************/

class playerEntity : public realPhysicalEntity, virtual public collideable {

    int health, maxHealth, hurtTimer = 0;

    bool gunUnlocked[16] = {false};
    int gunAmmos[16] {0};
    int gunMaxAmmos[16] = {0};
    int gunCoolDowns[16] = {0};
    int gunDisplayChars[16] = {'E'};
    Color gunColors[16] = {{255, 0, 255, 255}};
    Color gunColorsFaded[16] = {{127, 0, 127, 127}};

    int gunSelect = 0;

    //Used in bit manipulation

    int ops[16][4] = {{4}, {0}};
    int args[16][4] = {{0}, {0}};
    int opCount = 1;
    bitset<8> channels[10];

    bool pickUpsCollected[512] = { false };

    entityList localEntities;
    int lastMovedX, lastMovedY;
    Vector2 positionOnScreen = {0, 0};

    public:

    string nextRoom;
    bool shouldChangeRooms;

    int won = 0;

    //Constructor + save and load functions

    explicit playerEntity(  float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, string newNextRoom);

    bool save(string fileName);

    bool load(string fileName);
    
    //Access whether pickup is collected. Saved here so that it can go in the same savefile.
    
    bool isCollected(int pickUpID);

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

    //Draw the HUD

    void drawHUD(Font displayFont);

    //Draw a GUI inventory/byte editor screen

    void drawTabScreen(Font displayFont);

};

/*****************************************************************************/
//Bullet
//Also self-explanatory
/*****************************************************************************/

class bullet : public physicalParticle {

    int damage;
    bool hit = false;
    bool exploded = false;
    entityList collisionParticles;
    float width = 0.8;

    public:

    bullet(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor,
                   float newXMomentum, float newYMomentum, int c, int newLifeTime, float newElasticity,
                   float newMaxSpeed, float newGravity, float newFriction, int newDamage);

    bool doesCollide(float otherX, float otherY, int type);

    collision getCollision();

    bool stopColliding();

    void tickSet(collider& col);

    void tickGet(collider& col);

    bool finalize();

    void print(float cameraX, float cameraY, Font displayFont);

};

#endif //GAME_CLASSES_HPP

/*****************************************************************************/
//Gun pickup
//Unlocks a gun
/*****************************************************************************/

class gunPickUp : public pickUp {

    int gunID;

    public:

    explicit gunPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID);

    collision getCollision();

};

/*****************************************************************************/
//Ammo pickup
//Adds ammo to a gun
/*****************************************************************************/

class ammoPickUp : public pickUp {

    int gunID;
    int ammoCount;

    public:

    explicit ammoPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID, int newAmmoCount);

    collision getCollision();

};


/*****************************************************************************/
//healthPickUp
//Adds health back
/*****************************************************************************/

class healthPickUp : public pickUp {

    int healthCount;

    public:

    explicit healthPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID);

    collision getCollision();

};


/*****************************************************************************/
//max health pickup
//Increases max health
/*****************************************************************************/

class maxHealthPickUp : public pickUp {

    int healthCount;

    public:

    explicit maxHealthPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifetime, int newID, bool newTouch, int newGunID);

    collision getCollision();

};


/*****************************************************************************/
//Op pickup
//Gives the player another bitwise op to play with
/*****************************************************************************/

class opPickUp : public pickUp {

    string message;

    public:

    explicit opPickUp(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor, int newDisplayChar, int newLifeTime, int newID, bool newTouch, string newMessage);

    collision getCollision();

};
