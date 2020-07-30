#include "logic.hpp"

    logic::logic(vector<int> newOutputs) :
        entity(0, 0, WHITE, 1),
        outputs(newOutputs) {}

    unsigned int logic::type() {
        return LOGICTYPE;   //Probably won't ever be used
    }

    void logic::tickSet() {
        if (outputState) {
            for (int i = 0; i < outputs.size(); i++) {
                world -> setChannel(outputs[i], false);
            }
        }
    }

    bool logic::finalize() {
        return false;
    }

    void logic::print() {}

/*****************************************************************************/
//TimerRelay: Repeats whatever broadcast sequence it receives on a delay
/*****************************************************************************/

    trigger::trigger(vector<int> newOutputs, float newX, float newY, int newWidth, int newHeight, bool newMustInteract, bool newToggle, set<int> newTriggeredBy) :
        width(newWidth),
        height(newHeight),
        mustInteract(newMustInteract),
        toggle(newToggle),
        triggeredBy(newTriggeredBy),
        entity(newX, newY, WHITE, 1.0),
        logic(newOutputs) {}

    unsigned int trigger::type() {
        return TRIGGERTYPE;
    }

    bool trigger::doesCollide(float otherX, float otherY, int otherType, unsigned int otherID) {
        return false;
    }

    collision trigger::getCollision(float otherX, float otherY, int otherType, unsigned int otherID) {
        return collision(type(), id);
    }

    bool trigger::stopColliding() {
        return false;
    }

    void trigger::tickSet() {
        for (collision col : collisions) {
            if (col.xVal > x - 0.5 && col.xVal <= x + width - 0.5 && col.yVal > y - 0.5 && col.yVal <= y + width - 0.5) { //If touching
                if ((mustInteract && col.type == PLAYERTYPE && col.damage >= 1) || (!mustInteract && triggeredBy.count(col.type))) {
                    if (toggle) {
                        for (int output : outputs) {
                            world -> toggleChannel(output, true);
                        }
                    }
                    else {
                        for (int output : outputs) {
                            world -> setChannel(output, true); 
                        }
                    }
                }
            }
        }
        collisions.clear();
    }

    void trigger::tickGet() {}

/*****************************************************************************/
//ANDRelay: Broadcasts on all output channels if all input channels are active
/*****************************************************************************/

    ANDRelay::ANDRelay(vector<int> newOutputs, vector<int> newInputs) :
        entity(0, 0, WHITE, 1),
        inputs(newInputs),
        logic(newOutputs) {}

    void ANDRelay::tickGet() {
        outputState = true;
        for (int input : inputs) {
            if (!world -> getChannel(input)) {
                outputState = false;
                break;
            }
        }
    }

/*****************************************************************************/
//ORRelay: Broadcasts on all output channels if any input channel is active
/*****************************************************************************/

    ORRelay::ORRelay(vector<int> newOutputs, vector<int> newInputs) :
        entity(0, 0, WHITE, 1),
        inputs(newInputs),
        logic(newOutputs) {}

    void ORRelay::tickGet() {
        outputState = false;
        for (int input : inputs) {
            if (world -> getChannel(input)) {
                outputState = true;
                break;
            }
        }
    }

/*****************************************************************************/
//NOTRelay: Broadcasts on all output channels if input channel isn't active
/*****************************************************************************/

    NOTRelay::NOTRelay(vector<int> newOutputs, int newInput) :
        entity(0, 0, WHITE, 1),
        input(newInput),
        logic(newOutputs) {}

    void NOTRelay::tickGet() {
        outputState = !world -> getChannel(input);
    }

/*****************************************************************************/
//TimerRelay: Repeats whatever broadcast sequence it receives on a delay
/*****************************************************************************/

    timerRelay::timerRelay(vector<int> newOutputs, int newInput, int newDelay) :
        entity(0, 0, WHITE, 1),
        logic(newOutputs),
        input(newInput),
        delay(newDelay) {}

    void timerRelay::tickGet() {
        if (lastState != world -> getChannel(input)) {   //Record the times when the value of the channel changed
            events.push_back(tickCounter);
            lastState = world -> getChannel(input);
        }
        if (events.size() > 0 && *events.begin() == tickCounter - delay) {   //If the value of the channel changed [delay] ticks ago
            outputState = !outputState;
            events.pop_front();
        }
    }

/*****************************************************************************/
//ToggleRelay: Toggles a channel
/*****************************************************************************/

    toggleRelay::toggleRelay(vector<int> newOutputs, int newInput) :
        entity(0, 0, WHITE, 1),
        logic(newOutputs),
        input(newInput) {}

    void toggleRelay::tickSet() {
        if (outputState) {
            for (int output : outputs) {
                world -> toggleChannel(output, true);
            }
        }
    }

    void toggleRelay::tickGet() {
        outputState = world -> getChannel(input);
    }

/*****************************************************************************/
//randomSource: Broadcasts a biased random sequence
/*****************************************************************************/

    randomSource::randomSource(vector<int> newOutputs, float newMix, float newProbability) :
        entity(0, 0, WHITE, 1),
        logic(newOutputs),
        mix(newMix),
        probability(newProbability) {}

    void randomSource::tickSet() {
        if (GetRandomValue(1, 1000) < 1000 * mix) {
            outputState = (GetRandomValue(1, 1000) < 1000 * probability);
        }
        logic::tickSet();
    }

    void randomSource::tickGet() {}
