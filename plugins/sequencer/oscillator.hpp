#ifndef _OSCILLATOR_H_
#define _OSCILLATOR_H_
#include <iostream>
#include "math.h"

#define PI_2 6.28318530717959

class VelocityLFO
{
public:
    VelocityLFO(double samplerate, double frequency, double phase);
    virtual ~VelocityLFO();
    double getSample();
    void tick();
    void setFrequency(double frequency);
    void setPhase(double phase);
    double getFrequency();
	int calculate();
private:
    double frequency;
    double phase;
    double sample;
    double samplerate;
	double x1;
	double velocityCurve;

	int    curveLength;
	int    frame;

	bool curveClip;
	bool clip;

    
    
    
};

#endif
