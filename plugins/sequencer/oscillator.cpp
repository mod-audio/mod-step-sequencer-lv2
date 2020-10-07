#include "oscillator.hpp"

VelocityLFO::VelocityLFO(double samplerate, double frequency, double phase) :
frequency(frequency),
phase(phase),
sample(0),
samplerate(samplerate),
x1(0.00000001),
velocityCurve(0),
curveLength(1),
frame(0),
curveClip(false),
clip(false)
{

}

VelocityLFO::~VelocityLFO()
{
}

double VelocityLFO::getSample()
{
    return sample;
}

void VelocityLFO::tick()
{
    calculate();
}

void VelocityLFO::setPhase(double phase)
{
    this->phase = phase;
}


void VelocityLFO::setFrequency(double frequency)
{
    this->frequency = frequency;
}

double VelocityLFO::getFrequency()
{
	return frequency;
}

int VelocityLFO::calculate()
{
    double velOscPhase;
    double warpedpos;
    double m1;
    double m2;

    double a = 1.0;
    double phaseLenght = 1.0;

    velOscPhase = ((frequency * 2) / (double)curveLength) / samplerate;
    m1 = a / x1;
    m2 = a / ( a - x1 );

    if(phase < x1) {
        warpedpos = m1 * phase;
    }
    else {
        warpedpos = (m2 * phase * -1) + m2;
    }

    frame = 127 * warpedpos;

    //"clip" signal
    if (curveClip == 1) {
        if (frame >= 126.88 || clip)
        {
            frame = 127;
            clip = true;
        } else if (clip) {
            frame = 50;
        }
    }

    phase+=velOscPhase;

    while (phase >= phaseLenght) {
        phase-= phaseLenght;
        x1 = (velocityCurve > 0) ? velocityCurve * 0.01 : 0.00000009;
    }

    return frame;
}
