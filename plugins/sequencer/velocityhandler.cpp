#include "velocityhandler.hpp"

VelocityHandler::VelocityHandler(float sampleRate) : lfo(new VelocityLFO(sampleRate, 0.1, 0.0))
{
	for (unsigned i = 0; i < NUM_STEPS; i++) {
		velocityPattern[i] = 0;
	}
}

VelocityHandler::~VelocityHandler()
{
	delete lfo;
	lfo = nullptr;
}

void VelocityHandler::setVelocityCurve(float value)
{
	lfo->setVelocityCurve(value);
}

void VelocityHandler::setCurveDepth(float value)
{
	lfo->setCurveDepth(value);
}

void VelocityHandler::setCurveClip(bool value)
{
	lfo->setCurveClip(value);
}

void VelocityHandler::setCurveLength(int value)
{
	lfo->setCurveLength(value);
}

void VelocityHandler::setSampleRate(float sampleRate)
{
	this->sampleRate = sampleRate;
}

void VelocityHandler::setMode(int velocityMode)
{
	this->velocityMode = velocityMode;
}

void VelocityHandler::setNumSteps(int numSteps)
{
	if (numSteps > 0 && numSteps < NUM_STEPS) {
		this->numSteps = numSteps;
	}
}

void VelocityHandler::setVelocityPattern(int index, uint8_t value)
{
	velocityPattern[index] = value;
}

void VelocityHandler::goToNextStep()
{
	step = (step + 1) % numSteps;
}

uint8_t VelocityHandler::getVelocity()
{
	std::cout << "velocity in pattern = " << (int)velocity << std::endl;
	return velocity;
}

float VelocityHandler::getVelocityCurve()
{
	return lfo->getVelocityCurve();
}

float VelocityHandler::getCurveDepth()
{
	return lfo->getCurveDepth();
}

bool VelocityHandler::getCurveClip()
{
	return lfo->getCurveClip();
}

int VelocityHandler::getCurveLength()
{
	return lfo->getCurveLength();
}

void VelocityHandler::process()
{
	switch (velocityMode)
	{
		case STATIC:
			velocity = 100;
			break;
		case LFO:
			velocity = static_cast<uint8_t>(lfo->getSample());
			break;
		case PATTERN:
			velocity = velocityPattern[step];
			break;
	}
	lfo->tick();
}
