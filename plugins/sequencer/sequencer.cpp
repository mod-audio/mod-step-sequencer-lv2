#include "sequencer.hpp"

Sequencer::Sequencer()
{
	clock.transmitHostInfo(0, 4, 1, 1, 120.0);
	clock.setSampleRate(static_cast<float>(48000.0));
	clock.setDivision(7);

	SeqPattern = new Pattern*[6];

	SeqPattern[0] = new PatternUp();
	SeqPattern[1] = new PatternDown();
	SeqPattern[2] = new PatternUpDown();
	SeqPattern[3] = new PatternUpDownAlt();
	SeqPattern[4] = new PatternUp();
	SeqPattern[5] = new PatternRandom();


	octavePattern = new Pattern*[5];

	octavePattern[0] = new PatternUp();
	octavePattern[1] = new PatternDown();
	octavePattern[2] = new PatternUpDown();
	octavePattern[3] = new PatternUpDownAlt();
	octavePattern[4] = new PatternCycle();

	for (unsigned i = 0; i < NUM_VOICES; i++) {
		midiNotes[i][0] = EMPTY_SLOT;
		midiNotes[i][1] = 0;
	}
	for (unsigned i = 0; i < NUM_VOICES; i++) {
		noteOffBuffer[i][MIDI_NOTE] = EMPTY_SLOT;
		noteOffBuffer[i][MIDI_CHANNEL] = 0;
		noteOffBuffer[i][TIMER] = 0;
	}
}

Sequencer::~Sequencer()
{
	delete[] SeqPattern;
	SeqPattern = nullptr;
	delete[] octavePattern;
	octavePattern = nullptr;
}

void Sequencer::setSampleRate(float sampleRate)
{
	this->sampleRate = sampleRate;
}
void Sequencer::setNotemode(int value)
{
	notemode = value;
}

void Sequencer::setMode(int value)
{
	mode = value;
}

void Sequencer::setDivision(int value)
{
	division = value;
}

void Sequencer::setNoteLength(float value)
{
	noteLength = value;
}

void Sequencer::setOctaveSpread(int value)
{
	octaveSpread = value;
}

void Sequencer::setPlaymode(int value)
{
	playmode = value;
}

void Sequencer::setSwing(float value)
{
	swing = value;
}

void Sequencer::setRandomizeTiming(float value)
{
	randomizeTiming = value;
}

void Sequencer::setVelocityMode(int value)
{
	velocityMode = value;
}

void Sequencer::setVelocityCurve(float value)
{
	velocityCurve = value;
}

void Sequencer::setCurvedepth(float value)
{
	curvedepth = value;
}

void Sequencer::setCurveClip(bool value)
{
	curveClip = value;
}

void Sequencer::setCurveLength(int value)
{
	curveLength = value;
}

void Sequencer::setPatternlength(int value)
{
	patternlength = value;
}

void Sequencer::setVelocityNote(int index, int value)
{
	velocityNote[index] = value;
}

void Sequencer::setConnectLfo1(int value)
{
	connectLfo1 = value;
}

void Sequencer::setLFO1depth(float value)
{
	lFO1depth = value;
}

void Sequencer::setConnectLfo2(int value)
{
	connectLfo2 = value;
}

void Sequencer::setLFO2depth(float value)
{
	lFO2depth = value;
}

void Sequencer::setEnabled(bool value)
{
	enabled = value;
}

int Sequencer::getNotemode() const
{
	return notemode;
}

int Sequencer::getMode() const
{
	return mode;
}

int Sequencer::getDivision() const
{
	return division;
}

float Sequencer::getNoteLength() const
{
	return noteLength;
}

int Sequencer::getOctaveSpread() const
{
	return octaveSpread;
}

int Sequencer::getPlaymode() const
{
	return playmode;
}

float Sequencer::getSwing() const
{
	return swing;
}

float Sequencer::getRandomizeTiming() const
{
	return randomizeTiming;
}

int Sequencer::getVelocityMode() const
{
	return velocityMode;
}

float Sequencer::getVelocityCurve() const
{
	return velocityCurve;
}

float Sequencer::getCurvedepth() const
{
	return curvedepth;
}

bool Sequencer::getCurveClip() const
{
	return curveClip;
}

int Sequencer::getCurveLength() const
{
	return curveLength;
}

int Sequencer::getPatternlength() const
{
	return patternlength;
}

int Sequencer::getVelocityNote(int index) const
{
	return velocityNote[index];
}

int Sequencer::getConnectLfo1() const
{
	return connectLfo1;
}

float Sequencer::getLFO1depth() const
{
	return lFO1depth;
}

int Sequencer::getConnectLfo2() const
{
	return connectLfo2;
}

float Sequencer::getLFO2depth() const
{
	return lFO2depth;
}

bool Sequencer::getEnabled() const
{
	return enabled;
}

void Sequencer::transmitHostInfo(const bool playing, const float beatsPerBar,
		const int beat, const float barBeat, const double bpm)
{
	clock.transmitHostInfo(playing, beatsPerBar, beat, barBeat, bpm);
	this->barBeat = barBeat;
}

void Sequencer::reset()
{
	clock.reset();
	clock.setNumBarsElapsed(0);

	for (unsigned a = 0; a < NUM_ARP_MODES; a++) {
		SeqPattern[SeqMode]->reset();
	}
	for (unsigned o = 0; o < NUM_OCTAVE_MODES; o++) {
		octavePattern[o]->reset();
	}

	activeNotesIndex = 0;
	firstNoteTimer  = 0;
	notePlayed = 0;
	activeNotes = 0;
	//previousLatch = 0;
	notesPressed = 0;
	activeNotesBypassed = 0;
	latchPlaying = false;
	firstNote = false;
	first = true;

	for (unsigned i = 0; i < NUM_VOICES; i++) {
		midiNotes[i][MIDI_NOTE] = EMPTY_SLOT;
		midiNotes[i][MIDI_CHANNEL] = 0;
	}
}

void Sequencer::emptyMidiBuffer()
{
	midiHandler.emptyMidiBuffer();
}

struct MidiBuffer Sequencer::getMidiBuffer()
{
	return midiHandler.getMidiBuffer();
}

void Sequencer::process(const MidiEvent* events, uint32_t eventCount, uint32_t n_frames)
{
    struct MidiEvent midiEvent;
    struct MidiEvent midiThroughEvent;
}
