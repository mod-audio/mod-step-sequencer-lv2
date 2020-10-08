#include "sequencer.hpp"
#include <iostream>

Sequencer::Sequencer()
{
	clock.transmitHostInfo(0, 4, 1, 1, 120.0);
	clock.setSampleRate(static_cast<float>(48000.0));
	clock.setDivision(7);

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
	clock.setDivision(value);
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
	clock.setSwing(value);
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

void Sequencer::setPanic(bool value)
{
	panic = value;
}

void Sequencer::setEnabled(bool value)
{
	sequencerEnabled = value;
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
	return clock.getSwing();
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

bool Sequencer::getPanic() const
{
	return panic;
}

bool Sequencer::getEnabled() const
{
	return sequencerEnabled;
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

void Sequencer::clear()
{
	numActiveNotes = 0;
	notePlayed = 0;
	for (unsigned i = 0; i < NUM_VOICES; i++) {
		midiNotes[i][0] = EMPTY_SLOT;
		midiNotes[i][1] = 0;
	}
}

struct MidiBuffer Sequencer::getMidiBuffer()
{
	return midiHandler.getMidiBuffer();
}

void Sequencer::process(const MidiEvent* events, uint32_t eventCount, uint32_t n_frames)
{
    struct MidiEvent midiEvent;
    struct MidiEvent midiThroughEvent;

	if (panic) {
		reset();
		panic = false;
	}

	for (uint32_t i=0; i<eventCount; ++i) {

		uint8_t status = events[i].data[0] & 0xF0;

		uint8_t midiNote = events[i].data[1];
		uint8_t noteToFind;
		size_t searchNote;

		if (sequencerEnabled) {

			//midiNotesCopied = false;

			//bool voiceFound;
			//bool pitchFound;
			//size_t findFreeVoice;
			//size_t findActivePitch;

			if (midiNote == 0x7b && events[i].size == 3) {
				clear();
			}

			uint8_t channel = events[i].data[0] & 0x0F;

			switch(status) {
				case MIDI_NOTEON:
					switch (mode)
					{
						case STATE_RECORD:
							if (!recording) {
								clear();
							}
							playing = false;
							recording = true;
							midiNotes[numActiveNotes][MIDI_NOTE] = midiNote;
							midiNotes[numActiveNotes][MIDI_CHANNEL] = channel;
							numActiveNotes++;
							overwrite = false;
							break;
						case STATE_RECORD_APPEND:
							midiNotes[numActiveNotes][MIDI_NOTE] = midiNote;
							midiNotes[numActiveNotes][MIDI_CHANNEL] = channel;
							numActiveNotes++;
							overwrite = false;
							break;
						case STATE_RECORD_OVERWRITE:
							if (!overwrite) {
								overwriteIndex = 0;
								overwrite = true;
							}
							midiNotes[overwriteIndex][MIDI_NOTE] = midiNote;
							midiNotes[overwriteIndex][MIDI_CHANNEL] = channel;
							overwriteIndex = (overwriteIndex + 1) % numActiveNotes;
							break;
					}
					break;
				case MIDI_NOTEOFF:
					break;
				default:
					midiThroughEvent.frame = events[i].frame;
					midiThroughEvent.size = events[i].size;
					for (unsigned d = 0; d < midiThroughEvent.size; d++) {
						midiThroughEvent.data[d] = events[i].data[d];
					}
					midiHandler.appendMidiThroughMessage(midiThroughEvent);
					break;
			}
		} else { //if sequencer is off
			//send MIDI message through
			midiHandler.appendMidiThroughMessage(events[i]);
		}
	}

	int patternSize = 1;

	switch (octaveMode)
	{
		case ONE_OCT_UP_PER_CYCLE:
			octavePattern[octaveMode]->setPatternSize(patternSize);
			octavePattern[octaveMode]->setCycleRange(octaveSpread);
			break;
		default:
			octavePattern[octaveMode]->setPatternSize(octaveSpread);
			break;
	}

	if (mode == STATE_PLAY) {
		recording = false;
		playing = true;
	}

	switch (mode)
	{
		case STATE_CLEAR_ALL:
			clear();
			break;
		case STATE_STOP:
			playing = false;
			break;
		case STATE_UNDO_LAST:
			break;
	}

	for (unsigned s = 0; s < n_frames; s++) {

		clock.tick();

		if (clock.getGate()) {

			if (playing && numActiveNotes > 0 ) {

				if (midiNotes[notePlayed][MIDI_NOTE] > 0
						&& midiNotes[notePlayed][MIDI_NOTE] < 128)
				{
					//create MIDI note on message
					uint8_t midiNote = midiNotes[notePlayed][MIDI_NOTE];
					uint8_t channel = midiNotes[notePlayed][MIDI_CHANNEL];

					if (sequencerEnabled) {

						uint8_t octave = octavePattern[octaveMode]->getStep() * 12;
						octavePattern[octaveMode]->goToNextStep();

						midiNote = midiNote + octave;

						midiEvent.frame = s;
						midiEvent.size = 3;
						midiEvent.data[0] = MIDI_NOTEON | channel;
						midiEvent.data[1] = midiNote;
						midiEvent.data[2] = velocity;

						midiHandler.appendMidiMessage(midiEvent);

						noteOffBuffer[activeNotesIndex][MIDI_NOTE] = (uint32_t)midiNote;
						noteOffBuffer[activeNotesIndex][MIDI_CHANNEL] = (uint32_t)channel;
						activeNotesIndex = (activeNotesIndex + 1) % NUM_NOTE_OFF_SLOTS;
					}
				}
				notePlayed = (notePlayed + 1) % numActiveNotes;
			}
		}
		clock.closeGate();

		for (size_t i = 0; i < NUM_NOTE_OFF_SLOTS; i++) {
			if (noteOffBuffer[i][MIDI_NOTE] != EMPTY_SLOT) {
				noteOffBuffer[i][TIMER] += 1;
				if (noteOffBuffer[i][TIMER] > static_cast<uint32_t>(clock.getPeriod() * noteLength)) {
					midiEvent.frame = s;
					midiEvent.size = 3;
					midiEvent.data[0] = MIDI_NOTEOFF | noteOffBuffer[i][MIDI_CHANNEL];
					midiEvent.data[1] = static_cast<uint8_t>(noteOffBuffer[i][MIDI_NOTE]);
					midiEvent.data[2] = 0;

					midiHandler.appendMidiMessage(midiEvent);

					noteOffBuffer[i][MIDI_NOTE] = EMPTY_SLOT;
					noteOffBuffer[i][MIDI_CHANNEL] = 0;
					noteOffBuffer[i][TIMER] = 0;

				}
			}
		}
	}
	midiHandler.mergeBuffers();
}
