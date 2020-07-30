#ifndef LOGIC_HPP
#define LOGIC_HPP

#include "world.hpp"
#include "effects.hpp"
#include "entity.hpp"
#include "meta.hpp"

using namespace rapidjson;

class logic : virtual public entity {

    public:

    bool outputState = false;
    vector<int> outputs;

    logic(vector<int> outputs);

    unsigned int type();

    void tickSet();

    bool finalize();

    void print();

};

//Broadcasts when entity enters trigger space

class trigger : public collideable, public logic {

    int width = 1, height = 1;
    bool mustInteract;
    bool toggle;
    bool triggered = false;
    set<int> triggeredBy;

    public:

    explicit trigger(vector<int> newOutputs, float newX, float newY, int newWidth, int newHeight, bool newMustInteract, bool newToggle, set<int> newTriggeredBy);

    unsigned int type();

    bool doesCollide(float otherX, float otherY, int otherType, unsigned int otherID);

    collision getCollision(float otherX, float otherY, int otherType, unsigned int otherID);

    bool stopColliding();

    void tickSet();

    void tickGet();

};

class ANDRelay : public logic {

    vector<int> inputs;

    public:

    ANDRelay(vector<int> newOutputs, vector<int> newInputs);

    void tickGet();

};

class ORRelay : public logic {

    vector<int> inputs;

    public:

    ORRelay(vector<int> newOutputs, vector<int> newInputs);

    void tickGet();

};

class NOTRelay : public logic {

    int input;

    public:

    NOTRelay(vector<int> newOutputs, int newInput);

    void tickGet();

};

class timerRelay : public logic {

    int input, delay;
    bool lastState = false;
    list<unsigned int> events;

    public:

    timerRelay(vector<int> newOutputs, int newInput, int newDelay);

    void tickGet();

};

class toggleRelay : public logic {

    int input;

    public:

    toggleRelay(vector<int> newOutputs, int newInput);

    void tickSet();

    void tickGet();

};

class randomSource : public logic {

    float mix, probability;

    public:

    randomSource(vector<int> newOutputs, float newMix, float newProbability);

    void tickSet();

    void tickGet();

};

#endif //LOGIC_HPP
