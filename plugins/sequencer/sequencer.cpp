#include "sequencer.hpp"
#include <iostream>

Sequencer::Sequencer(double sampleRate) : velocityHandler(new VelocityHandler(static_cast<float>(sampleRate)))
{
	period = clock.getPeriod();
	clockPos = clock.getPos();
	division = clock.getDivision();

	metaRecorder = new MetaRecorder(period, clockPos, division);
	clock.transmitHostInfo(0, 4, 1, 1, 120.0);
	clock.setSampleRate(static_cast<float>(sampleRate));
	clock.setDivision(7);
	clock.setSyncMode(2);

	octavePattern = new Pattern*[5];

	octavePattern[0] = new PatternUp();
	octavePattern[1] = new PatternDown();
	octavePattern[2] = new PatternUpDown();
	octavePattern[3] = new PatternUpDownAlt();
	octavePattern[4] = new PatternCycle();

	for (unsigned i = 0; i < NUM_VOICES; i++) {
		midiNotes[i][MIDI_NOTE] = EMPTY_SLOT;
		midiNotes[i][MIDI_CHANNEL] = 0;
		midiNotes[i][NOTE_TYPE] = 0;
	}
	for (unsigned i = 0; i < NUM_VOICES; i++) {
		noteOffBuffer[i][MIDI_NOTE] = EMPTY_SLOT;
		noteOffBuffer[i][MIDI_CHANNEL] = 0;
		noteOffBuffer[i][NOTE_TYPE] = 0;
		noteOffBuffer[i][TIMER] = 0;
	}

	memset(modulatableParameters, 0, sizeof(modulatableParameters));
	memset(prevParameter, 0, sizeof(prevParameter));
}

Sequencer::~Sequencer()
{
	delete metaRecorder;
	metaRecorder = nullptr;
	delete velocityHandler;
	velocityHandler = nullptr;
	delete[] octavePattern;
	octavePattern = nullptr;
}

void Sequencer::setSampleRate(float sampleRate)
{
	this->sampleRate = sampleRate;
	clock.setSampleRate(sampleRate);
}

void Sequencer::setNotemode(int value)
{
	noteMode = value;
}

void Sequencer::setMode(int value)
{
	mode = value;


	switch (mode)
	{
		case STATE_CLEAR_ALL:
			clear();
			break;
		case STATE_STOP:
			playing = false;
			break;
		case STATE_UNDO_LAST:
			//undo
            if (stateIndex > 0) {
                for (int i = 0; i < prevState.numAcitveNotes; i++) {
                    midiNotes[i][MIDI_NOTE]    = prevState.midiNotes[stateIndex - 1][i][MIDI_NOTE];
                    midiNotes[i][MIDI_CHANNEL] = prevState.midiNotes[stateIndex - 1][i][MIDI_CHANNEL];
                    midiNotes[i][NOTE_TYPE]    = prevState.midiNotes[stateIndex - 1][i][NOTE_TYPE];
                }
            }
			stateIndex = (stateIndex > 0) ? stateIndex - 1 : stateIndex;
			notePlayed = notePlayed % prevState.numAcitveNotes;
			numActiveNotes = prevState.numAcitveNotes;
			break;
	}
}

void Sequencer::setQuantizeMode(float value)
{
    quantizeMultiplier = value;
}

void Sequencer::setDivision(int value)
{
	modulatableParameters[1] = value;
}

void Sequencer::setNoteLength(float value)
{
	modulatableParameters[2] = value;
}

void Sequencer::setOctaveSpread(int value)
{
	modulatableParameters[3] = value;
}

void Sequencer::setMaxLength(int value)
{
	maxLength = value;
}

void Sequencer::setPlaymode(int value)
{
	playMode = value;
}

void Sequencer::setModulatableParameters(float value, int index)
{
	modulatableParameters[index] = value;
}

void Sequencer::setSwing(float value)
{
	modulatableParameters[4] = value;
}

void Sequencer::setRandomizeTiming(float value)
{
	modulatableParameters[5] = value;
}

void Sequencer::setVelocityMode(int value)
{
	modulatableParameters[6] = value;
}

void Sequencer::setVelocityCurve(float value)
{
	modulatableParameters[7] = value;
}

void Sequencer::setCurveDepth(float value)
{
	modulatableParameters[8] = value;
}

void Sequencer::setCurveClip(bool value)
{
	modulatableParameters[9] = value;
}

void Sequencer::setCurveLength(int value)
{
	velocityHandler->setCurveLength(value);
}

void Sequencer::setPatternlength(int value)
{
	velocityHandler->setNumSteps(value);
}

void Sequencer::setVelocityNote(int index, int value)
{
	modulatableParameters[index + 10] = value;
}

void Sequencer::setConnectLfo1(int value)
{
	connectLfo1 = value;
}

void Sequencer::setLFO1depth(float value)
{
	lfo1Depth = value;
}

void Sequencer::setConnectLfo2(int value)
{
	connectLfo2 = value;
}

void Sequencer::setLFO2depth(float value)
{
	lfo2Depth = value;
}

void Sequencer::setMetaRecord(bool value)
{
	metaRecordingEnabled = value;
}

void Sequencer::setMetaMode(int value)
{
	metaRecordMode = value;
}

void Sequencer::setMetaQuantizeValue(int value)
{
	metaQuantizeValue = value;
}

void Sequencer::setPanic(bool value)
{
	panic = value;
}

void Sequencer::setEnabled(bool value)
{
	sequencerEnabled = value;
}


float Sequencer::applyRange(float numberToCheck, float min, float max)
{
    if (numberToCheck < min)
        return min;
    else if (numberToCheck > max)
        return max;
    else
        return numberToCheck;
}

void Sequencer::setParameters()
{
	for (unsigned p = 1; p < 18; p++) {
		variables[p] = getModulatableParameters(p);
	}

    int param = connectLfo1;
    if (param > 0) {
        float lfoValue   = maxParamValue[param] * lfo1Depth * (lfo1Value * 0.1);
        variables[param] = variables[param] + lfoValue;
        variables[param] = applyRange(variables[param], minParamValue[param], maxParamValue[param]);
    }
    param = connectLfo1;
    if (param > 0) {
        float lfoValue   = maxParamValue[param] * lfo2Depth * (lfo2Value * 0.1);
        variables[param] = variables[param] + lfoValue;
        variables[param] = applyRange(variables[param], minParamValue[param], maxParamValue[param]);
    }

    clock.setDivision((int)variables[1]);
    noteLength   = variables[2];
    octaveSpread = variables[3];
    clock.setSwing(variables[4]);
    clock.setRandomizeTiming(variables[5]);
    velocityHandler->setMode((int)variables[6]);
	velocityHandler->setVelocityCurve(variables[7]);
    velocityHandler->setCurveDepth(variables[8]);
	velocityHandler->setCurveClip((bool)variables[9]);
	velocityHandler->setVelocityPattern(0, (uint8_t)variables[10]);
	velocityHandler->setVelocityPattern(1, (uint8_t)variables[11]);
	velocityHandler->setVelocityPattern(2, (uint8_t)variables[12]);
	velocityHandler->setVelocityPattern(3, (uint8_t)variables[13]);
	velocityHandler->setVelocityPattern(4, (uint8_t)variables[14]);
	velocityHandler->setVelocityPattern(5, (uint8_t)variables[15]);
	velocityHandler->setVelocityPattern(6, (uint8_t)variables[16]);
	velocityHandler->setVelocityPattern(7, (uint8_t)variables[17]);
}

float Sequencer::getModulatableParameters(int index) const
{
	return modulatableParameters[index];
}

int Sequencer::getNotemode() const
{
	return noteMode;
}

int Sequencer::getMode() const
{
	return mode;
}

int Sequencer::getDivision() const
{
	return *division;
}

float Sequencer::getQuantizeMode() const
{
	return quantizeMultiplier;
}

float Sequencer::getNoteLength() const
{
	return noteLength;
}

int Sequencer::getOctaveSpread() const
{
	return octaveSpread;
}

int Sequencer::getMaxLength() const
{
	return maxLength;
}

int Sequencer::getPlaymode() const
{
	return playMode;
}

float Sequencer::getSwing() const
{
	return clock.getSwing();
}

float Sequencer::getRandomizeTiming() const
{
	return clock.getRandomizeTiming();
}

int Sequencer::getVelocityMode() const
{
	return velocityMode;
}

float Sequencer::getVelocityCurve() const
{
	return velocityCurve;
}

float Sequencer::getCurveDepth() const
{
	return velocityHandler->getCurveDepth();
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
	return lfo1Depth;
}

int Sequencer::getConnectLfo2() const
{
	return connectLfo2;
}

float Sequencer::getLFO2depth() const
{
	return lfo2Depth;
}

bool Sequencer::getMetaRecord() const
{
	return metaRecorder->getRecordingMode();
}

int Sequencer::getMetaMode() const
{
	return metaRecordMode;
}

int Sequencer::getMetaQuantizeValue() const
{
	return metaQuantizeValue;
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
		midiNotes[i][NOTE_TYPE] = 0;
	}
}

void Sequencer::emptyMidiBuffer()
{
	midiHandler.emptyMidiBuffer();
}

void Sequencer::clear() //TODO reset?
{
	numActiveNotes = 0;
	notePlayed = 0;
	for (unsigned i = 0; i < NUM_VOICES; i++) {
		midiNotes[i][MIDI_NOTE] = EMPTY_SLOT;
		midiNotes[i][MIDI_CHANNEL] = 0;
		midiNotes[i][NOTE_TYPE] = 0;
	}

	for (unsigned o = 0; o < NUM_OCTAVE_MODES; o++) {
		octavePattern[o]->reset();
	}
}

struct MidiBuffer Sequencer::getMidiBuffer()
{
	return midiHandler.getMidiBuffer();
}

void Sequencer::process(const float **cvInputs, const MidiEvent* events, uint32_t eventCount, uint32_t n_frames)
{
    struct MidiEvent midiEvent;
    struct MidiEvent midiThroughEvent;

	lfo1Value = cvInputs[0][0];
	lfo2Value = cvInputs[1][0];

	setParameters();

	if (metaRecordingEnabled && (metaRecordMode == START_RECORDING_WHEN_ENABLED)) {
		metaRecording = true;
	} else if (!metaRecordingEnabled) {
		metaRecording = false;
	}

	if (panic) {
		reset();
		panic = false;
	}

	for (uint32_t i=0; i<eventCount; ++i) {

		uint8_t status = events[i].data[0] & 0xF0;
		uint8_t midiNote = events[i].data[1];

		if (sequencerEnabled) {

			if (midiNote == 0x7b && events[i].size == 3) {
				clear();
			}

			uint8_t channel = events[i].data[0] & 0x0F;

			switch(status) {
				case MIDI_NOTEON:
					//store previous state
					if (metaRecordingEnabled && (metaRecordMode ==  START_RECORDING_NOTE_ON)) {
						metaRecording = true;
					}
					if (numActiveNotes > 0) {
						//added new state
						for (int p = 0; p < numActiveNotes; p++) {
							prevState.midiNotes[stateIndex][p][MIDI_NOTE]    = midiNotes[p][MIDI_NOTE];
							prevState.midiNotes[stateIndex][p][MIDI_CHANNEL] = midiNotes[p][MIDI_CHANNEL];
							prevState.midiNotes[stateIndex][p][NOTE_TYPE]    = midiNotes[p][NOTE_TYPE];
						}
						prevState.numAcitveNotes = numActiveNotes;
						stateIndex = (stateIndex + 1) % NUM_STATES;
					}

					switch (mode)
					{
						case STATE_RECORD:
							if (!recording) {
								clear();
							}
							playing = false;
							recording = true;
							midiNotes[numActiveNotes][MIDI_NOTE]    = midiNote;
							midiNotes[numActiveNotes][MIDI_CHANNEL] = channel;
							midiNotes[numActiveNotes][NOTE_TYPE]    = noteMode;
							numActiveNotes = (numActiveNotes + 1) % NUM_VOICES;
							overwrite = false;
							break;
						case STATE_PLAY:
							switch (playMode)
							{
								case PLAY_MOMENTARY:
									playing = true;
									break;
								case PLAY_LATCH_TRANSPOSE:
									{
									uint8_t firstNote =  midiNotes[overwriteIndex][MIDI_NOTE];
									transpose = midiNote - firstNote;
									}
									break;
								case PLAY_NOTES:
									midiHandler.appendMidiMessage(events[i]);
									break;
							}
							overwrite = false;
							break;
						case STATE_RECORD_OVERWRITE:
							if (!overwrite) {
								overwriteIndex = 0;
								overwrite = true;
							}
							midiNotes[overwriteIndex][MIDI_NOTE]    = midiNote;
							midiNotes[overwriteIndex][MIDI_CHANNEL] = channel;
							midiNotes[overwriteIndex][NOTE_TYPE]    = noteMode;
							overwriteIndex = (overwriteIndex + 1) % numActiveNotes;
							break;
						case STATE_RECORD_APPEND:
							midiNotes[numActiveNotes][MIDI_NOTE]    = midiNote;
							midiNotes[numActiveNotes][MIDI_CHANNEL] = channel;
							midiNotes[numActiveNotes][NOTE_TYPE]    = noteMode;
							numActiveNotes = (numActiveNotes + 1) % NUM_VOICES;
							overwrite = false;
							break;
					}
					break;
				case MIDI_NOTEOFF:
					switch (mode)
					{
						case STATE_RECORD:
							break;
						case STATE_PLAY:
							switch (playMode)
							{
								case PLAY_MOMENTARY:
									playing = false;
									break;
								case PLAY_LATCH_TRANSPOSE:
									break;
								case PLAY_NOTES:
									midiHandler.appendMidiMessage(events[i]);
									break;
							}
							break;
						case STATE_RECORD_OVERWRITE:
							break;
						case STATE_RECORD_APPEND:
							break;
					}
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
			if (mode < 3) {
				midiHandler.appendMidiThroughMessage(events[i]);
			}
		} else { //if sequencer is off
			//send MIDI message through
			midiHandler.appendMidiThroughMessage(events[i]);
		}
	}

	metaRecorder->setRecordingMode(metaRecording);

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
		uint32_t numFramesPerNote = clock.getNumFramesQuarterNote();
		if (playMode != PLAY_MOMENTARY
				&& (((uint32_t)(barBeat * numFramesPerNote) % numFramesPerNote) < 100)) {
			playing = true;
		}
    }

	if (metaRecorder->recordingQued()) {

		for (int e = 0; e < metaRecorder->getRecLength(); e++) {
			midiNotes[e][MIDI_NOTE] = metaRecorder->getMidiBuffer(e, MIDI_NOTE);
			midiNotes[e][MIDI_CHANNEL] = metaRecorder->getMidiBuffer(e, MIDI_CHANNEL);
			midiNotes[e][NOTE_TYPE] = metaRecorder->getMidiBuffer(e, NOTE_TYPE);
		}
		notePlayed = 0;
		transpose = 0;
		numActiveNotes = metaRecorder->getRecLength();
		metaRecorder->setQue(false);
	}

	for (unsigned s = 0; s < n_frames; s++) {

		clock.tick();
		velocityHandler->process();

		if (clock.getGate()) {

			if (playing && numActiveNotes > 0 ) {

				if (midiNotes[notePlayed][MIDI_NOTE] > 0
						&& midiNotes[notePlayed][MIDI_NOTE] < 128
						&& midiNotes[notePlayed][NOTE_TYPE] > 0)
				{
					//create MIDI note on message
					uint8_t midiNote = midiNotes[notePlayed][MIDI_NOTE];
					uint8_t channel = midiNotes[notePlayed][MIDI_CHANNEL];
					uint8_t noteType = midiNotes[notePlayed][NOTE_TYPE];

					if (sequencerEnabled) {

						uint8_t octave = octavePattern[octaveMode]->getStep() * 12;
						octavePattern[octaveMode]->goToNextStep();

						midiNote = midiNote + octave + transpose;

						metaRecorder->record(midiNote, channel, noteType);

						midiEvent.frame = s;
						midiEvent.size = 3;
						midiEvent.data[0] = MIDI_NOTEON | channel;
						midiEvent.data[1] = midiNote;
						midiEvent.data[2] = velocityHandler->getVelocity();

						midiHandler.appendMidiMessage(midiEvent);

						noteOffBuffer[activeNotesIndex][MIDI_NOTE] = (uint32_t)midiNote;
						noteOffBuffer[activeNotesIndex][MIDI_CHANNEL] = (uint32_t)channel;
						noteOffBuffer[activeNotesIndex][NOTE_TYPE] = (uint32_t)noteType;
						activeNotesIndex = (activeNotesIndex + 1) % NUM_NOTE_OFF_SLOTS;
					}
				}
				int sequenceLength = (maxLength == 9) ? numActiveNotes : maxLength;
				sequenceLength = (sequenceLength > numActiveNotes) ? numActiveNotes : sequenceLength;
				notePlayed = (notePlayed + 1) % sequenceLength;
				velocityHandler->goToNextStep();
			}
		}
		clock.closeGate();

		for (size_t i = 0; i < NUM_NOTE_OFF_SLOTS; i++) {
			if (noteOffBuffer[i][MIDI_NOTE] != EMPTY_SLOT) {
				noteOffBuffer[i][TIMER] += 1;
				uint32_t duration = static_cast<uint32_t>((noteOffBuffer[i][NOTE_TYPE] != 2) ? *clock.getPeriod() * noteLength : *clock.getPeriod());
				duration *= 0.5;
				if (noteOffBuffer[i][TIMER] > duration) {
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
