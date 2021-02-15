#ifndef _OSCILLATOR_H_
#define _OSCILLATOR_H_
#include <iostream>
#include "math.h"

#define PI_2 6.28318530717959

class VelocityLFO
{
public:
    VelocityLFO(double samplerate, double frequency, double phase);
    ~VelocityLFO();
    double getSample();

    void   setVelocityCurve(float value);
    void   setCurveDepth(float value);
    void   setCurveClip(bool value);
    void   setCurveLength(int value);
    float  getVelocityCurve();
    float  getCurveDepth();
    bool   getCurveClip();
    int    getCurveLength();
    void   tick();
    void   setFrequency(double frequency);
    void   setPhase(double phase);
    double getFrequency();
    int    calculate();
private:
    double frequency;
    double phase;
    double sample;
    double samplerate;
    double x1;
    double velocityCurve;
    double curveDepth; //TODO check type
    int    curveLength;
    int    frame;
    bool   curveClip;
    bool   clip;
};

#endif
