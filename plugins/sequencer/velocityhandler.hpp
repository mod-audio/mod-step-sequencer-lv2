#ifndef _H_VELOCITY_HANDLER_
#define _H_VELOCITY_HANDLER_

#include "velocityLFO.hpp"

#define NUM_STEPS 8


class VelocityHandler {
public:
    enum {
        STATIC = 0,
        LFO,
        PATTERN
    };
    VelocityHandler(float sampleRate);
    ~VelocityHandler();

    void  setVelocityCurve(float value);
    void  setCurveDepth(float value);
    void  setCurveClip(bool value);
    void  setCurveLength(int value);
    void  setCurveFrequency(float value);
    float getVelocityCurve();
    float getCurveDepth();
    bool  getCurveClip();
    int   getCurveLength();

    void setSampleRate(float sampleRate);
    void setMode(int mode);
    void setNumSteps(int steps);
    void setVelocityPattern(int index, uint8_t value);
    void goToNextStep();
    uint8_t getVelocity();
    void process();

private:
    VelocityLFO *lfo;

    uint8_t velocity;
    float sampleRate;
    int step;
    int numSteps;
    int velocityMode;
    uint8_t velocityPattern[NUM_STEPS];
};

#endif // _H_VELOCITY_HANDLER_
