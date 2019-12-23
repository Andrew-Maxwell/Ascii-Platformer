/* IDEAS/TODO:
Fix EASCII rendering (Will probably have to create an ASCII texture... On hold for now.)

*/

#include <thread>
#include <math.h>
#include <stdio.h>
#include <raylib.h>
#include <chrono>
#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <vector>
#include <sstream>
#include <string.h>

#define SCREENWIDTH 1280
#define SCREENHEIGHT 720
#define FONTSIZE 16
#define SCREENROWS SCREENHEIGHT / FONTSIZE
#define SCREENCOLS SCREENWIDTH / FONTSIZE

#define CAMERALAGX SCREENCOLS * 3 / 16
#define CAMERALAGY SCREENROWS * 3 / 16

#define PLAYERSPEED 0.15
#define JUMPSPEED 0.5
#define JPMAX 6
#define PARTICLELIFESPAN 100
#define COLLIDERTILESIZE 10
#define GRAVITY 0.02
#define FRICTION 0.2

Font FONT;

using namespace std;

//Forward declarations, not sure why this fixes things but it does.

class playerEntity;
class collider;


/******************************************************************************/
//Collision
//A sort of message between entities
/******************************************************************************/

struct collision {
    int type;
    int damage;
    float xVal;
    float yVal;
    string message;

    collision(int newType = 0, int newDamage = 0, float newXVal = 0, float newYVal = 0, string newMessage = "") :
        type(newType),
        damage(newDamage),
        xVal(newXVal),
        yVal(newYVal),
        message(newMessage) {}
};

/******************************************************************************/
//Virtual entity parent class.
/******************************************************************************/

class entity {

    protected:

    Color tint;
    float sizeFactor = 1;

    public:

    int type;
    int id;
    float x;
    float y;

    //Constructor and destructor

    entity(float newx, float newy, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor) {
        x = newx;
        y = newy;
        tint = {R, G, B, A};
        sizeFactor = newSizeFactor;
    }

    virtual ~entity() = default;

    //Tick functions
    //Move entities. Update collider

    virtual void tickSet(collider& col) = 0;

    //Update entities based on new collider values

    virtual void tickGet(collider& col) = 0;

    //perform additional cleanup. If this function returns true, then the entity is deleted by the entityList.

    virtual bool finalize() = 0;

    //Display the entity.

    virtual void print(float cameraX, float cameraY) = 0;
};

/******************************************************************************/
//Collideable
//An entity that can interact directly with other entities via the Collider.
/******************************************************************************/

class collideable : virtual public entity {

    protected:

    list<collision> collisions;

    public:

    //Collision function
    //If entity A "initiates" a collision with entity B, then the following functions are called during tickSet:

    //B's doesCollide() is called with A's data to check if A is close enough to collide

    virtual bool doesCollide(float otherX, float otherY, int otherType) = 0;

    //B's getCollision is used to get a collision object containing how A should respond to colliding with B

    virtual collision getCollision() = 0;

    //This function is called to add the collision object from B to A's collision list. Then, A responds during tickGet.

    void addCollision(collision newCollision) {
        collisions.push_back(newCollision);
    }

    //Return true when the collideable should be removed from collision list.

    virtual bool stopColliding() = 0;
};

/******************************************************************************/
//A list for containing entities. Every tick, every entity in the list has its four functions called
//once. Also, entities can contain entityLists (e.g. an entity containing the bullets it's fired) as
//long as the four functions are called by that entity's four functions.
/******************************************************************************/

class entityList {

    list<entity*> entities;

    public:

    ~entityList() {
        while (entities.begin() != entities.end()) {
            if ((*entities.begin()) -> type != 1) { //Don't delete the player!
                delete (*entities.begin());
            }
            entities.erase(entities.begin());
        }
    }

    void clear() {
        while (entities.begin() != entities.end()) {
            delete (*entities.begin());
            entities.erase(entities.begin());
        }
    }

    void tickSet(collider& col) {
        list<entity*>::iterator e = entities.begin();
        while (e != entities.end()) {
            (*e) -> tickSet(col);
            e++;
        }
    }

    void tickGet(collider& col) {
        list<entity*>::iterator e = entities.begin();
        while (e != entities.end()) {
            (*e) -> tickGet(col);
            e++;
        }
    }

    void finalize() {
        list<entity*>::iterator e = entities.begin();
        while (e != entities.end()) {
            if ((*e) -> finalize()) {
                delete *e;
                e = entities.erase(e);
            }
            else {
                e++;
            }
        }
    }

    void print(float cameraX, float cameraY) {
        list<entity*>::iterator e = entities.begin();
        while (e != entities.end()) {
            (*e) -> print(cameraX, cameraY);
            e++;
        }
    }

    void addEntity(entity* toAdd) {
        entities.push_back(toAdd);
    }
};

/******************************************************************************/
//layer
//Static multi-character entities read in from file.
//Designed to be the visual part of the level.
/******************************************************************************/

class layer : virtual public entity {

    protected:

    vector<string> canvas;
    string fileName;

    public:

    explicit layer( float newx, float newy, uint8_t R, uint8_t G,
                    uint8_t B, uint8_t A, float newSizeFactor,
                    string newFileName) :
                    entity(newx, newy, R, G, B, A, newSizeFactor) {
        fileName = newFileName;
        readLayer();
    }

    void readLayer() {
        canvas.clear();
        ifstream worldFile;
        worldFile.open(fileName);
        if (!worldFile) {
            cerr << "Error opening layer file.";
            exit(EXIT_FAILURE);
        }

        string line;
        while (getline(worldFile, line)) {
            canvas.push_back(line);
        }
        cout << endl;
        worldFile.close();
    }

    int getRows() {
        return canvas.size();
    }

    int getCols() {
        return canvas[0].length();
    }

    void tickSet(collider& col) {}

    void tickGet(collider& col) {}

    bool finalize() override {
        return false;
    }

    void print(float cameraX, float cameraY) {
        for (int i = max((int)(cameraY - SCREENROWS / sizeFactor / 2), 0); i < min((int)(cameraY + SCREENROWS / sizeFactor / 2) + 1, (int)canvas.size()); i++) {
            DrawTextEx(FONT, canvas[i].c_str(), (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX - x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY - y + i) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 0, tint);
        }
    }
};

/******************************************************************************/
/*Collidor*/
/******************************************************************************/

class collider : public layer {

    vector<collideable*> collideables;

    public:

    //Constructor

    collider(   float newX, float newY, string fileName = "collider.txt") :
                layer(newX, newY, 255, 255, 255, 255, 1, fileName),
                entity(newX, newY, 255, 255, 255, 255, 1) {}

    //Accessors

    void addCollideable(collideable* newCollideable) {
        collideables.push_back(newCollideable);
    }

    bool isSolid(int row, int col) {
        if (row >= 0 && row < canvas.size() && col >= 0 && col < canvas[row].size()) {
            return canvas[row][col] == 's';
        }
        return false;
    }

    bool isLiquid(int row, int col) {
        return canvas[row][col] == 'w';
    }

    //tickSet, tickGet, and finalize are each called here AFTER they have been called on all other entities.

    void tickSet(collider& col) {

        //You can press R at any time to reload the layer and from file.

        if (IsKeyPressed(KEY_R)) {
            layer::readLayer();
        }

        //Check for collisions between any two pairs of collideables.
        //Note that collisions can be asymmetric: A can collide with B but not B with A.
        //May change this depending on usage later.

        for (int i = 0; i < collideables.size(); i++) {
            for (int j = i + 1; j < collideables.size(); j++) {
                if (collideables[i] -> doesCollide(collideables[j] -> x, collideables[j] -> y, collideables[j] -> type)) {
                    collideables[j] -> addCollision(collideables[i] -> getCollision());
                }
                if (collideables[j] -> doesCollide(collideables[i] -> x, collideables[i] -> y, collideables[i] -> type)) {
                    collideables[i] -> addCollision(collideables[j] -> getCollision());
                }
            }
        }
    }

    void tickGet(collider& col) {
        //Iterate through and remove collideables for which stopColliding is true
        //called here because entities may be deleted in the finalize() step

        vector<collideable*>::iterator c = collideables.begin();
        while (c != collideables.end()) {
            if ((*c) -> stopColliding()) {
                c = collideables.erase(c);
            }
            else {
                c++;
            }
        }
    }

    bool finalize() {
    }
};

/******************************************************************************/
//Particle
//Entity represented by any char that moves in a predefined direction until its lifetime runs out.
//If the character passed to constructor is 0, then a character is chosen based on direction.
/******************************************************************************/

class particle : virtual public entity {

    protected:

    float xSpeed, ySpeed;
    int lifetime;
    char toPrint[2];

    void setDirection() {
        if (xSpeed == 0) {
            toPrint[0] = '|';
        }
        else {
            if (ySpeed / xSpeed < -1.09) {
                toPrint[0] = '|';
            }
            else if (ySpeed / xSpeed < -0.383) {
                toPrint[0] = '/';
            }
            else if (ySpeed / xSpeed < 0.383) {
                toPrint[0] = '-';
            }
            else if (ySpeed / xSpeed < 1.09) {
                toPrint[0] = '\\';
            }
            else {
                toPrint[0] = '|';
            }
        }
    }

    public:

    explicit particle(  float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                        float newSizeFactor, float newXSpeed, float newYSpeed, char c, int newLifetime) :
                        entity(newX, newY, R, G, B, A, newSizeFactor) {
        xSpeed = newXSpeed;
        ySpeed = newYSpeed;
        lifetime = newLifetime;
        if (c == 0) {
            setDirection();
        }
        else {
            toPrint[0] = c;
        }
        toPrint[1] = '\0';
    }

    void tickSet(collider& col) override {
        x += xSpeed;
        y += ySpeed;
    }

    void tickGet(collider& col) {}

    bool finalize() {
        if (lifetime-- == 0) {
            return true;
        }
        else {
            return false;
        }
    }

    void print(float cameraX, float cameraY) override {
        DrawTextEx(FONT, toPrint, (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
    }
};


/*****************************************************************************/
//Door
//Triggers a transition to another room when interacted with.
/*****************************************************************************/

class door : public collideable {

    float destinationX, destinationY;
    string destinationRoom;

    public:

    explicit door(  float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                    float newSizeFactor, string newDestinationRoom, float newDestX, float newDestY) :
        entity(newX, newY, R, G, B, A, newSizeFactor),
        destinationRoom(newDestinationRoom),
        destinationX(newDestX),
        destinationY(newDestY) {
        type = 3;
    }

    bool doesCollide(float otherX, float otherY, int otherType) {
        return (IsKeyPressed(KEY_DOWN) && otherX > x - 1 && otherX < x + 1 && otherY > y - 1 && otherY < y + 1 && otherType == 1);
    }

    collision getCollision() {
        return collision(type, 0, destinationX, destinationY, destinationRoom);
    }

    bool stopColliding() {
        return false;
    }

    void tickSet(collider& col) {}

    void tickGet(collider& col) {}

    bool finalize() {return false;}

    void print(float cameraX, float cameraY) {
        DrawTextEx(FONT, "D", (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
    }
};

/*****************************************************************************/
//SavePoint
//Saves the game when interacted with.
/*****************************************************************************/

//Forward declaration for particle effects

void explosion( entityList& entities, int count, float x, float y, uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                float newSizeFactor, float speed, char c, int lifespan, float elasticity);

class savePoint : public collideable {

    bool isSaved = false;
    entityList myParticles;

    public:

    explicit savePoint(float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor) : entity(newX, newY, R, G, B, A, newSizeFactor) {type = 4;}

    bool doesCollide(float otherX, float otherY, int otherType) {
        isSaved = (IsKeyPressed(KEY_DOWN) && otherX > x - 1 && otherX < x + 1 && otherY > y - 1 && otherY < y + 1 && otherType == 1);
        return isSaved;
    }

    collision getCollision() {
        return collision(4);
    }

    bool stopColliding() {
        return false;
    }

    void tickSet(collider& col) {
        if (isSaved) {
            explosion(myParticles, 16, x, y, tint.r, tint.g, tint.b, tint.a, sizeFactor, 0.3, '*', 100, 0.5);
        }
        myParticles.tickSet(col);
    }

    void tickGet(collider& col) {
        myParticles.tickGet(col);
    }

    bool finalize() {
        myParticles.finalize();
        return false;
    }

    void print(float cameraX, float cameraY) {
        DrawTextEx(FONT, "S", (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
        myParticles.print(cameraX, cameraY);
    }
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
                        float newSizeFactor, int newWidth = 0, int newHeight = 0) :
                        entity(newX, newY, R, G, B, A, newSizeFactor) {
        toPrint = {"O----O", "|    |", "|    |", "|    |", "O----O"};
        width = 5;
        height = 5;
        type = 2;
    }

    bool doesCollide(float otherX, float otherY, int otherType) {
        return (otherX > x && otherX < x + width && otherY > y && otherY < y + height && otherType == 1);
    }

    collision getCollision() {
        return collision(type, 0, 0, 0, "");
    }

    bool stopColliding() {
        return false;
    }

    void tickSet(collider& col) {
        if (GetRandomValue(0, 10) == 0) {
            myParticles.addEntity(new particle( x + GetRandomValue(0, 10 * width) / 10.0, y + height - 1.5, tint.r, tint.g,
                                                tint.b, tint.a, sizeFactor, 0, -0.01, '*', (height - 1.5) * 100));
        }
        myParticles.tickSet(col);
    }

    void tickGet(collider& col) {
        myParticles.tickGet(col);
    }

    bool finalize() {
        myParticles.finalize();
        return false;
    }

    void print(float cameraX, float cameraY) {
        for (int i = 0; i < height; i++) {
            DrawTextEx(FONT, toPrint[i].c_str(), (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y + i) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 0, tint);
        }
        myParticles.print(cameraX, cameraY);
    }
};

/******************************************************************************/
//lightPhysicalEntity
//An entity to which physics (gravity and not travelling through solid objects)
//applies -- loosely.
/******************************************************************************/

class lightPhysicalEntity : virtual public entity {

    protected:

    float elasticity;
    float xMomentum, yMomentum;
    int xSign = 1, ySign = 1;

    public:

    explicit lightPhysicalEntity( float newx, float newy, uint8_t R, uint8_t G, uint8_t B,
                                  uint8_t A, float newSizeFactor, float elasticity, float newXMomentum,
                                  float newYMomentum) :
                                  entity(newx, newy, R, G, B, A, newSizeFactor) {
        xMomentum = newXMomentum;
        yMomentum = newYMomentum;
    }

    void tickSet(collider& col) {
        yMomentum += GRAVITY;

        if (col.isSolid((int)y, (int)(x + xMomentum) + (xMomentum > 0))) {
            x = floor(x + xMomentum) + (xMomentum < 0);
            xMomentum *= (-1 * elasticity);
        }
        else {
            x += xMomentum;
        }

        if (col.isSolid((int)(y + yMomentum) + (yMomentum > 0), (int)x)) {
            y = floor(y + yMomentum) + (yMomentum < 0);
            yMomentum *= (-1 * elasticity);
            xMomentum *= FRICTION;
        }
        else {
            y += yMomentum;
        }
    }

    void tickGet(collider& col) {}

    bool finalize() {return false;}

    void print() {}
};

/******************************************************************************/
//realPhysicalEntity
//An entity to which physics (gravity and not travelling through solid objects)
//applies, more rigorously.
/******************************************************************************/

class realPhysicalEntity : virtual public entity {

    protected:

    float elasticity;
    float xMomentum, yMomentum;
    int xSign = 1, ySign = 1;
    float width = 0.8;

    public:

    explicit realPhysicalEntity(float newx, float newy,  uint8_t R, uint8_t G, uint8_t B,
                                uint8_t A, float newSizeFactor, float elasticity, float newXMomentum,
                                float newYMomentum) :
                                entity(newx, newy, R, G, B, A, newSizeFactor) {
        xMomentum = newXMomentum;
        yMomentum = newYMomentum;
    }

    void tickSet(collider& col) {
        yMomentum += GRAVITY;

        float xDist = xMomentum / (abs(xMomentum) + 1);
        for (int i = 0; i < abs(xMomentum) + 1; i++) {
            if (col.isSolid((int)y, (int)(x + xDist) + (xDist > 0))) {// || col.isSolid((int)y + 0.5, (int)(x + xDist) + (xDist > 0))) {
                x = floor(x + xDist) + (xDist < 0);
                xMomentum *= (-1 * elasticity);
                break;
            }
            else {
                x += xDist;
            }
        }

        float yDist = yMomentum / (abs(yMomentum) + 1);
        for (int i = 0; i < abs(yMomentum) + 1; i++) {
            if (col.isSolid((int)(y + yDist) + (yDist > 0), (int)(x + 0.5 - width / 2)) || col.isSolid((int)(y + yDist) + (yDist > 0), (int)(x + 0.5 + width / 2))) {
                y = floor(y + yDist) + (yDist < 0);
                yMomentum *= (-1 * elasticity);
                xMomentum *= FRICTION;
                break;
            }
            else {
                y += yDist;
            }
        }
    }

    void tickGet(collider& col) {}

    bool finalize() {return false;}

    void print() {}
};

/******************************************************************************/
//physicalParticle
//A particle to which physics applies.
/******************************************************************************/

class physicalParticle : public particle, public lightPhysicalEntity {

    bool shouldDelete = false;
    bool dynamicChar;

    public:

    physicalParticle(   float newX, float newY,  uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                        float newSizeFactor, float newXSpeed, float newYSpeed, char c, int newLifetime,
                        float newElasticity) :
                        entity(newX, newY, R, G, B, A, newSizeFactor),
                        particle(newX, newY, R, G, B, A, newSizeFactor, 0, 0, c, newLifetime),
                        lightPhysicalEntity(newX, newY, R, G, B, A, newSizeFactor, newElasticity, newXSpeed, newYSpeed) {

        dynamicChar = (c == 0);
        elasticity = newElasticity;
        lifetime = newLifetime;
    }

    void tickSet(collider& col) {
        lightPhysicalEntity::tickSet(col);
        if (col.isSolid(y, x)) {
            shouldDelete = true;
        }
    }

    void tickGet(collider& col) {
        lightPhysicalEntity::tickGet(col);
    }

    bool finalize() {
        return (xMomentum < 0.01 && xMomentum > -0.01 && yMomentum < 0.01 && yMomentum > -0.01) || shouldDelete || particle::finalize();
    }

    void print(float cameraX, float cameraY) override {
        if (dynamicChar) {
            if (abs(xMomentum) + abs(yMomentum) > 0.1) {
                xSpeed = xMomentum;
                ySpeed = yMomentum;
                particle::setDirection();
            }
            else {
                toPrint[0] = '.';
            }
        }
        DrawTextEx(FONT, toPrint, (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
    }
};

/******************************************************************************/
//Explosion()
//Spawn particles moving outwards in a circle
/******************************************************************************/

void explosion( entityList& entities, int count, float x, float y, uint8_t R, uint8_t G, uint8_t B, uint8_t A,
                float newSizeFactor, float speed, char c, int lifespan, float elasticity) {
    for (float angle = 0; angle < 2 * M_PI; angle += (2 * M_PI / count)) {
        entities.addEntity(new physicalParticle(x, y, R, G, B, A, newSizeFactor, cos(angle) * speed, sin(angle) * speed, c, lifespan, elasticity));
    }
}

/******************************************************************************/
//PlayerEntity
//what it sounds like, I guess.
/******************************************************************************/

class playerEntity : public realPhysicalEntity, virtual public collideable {

    entityList localEntities;
    int lastMovedX, lastMovedY;
    float bulletSpeed = 1;

    public:

    string nextRoom;
    bool shouldChangeRooms;

    int won = 0;

    //Constructor

    explicit playerEntity(  float newX, float newY, uint8_t R, uint8_t G, uint8_t B, uint8_t A, float newSizeFactor) :
                            realPhysicalEntity(newX, newY, R, G, B, A, newSizeFactor, 1.0, 0.0, 0.0),
                            entity(newX, newY, R, G, B, A, newSizeFactor)
    {
        yMomentum = 0;
        elasticity = 0;
        type = 1;
    }

    //Special accessors

    float getSizeFactor() {
        return sizeFactor;
    }

    void setSizeFactor(float sf) {
        sizeFactor = sf;
    }

    //Collision functions

    bool doesCollide(float otherX, float otherY, int type) {
        return (otherX >= x && otherX <= x + 1 && otherY >= y && otherY <= y + 1);
    }

    collision getCollision() {
        collision c;
        return c;
    }

    bool stopColliding() {
        return false;
    }

    //Tick functions

    void tickSet(collider& col) override {
        lastMovedY = 0;
        xMomentum *= 0.5;
/*
        if (xMomentum > 1) {
            xMomentum = 1;
        }
        if (yMomentum > 1) {
            yMomentum = 1;
        }
*/

        //Reload room

        if (IsKeyPressed(KEY_R)) {
            shouldChangeRooms = true;
        }

        //Movement

        if (IsKeyDown(KEY_RIGHT)) {
            xMomentum += PLAYERSPEED;
            lastMovedX = 1;
        }
        if (IsKeyDown(KEY_LEFT)) {
            xMomentum -= PLAYERSPEED;
            lastMovedX = -1;
        }
        if (IsKeyDown(KEY_UP)) {
            lastMovedY += -1;
        }
        if (IsKeyDown(KEY_DOWN)) {
            lastMovedY += 1;
        }

        //Boollets

        if (IsKeyPressed(KEY_X)) {
            particle* p;
            if (lastMovedY) {
                p = new particle(x, y, tint.r, tint.g, tint.b, tint.a, sizeFactor, 0, lastMovedY * bulletSpeed, 0, 20);
            }
            else {
                p = new particle(x, y, tint.r, tint.g, tint.b, tint.a, sizeFactor, lastMovedX * bulletSpeed, 0, 0, 20);
            }
            localEntities.addEntity(p);
        }

        //Splosions

        if (IsKeyPressed(KEY_C)) {
            explosion(localEntities, 50, x, y, tint.r, tint.g, tint.b, tint.a, sizeFactor, 1, 0, PARTICLELIFESPAN, 0.2);
        }

        //Jomping

        if (IsKeyPressed(KEY_Z) && (col.isSolid(y + 1, x + (1 - width) / 2) || col.isSolid(y + 1, x + (1 + width) / 2)))  {
            yMomentum -= JUMPSPEED;
        }
        else if (IsKeyUp(KEY_Z)) {
            yMomentum = max(0.0f, yMomentum);
        }

        //Death

        if (y > col.getRows() + 25) {
            won = -1;
        }

        //Apply physics

        realPhysicalEntity::tickSet(col);

        //update all child entities

        localEntities.tickSet(col);
    }

    void tickGet(collider& col) {
        localEntities.tickGet(col);
        list<collision>::iterator colIter = collisions.begin();
        while (colIter != collisions.end()) {
            switch(colIter -> type) {
                case 2:         //Win portal
                    won = 1;
                    break;
                case 3:         //Door
                    x = colIter -> xVal;
                    y = colIter -> yVal;
                    nextRoom = colIter -> message;
                    shouldChangeRooms = true;
                    localEntities.clear();
                    break;
                case 4:         //Savepoint
                    ofstream saveOut;
                    saveOut.open("save.txt");
                    saveOut << nextRoom << " " << x << " " << y;
                    saveOut.close();
            }
            colIter = collisions.erase(colIter);
        }
    }

    bool finalize() {
        localEntities.finalize();
        return false;
    }

    void print(float cameraX, float cameraY) {
        DrawTextEx(FONT, "@", (Vector2){ (SCREENCOLS / sizeFactor / 2 - cameraX + x) * FONTSIZE * sizeFactor, (SCREENROWS / sizeFactor / 2 - cameraY + y) * FONTSIZE * sizeFactor }, FONTSIZE * sizeFactor, 1, tint);
        localEntities.print(cameraX, cameraY);
    }

};

/******************************************************************************/
//Read in a list of entities from a file
/******************************************************************************/

void readEntities(entityList& el, collider*& col, Color& background, float& playerSizeFactor, string fileName) {
    ifstream entityFile;
    entityFile.open(fileName);
    if (!entityFile) {
        cerr << "Error opening entity file. Attempting to open" << fileName;
        exit(EXIT_FAILURE);
    }
    char c;
    float x, y, sizeFactor;
    int R, G, B, A;
    string colliderFileName;
    entityFile >> colliderFileName >> R >> G >> B >> A >> playerSizeFactor;
    background = {R, G, B, A};
    col = new collider(0.0, 0.0, colliderFileName);
    while (entityFile >> c >> x >> y >> R >> G >> B >> A >> sizeFactor) {
        cout << "Read in a " << c << endl;
        switch(c) {
            case 'L': {
                string fileName;
                entityFile >> fileName;
                layer * L = new layer(x, y, R, G, B, A, sizeFactor, fileName);
                el.addEntity(L);
                break;
            }
            case 'E': {
                int width, height;
                entityFile >> width >> height;
                endingGate * E = new endingGate(x, y, R, G, B, A, sizeFactor, width, height);
                el.addEntity(E);
                col -> addCollideable(E);
                break;
            }
            case 'D': {
                string nextRoom;
                float destinationX, destinationY;
                entityFile >> nextRoom >> destinationX >> destinationY;
                door * D = new door(x, y, R, G, B, A, sizeFactor, nextRoom, destinationX, destinationY);
                el.addEntity(D);
                col -> addCollideable(D);
                break;
            }
            case 'S': {
                savePoint * S = new savePoint(x, y, R, G, B, A, sizeFactor);
                el.addEntity(S);
                col -> addCollideable(S);
                break;
            }
            default: {
                cerr << "Error: Bad entity when reading entity list.";
                break;
            }
        }
    }
    entityFile.close();
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


int main() {

//Initialize raylib

    InitWindow(SCREENWIDTH, SCREENHEIGHT, "ASCII Platformer");
    SetTargetFPS(60);
    int fontChars[256] = {0x0000,0x263A,0x263B,0x2665,0x2666,0x2663,0x2660,0x2022,0x25D8,0x25CB,0x25D9,0x2642,0x2640,0x266A,0x266B,0x263C,0x25BA,0x25C4,0x2195,0x203C,0x00B6,0x00A7,0x25AC,0x21A8,0x2191,0x2193,0x2192,0x2190,0x221F,0x2194,0x25B2,0x25BC,0x0020,0x0021,0x0022,0x0023,0x0024,0x0025,0x0026,0x0027,0x0028,0x0029,0x002A,0x002B,0x002C,0x002D,0x002E,0x002F,0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x0037,0x0038,0x0039,0x003A,0x003B,0x003C,0x003D,0x003E,0x003F,0x0040,0x0041,0x0042,0x0043,0x0044,0x0045,0x0046,0x0047,0x0048,0x0049,0x004A,0x004B,0x004C,0x004D,0x004E,0x004F,0x0050,0x0051,0x0052,0x0053,0x0054,0x0055,0x0056,0x0057,0x0058,0x0059,0x005A,0x005B,0x005C,0x005D,0x005E,0x005F,0x0060,0x0061,0x0062,0x0063,0x0064,0x0065,0x0066,0x0067,0x0068,0x0069,0x006A,0x006B,0x006C,0x006D,0x006E,0x006F,0x0070,0x0071,0x0072,0x0073,0x0074,0x0075,0x0076,0x0077,0x0078,0x0079,0x007A,0x007B,0x007C,0x007D,0x007E,0x2302,0x00C7,0x00FC,0x00E9,0x00E2,0x00E4,0x00E0,0x00E5,0x00E7,0x00EA,0x00EB,0x00E8,0x00EF,0x00EE,0x00EC,0x00C4,0x00C5,0x00C9,0x00E6,0x00C6,0x00F4,0x00F6,0x00F2,0x00FB,0x00F9,0x00FF,0x00D6,0x00DC,0x00A2,0x00A3,0x00A5,0x20A7,0x0192,0x00E1,0x00ED,0x00F3,0x00FA,0x00F1,0x00D1,0x00AA,0x00BA,0x00BF,0x2310,0x00AC,0x00BD,0x00BC,0x00A1,0x00AB,0x00BB,0x2591,0x2592,0x2593,0x2502,0x2524,0x2561,0x2562,0x2556,0x2555,0x2563,0x2551,0x2557,0x255D,0x255C,0x255B,0x2510,0x2514,0x2534,0x252C,0x251C,0x2500,0x253C,0x255E,0x255F,0x255A,0x2554,0x2569,0x2566,0x2560,0x2550,0x256C,0x2567,0x2568,0x2564,0x2565,0x2559,0x2558,0x2552,0x2553,0x256B,0x256A,0x2518,0x250C,0x2588,0x2584,0x258C,0x2590,0x2580,0x03B1,0x00DF,0x0393,0x03C0,0x03A3,0x03C3,0x00B5,0x03C4,0x03A6,0x0398,0x03A9,0x03B4,0x221E,0x03C6,0x03B5,0x2229,0x2261,0x00B1,0x2265,0x2264,0x2320,0x2321,0x00F7,0x2248,0x00B0,0x2219,0x00B7,0x221A,0x207F,0x00B2,0x25A0,0x00A0};
    FONT = LoadFontEx("PxPlus_IBM_BIOS.ttf", 16, fontChars, 256);

//misc initializations

    chrono::steady_clock::time_point tickStart, tickEnd;
    long total = 0, tickCount = 0;

    int won = -1;
    playerEntity* player;
    bool shouldChangeRooms = false;

    while (!(WindowShouldClose() || won > 0)) {      //While we don't want to stop playing, and haven't won

        player = new playerEntity(0, 0, 50, 100, 200, 255, 1);

        //Attempt to load save file, otherwise use default starting values.
        ifstream saveFile;
        saveFile.open("save.txt");
        if (saveFile) {
            saveFile >> player -> nextRoom >> player -> x >> player -> y;
            saveFile.close();
        }
        else {
            player -> x = 39;
            player -> y = 91;
            player -> nextRoom = "room1.txt";
        }
        won = 0;

        while (!(won || WindowShouldClose())) {         //While we're still alive

            //Read in entities, including colllider

            entityList entities;
            collider* col;
            Color background;
            float newSf;
            readEntities(entities, col, background, newSf, player -> nextRoom);
            player -> setSizeFactor(newSf);
            col -> addCollideable(player);
            entities.addEntity(player);

            //Camera initializations

            float cameraX = col -> getCols() / 2;
            float cameraY = col -> getRows() / 2;
            bool moveCameraX = (col -> getCols() > SCREENCOLS * player -> getSizeFactor());
            bool moveCameraY = (col -> getRows() > SCREENROWS * player -> getSizeFactor());

            shouldChangeRooms = false;

            while (!(won || WindowShouldClose() || shouldChangeRooms)) {        //While we're in the same room
                won = player -> won;
                tickStart = chrono::steady_clock::now();

                //Update camera

                if (moveCameraX) {
                    if (player -> x > cameraX + CAMERALAGX) {
                        cameraX = player -> x - CAMERALAGX;
                    }
                    else if (player -> x < cameraX - CAMERALAGX) {
                        cameraX = player -> x + CAMERALAGX;
                    }
                }
                if (moveCameraY) {
                    if (player -> y > cameraY + CAMERALAGY) {
                        cameraY = player -> y - CAMERALAGY;
                    }
                    else if (player -> y < cameraY - CAMERALAGY) {
                        cameraY = player -> y + CAMERALAGY;
                    }
                }

                //Update entities

                entities.tickSet(*col);
                col -> tickSet(*col);
                entities.tickGet(*col);
                col -> tickGet(*col);
                entities.finalize();
                col -> finalize();

                //Display the world

                BeginDrawing();
                ClearBackground(background);
                entities.print(cameraX, cameraY);
                tickEnd = chrono::steady_clock::now();
                EndDrawing();

                //Handle room changing

                if (player -> shouldChangeRooms) {
                    shouldChangeRooms = true;
                    player -> shouldChangeRooms = false;
                    delete col;
                }

                //Performance heuristics

                int tickLength = chrono::duration_cast<chrono::microseconds>(tickEnd - tickStart).count();
                total += tickLength;
                tickCount++;
            }
        }
        delete player;
    }

//final clean up

    CloseWindow();
    cout << "Average tick length: " << total / tickCount << endl;
    if (won == -1) {
        cout << "You lose.";
    }
    else {
        cout << "You win! your score is: " << won;
    }
}

