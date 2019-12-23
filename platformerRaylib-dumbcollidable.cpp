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
    float xPush;
    float yPush;
    string message;

    collision(int newType = 0, int newDamage = 0, float newXPush = 0, float newYPush = 0, string newMessage = "") :
        type(newType),
        damage(newDamage),
        xPush(newXPush),
        yPush(newYPush),
        message(newMessage) {}
};

/******************************************************************************/
//Virtual entity parent class.
/******************************************************************************/

class entity {

    public:

    int type;
    int id;
    float x;
    float y;
    Color tint;

    //Constructor and destructor

    entity(float newx, float newy) {
        x = newx;
        y = newy;
        tint = WHITE;
    }

    virtual ~entity() = default;

    //Set this entity's color

    void setColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A) {
        tint = {R, G, B, A};
    }

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

class collideable : public entity {

    public:
    list<collision> collisions;

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
}

/******************************************************************************/
//A list for containing entities. Every tick, every entity in the list has its four functions called
//once. Also, entities can contain entityLists (e.g. an entity containing the bullets it's fired) as
//long as the four functions are called by that entity's four functions.
/******************************************************************************/

class entityList {

    list<entity*> entities;
    playerEntity* player;

    public:

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

    explicit layer(float newx = 0, float newy = 0, string newFileName = "world.txt") : entity(newx, newy) {
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
        worldFile >> x >> y;
        worldFile.ignore(10000, '\n');
        string line;
        while (getline(worldFile, line)) {
            canvas.push_back(line);
        }
        worldFile.close();
    }

    int getRows() {
        return canvas.size();
    }

    int getCols() {
        return canvas[0].length();
    }

    void tickSet(collider& col) {
        if (IsKeyPressed(KEY_R)) {
            readLayer();
        }
    }

    void tickGet(collider& col) {}

    bool finalize() override {
        return false;
    }

    void print(float cameraX, float cameraY) {
        for (int i = max((int)(cameraY - SCREENROWS / 2), 0); i < min((int)(cameraY + SCREENROWS / 2) + 1, (int)canvas.size()); i++) {
            DrawTextEx(FONT, canvas[i].c_str(), (Vector2){ (SCREENCOLS / 2 - cameraX - x) * FONTSIZE, (SCREENROWS / 2 - cameraY - y + i) * FONTSIZE }, FONTSIZE, 0, tint);
        }
    }
};

/******************************************************************************/
/*Collidor*/
/******************************************************************************/

class collider : public layer {

    vector<collideable*> collideables;
    int rows;
    int cols;

    public:

    //Constructor

    collider(float newX = 0, float newY = 0, string fileName = "collider.txt") : layer(newX, newY, fileName), entity(newX, newY) {

    }

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

    public:

    float xSpeed, ySpeed;
    int lifetime;
    char* toPrint;

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

    explicit particle(float newX, float newY, float newXSpeed, float newYSpeed, char c, int newLifetime) : entity(newX, newY) {
        toPrint = new char[2];
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
        DrawTextEx(FONT, toPrint, (Vector2){ (SCREENCOLS / 2 - cameraX + x) * FONTSIZE, (SCREENROWS / 2 - cameraY + y) * FONTSIZE }, FONTSIZE, 1, tint);
    }
};

/*****************************************************************************/
//endingGate
//an imposing gate that spawns particles that float slowly upwards.
/*****************************************************************************/

class endingGate : public entity {

    entityList myParticles;
    int width, height;
    vector<string> toPrint;

    public:

    explicit endingGate(float newX, float newY, int newWidth, int newHeight) : entity(newX, newY) {
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
            particle* p = new particle(x + GetRandomValue(0, 10 * width) / 10.0, y + height - 1.5, 0, -0.01, '*', (height - 1.5) * 100);
            p -> setColor(tint.r, tint.g, tint.b, 150);
            myParticles.addEntity(p);
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
            DrawTextEx(FONT, toPrint[i].c_str(), (Vector2){ (SCREENCOLS / 2 - cameraX + x) * FONTSIZE, (SCREENROWS / 2 - cameraY + y + i) * FONTSIZE }, FONTSIZE, 0, tint);
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

    public:

    float elasticity;
    float xMomentum, yMomentum;
    int xSign = 1, ySign = 1;

    explicit lightPhysicalEntity(float newx = 0, float newy = 0, float elasticity = 0, float newXMomentum = 0, float newYMomentum = 0) : entity(newx, newy) {
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

public:

    float elasticity;
    float xMomentum, yMomentum;
    int xSign = 1, ySign = 1;
    float width = 0.8;

    explicit realPhysicalEntity(float newx = 0, float newy = 0, float elasticity = 0, float newXMomentum = 0, float newYMomentum = 0) : entity(newx, newy) {
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

    public:

    physicalParticle(float newX, float newY, float newXSpeed, float newYSpeed, char c, int newLifetime, float newElasticity) :
        entity(newX, newY),
        particle(newX, newY, 0, 0, c, newLifetime),
        lightPhysicalEntity(newX, newY, newElasticity, newXSpeed, newYSpeed) {

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
        if (abs(xMomentum) + abs(yMomentum) > 0.1) {
            xSpeed = xMomentum;
            ySpeed = yMomentum;
            particle::setDirection();
        }
        else {
            toPrint[0] = '.';
        }
        DrawTextEx(FONT, toPrint, (Vector2){ (SCREENCOLS / 2 - cameraX + x) * FONTSIZE, (SCREENROWS / 2 - cameraY + y) * FONTSIZE }, FONTSIZE, 1, tint);
    }
};

/******************************************************************************/
//Explosion()
//Spawn particles moving outwards in a circle
/******************************************************************************/

void explosion(entityList& entities, int count, float x, float y, float speed, char c, int lifespan, float elasticity) {
    for (float angle = 0; angle < 2 * M_PI; angle += (2 * M_PI / count)) {
        entities.addEntity(new physicalParticle(x, y, cos(angle) * speed, sin(angle) * speed, c, lifespan, elasticity));
    }
}

/******************************************************************************/
//PlayerEntity
//what it sounds like, I guess.
/******************************************************************************/

class playerEntity : public realPhysicalEntity {

    bool shot = false;
    bool shotz = false;
    bool jumped = false;
    entityList localEntities;
    int lastMovedX, lastMovedY;
    float bulletSpeed = 1;

    public:

    int won = 0;

    //Constructor

    explicit playerEntity(float newX, float newY) : realPhysicalEntity(newX, newY, 1.0),
                                                    entity(newX, newY)
    {
        yMomentum = 0;
        elasticity = 0;
        type = 1;
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

        if (IsKeyDown(KEY_X) && !shot) {
            particle* p;
            if (lastMovedY) {
                p = new particle(x, y, 0, lastMovedY * bulletSpeed, 0, 20);
            }
            else {
                p = new particle(x, y, lastMovedX * bulletSpeed, 0, 0, 20);
            }
            localEntities.addEntity(p);
            shot = true;
        }
        else if (IsKeyUp(KEY_X)) {
            shot = false;
        }

        //Splosions

        if (IsKeyDown(KEY_C) && !shotz) {
            explosion(localEntities, 50, x, y, 1, 0, PARTICLELIFESPAN, 0.2);
            shotz = true;
        }
        else if (IsKeyUp(KEY_C)) {
            shotz = false;
        }

        //Jomping

        if (IsKeyDown(KEY_Z) && !jumped && (col.isSolid(y + 1, x + (1 - width) / 2) || col.isSolid(y + 1, x + (1 + width) / 2)))  {
            yMomentum -= JUMPSPEED;
            jumped = true;
        }
        else if (IsKeyUp(KEY_Z)) {
            jumped = false;
            yMomentum = max(0.0f, yMomentum);
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
            switch((*colIter).type) {
                case 2:
                    won = 1;
                    break;
            }
            colIter++;
        }
    }

    bool finalize() {
        localEntities.finalize();
        return false;
    }

    void print(float cameraX, float cameraY) {
        DrawTextEx(FONT, "@", (Vector2){ (SCREENCOLS / 2 - cameraX + x) * FONTSIZE, (SCREENROWS / 2 - cameraY + y) * FONTSIZE }, FONTSIZE, 1, tint);
        localEntities.print(cameraX, cameraY);
    }

};

/******************************************************************************/
//Read in a list of entities from a file
/******************************************************************************/

void readEntities(entityList& el, playerEntity*& p, collider*& col, string fileName) {
    ifstream entityFile;
    entityFile.open(fileName);
    if (!entityFile) {
        cerr << "Error opening entity file.";
        exit(EXIT_FAILURE);
    }
    char c;
    float x, y;
    int R, G, B, A;
    string colliderFileName;
    entityFile >> colliderFileName;
    col = new collider(0.0, 0.0, colliderFileName);
    while (entityFile >> c >> x >> y >> R >> G >> B >> A) {
        switch(c) {
            case 'L': {
                string fileName;
                entityFile >> fileName;
                layer * L = new layer(x, y, fileName);
                L -> setColor(R, G, B, A);
                el.addEntity(L);
                break;
            }
            case 'E': {
                int width, height;
                entityFile >> width >> height;
                endingGate * E = new endingGate(x, y, width, height);
                E -> setColor(R, G, B, A);
                el.addEntity(E);
                col -> addCollideable(E);
                break;
            }
            case '@': {
                p = new playerEntity(x, y);
                p -> setColor(R, G, B, A);
                el.addEntity(p);
                col -> addCollideable(p);
                break;
            }
            default: {
                cerr << "Error: Bad entity when reading entity list.";
                break;
            }
        }
    }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


int main() {

//Initialize raylib

    InitWindow(SCREENWIDTH, SCREENHEIGHT, "ASCII Platformer");
    SetTargetFPS(60);
    int fontChars[256];
    for (int i = 0; i < 256; i++) {
        fontChars[i] = i;
    }
    FONT = LoadFontEx("PxPlus_IBM_BIOS.ttf", 8, fontChars, 256);

//Read in entities, including colllider

    entityList entities;
    playerEntity* player;
    collider* col;
    readEntities(entities, player, col, "entities.txt");

//misc initializations

    int won;
    chrono::steady_clock::time_point tickStart, tickEnd;
    long total = 0, tickCount = 0;
    float cameraX = col -> getCols() / 2;
    float cameraY = col -> getRows() / 2;
    bool moveCameraX = (col -> getCols() > SCREENCOLS);
    bool moveCameraY = (col -> getRows() > SCREENROWS);

//main loop

    while (!(won || WindowShouldClose())) {
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
        ClearBackground(BLACK);
        entities.print(cameraX, cameraY);
        tickEnd = chrono::steady_clock::now();
        EndDrawing();

//Performance heuristics

        int tickLength = chrono::duration_cast<chrono::microseconds>(tickEnd - tickStart).count();
        total += tickLength;
        tickCount++;
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

