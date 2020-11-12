#include "clock.hpp"
#include <iostream>

PluginClock::PluginClock() :
	gate(false),
	trigger(false),
	beatSync(true),
	phaseReset(false),
	playing(false),
	previousPlaying(false),
	endOfBar(false),
	init(false),
	pos(0),
	bpm(120.0),
	swing(0.0),
	randomize(0.0),
	internalBpm(120.0),
	previousBpm(0),
	sampleRate(48000.0),
	frequency(1.0),
	previousSyncMode(0),
	barLength(4),
	previousBeat(0),
	triggerIndex(0)
{
	//TODO everything initialized?
	srand (static_cast <unsigned> (time(0)));
	randomValue = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));

	numFramesQuarterNote = (60 / bpm) * sampleRate;
}

PluginClock::~PluginClock()
{
}

void PluginClock::transmitHostInfo(const bool playing, const float beatsPerBar,
		const int hostBeat, const float hostBarBeat, const float hostBpm)
{
	this->beatsPerBar = beatsPerBar;
	this->hostBeat = hostBeat;
	this->hostBarBeat = hostBarBeat;
	this->hostBpm = hostBpm;
	this->playing = playing;

	if (playing && !previousPlaying && beatSync) {
		syncClock();
	}
	if (playing != previousPlaying) {
		previousPlaying = playing;
	}

	if (!init) {
		calcPeriod();
		init = true;
	}
}

void PluginClock::setSyncMode(int mode)
{
	switch (mode)
	{
		case FREE_RUNNING:
			beatSync = false;
			break;
		case HOST_BPM_SYNC:
			beatSync = false;
			break;
		case HOST_QUANTIZED_SYNC:
			beatSync = true;
			break;
	}

	this->syncMode = mode;
}

void PluginClock::setInternalBpmValue(float internalBpm)
{
	this->internalBpm = internalBpm;
}

void PluginClock::setBpm(float bpm)
{
	this->bpm = bpm;
	calcPeriod();
	numFramesQuarterNote = (60 / bpm) * sampleRate;
}

void PluginClock::setRandomizeTiming(float randomize)
{
	this->randomize = randomize;
}

void PluginClock::setSwing(float swing)
{
	this->swing = swing;
}

void PluginClock::setSampleRate(float sampleRate)
{
	this->sampleRate = sampleRate;
	calcPeriod();
}

void PluginClock::setDivision(int setDivision)
{
	if (division != setDivision) {
		this->division = setDivision;
		this->divisionValue = divisionValues[setDivision];
		calcPeriod();
	}
}

void PluginClock::syncClock()
{
    pos = static_cast<uint32_t>((fmod((sampleRate * (60.0f / (bpm*2)) * (hostBarBeat + (numBarsElapsed * beatsPerBar))), sampleRate * (60.0f / (bpm * (divisionValue/2.f)))) * 2.0));
}

void PluginClock::setPos(uint32_t pos)
{
	this->pos = pos;
}

void PluginClock::setNumBarsElapsed(uint32_t numBarsElapsed)
{
	this->numBarsElapsed = numBarsElapsed;
}

void PluginClock::calcPeriod()
{
	frequency = (60.0f / (bpm * (divisionValue/2.f))) * 2.0;
	//period = static_cast<uint32_t>((sampleRate * (60.0f / (bpm * (divisionValue/2.f)))) * 2.0);
	period = static_cast<uint32_t>(sampleRate * frequency);
	halfWavelength = static_cast<uint32_t>(period / 2.0f);
	quarterWaveLength = static_cast<uint32_t>(halfWavelength / 2.0f);
	period = (period <= 0) ? 1 : period;
}

void PluginClock::closeGate()
{
	gate = false;
}

void PluginClock::reset()
{
	trigger = false;
}

uint32_t PluginClock::getNumFramesQuarterNote()
{
    return numFramesQuarterNote;
}

float PluginClock::getSampleRate() const
{
	return sampleRate;
}

bool PluginClock::getGate() const
{
	return gate;
}

int PluginClock::getSyncMode() const
{
	return syncMode;
}

float PluginClock::getInternalBpmValue() const
{
	return internalBpm;
}

float PluginClock::getFrequency()
{
	return frequency;
}

int* PluginClock::getDivision()
{
	return &division;
}

uint32_t* PluginClock::getPeriod()
{
	return &period;
}

uint32_t* PluginClock::getPos()
{
	return &pos;
}

float PluginClock::getSwing() const
{
	return swing;
}

float PluginClock::getRandomizeTiming() const
{
	return randomize;
}

void PluginClock::tick()
{
	int beat = static_cast<int>(hostBarBeat);

	if (beatsPerBar <= 1) {
		if (hostBarBeat > 0.99 && !endOfBar) {
			endOfBar = true;
		}
		else if (hostBarBeat < 0.1 && endOfBar) {
			numBarsElapsed++;
			endOfBar = false;
		}
	} else {
		if (beat != previousBeat) {
			numBarsElapsed = (beat == 0) ? numBarsElapsed + 1 : numBarsElapsed;
			previousBeat = beat;
		}
	}

	float threshold = 0.009; //TODO might not be needed

	switch (syncMode)
	{
		case FREE_RUNNING:
			if ((internalBpm != previousBpm) || (syncMode != previousSyncMode)) {
				setBpm(internalBpm);
				previousBpm = internalBpm;
				previousSyncMode = syncMode;
			}
			break;
		case HOST_BPM_SYNC:
			if ((hostBpm != previousBpm && (fabs(previousBpm - hostBpm) > threshold)) || (syncMode != previousSyncMode)) {
				setBpm(hostBpm);
				previousBpm = hostBpm;
				previousSyncMode = syncMode;
			}
			break;
		case HOST_QUANTIZED_SYNC:
			if ((hostBpm != previousBpm && (fabs(previousBpm - hostBpm) > threshold)) || (syncMode != previousSyncMode)) {
				setBpm(hostBpm);
				if (playing) {
					syncClock();
				}
				previousBpm = hostBpm;
				previousSyncMode = syncMode;
			}
			break;
	}

	if (pos > period) {
		pos = 0;
	}

	float applied_swing = (swing * 0.5) + (randomize * randomValue);
	applied_swing = (applied_swing > 0.5) ? 0.5 : applied_swing;
	applied_swing = (applied_swing < -0.5) ? -0.5 : applied_swing;

	uint32_t triggerPos[2];
	triggerPos[0] = 0;
	triggerPos[1] = static_cast<uint32_t>(halfWavelength + (halfWavelength * applied_swing));

	if (pos < triggerPos[1] && trigger) {
		triggerIndex ^= 1;
		trigger = false;
	}
	if (pos >= triggerPos[triggerIndex] && !trigger) {
		randomValue = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
		gate = true;
		trigger = true;
	}

	if (playing && beatSync) {
		syncClock(); //hard-sync to host position
	} else {
		pos++;
	}
}
