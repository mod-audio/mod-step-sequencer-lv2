#include "velocityLFO.hpp"

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

void VelocityLFO::setVelocityCurve(float velocityCurve)
{
	this->velocityCurve = velocityCurve;
}

void VelocityLFO::setCurveDepth(float curveDepth)
{
	this->curveDepth = curveDepth;
}

void VelocityLFO::setCurveClip(bool curveClip)
{
	this->curveClip = curveClip;
}

void VelocityLFO::setCurveLength(int curveLength)
{
	this->curveLength = curveLength;
}

void VelocityLFO::setPhase(double phase)
{
    this->phase = phase;
}

void VelocityLFO::setFrequency(double newFrequency)
{
	if (newFrequency != frequency) {
		frequency = 1.0 / newFrequency;
	}
}

double VelocityLFO::getSample()
{
    return sample;
}

float VelocityLFO::getVelocityCurve()
{
	return velocityCurve;
}

float VelocityLFO::getCurveDepth()
{
	return curveDepth;
}

bool VelocityLFO::getCurveClip()
{
	return curveClip;
}

int VelocityLFO::getCurveLength()
{
	return curveLength;
}

double VelocityLFO::getFrequency()
{
	return frequency;
}

void VelocityLFO::tick()
{
    sample = calculate() * curveDepth;
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
