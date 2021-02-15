#include "sequencer.hpp"

Sequencer::Sequencer(double sampleRate) :
    velocityHandler(new VelocityHandler(static_cast<float>(sampleRate))),
    notePlayed(0),
    octaveMode(0),
    metaTranspose(0),
    transpose(0),
    activeNotesIndex(0),
    stateIndex(0),
    numActiveNotes(0),
    sampleRate(48000),
    metaSpeed(1),
    metaRecordingEnabled(false),
    metaRecording(false),
    firstNote(false),
    overwrite(false),
    metaRecordMode(0),
    metaQuantizeValue(0),
    overwriteIndex(0),
    noteMode(0),
    mode(0),
    noteLength(0.8),
    octaveSpread(1),
    playMode(0),
    swing(0.0),
    randomizeTiming(0),
    velocityMode(0),
    velocityCurve(0),
    maxLength(9),
    curveDepth(0.0),
    curveClip(0),
    curveLength(1),
    patternlength(1),
    connectLfo1(0),
    lfo1Depth(0),
    connectLfo2(0),
    lfo2Depth(0),
    sequencerEnabled(true),
    panic(false),
    lfo1Value(0.0),
    lfo2Value(0.0),
    prevModulation1(0.0),
    prevModulation2(0.0),
    prevParam1(0),
    prevParam2(0),
    recording(false),
    playing(false),
    requestIndex(0)
{
    period = clock.getPeriod();
    clockPos = clock.getPos();
    division = clock.getDivision();

    metaRecorder = new MetaRecorder(period, clockPos, division);
    clock.transmitHostInfo(0, 4, 1, 1, 120.0);
    clock.setSampleRate(static_cast<float>(sampleRate));
    clock.setDivision(7);
    clock.setSyncMode(2);

    octavePattern = new Pattern*[NUM_OCTAVE_MODES];

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

    modulatableParameters[0] = 0;
    modulatableParameters[1] = DIVISION;
    modulatableParameters[2] = noteLength;
    modulatableParameters[3] = octaveSpread;
    modulatableParameters[4] = swing;
    modulatableParameters[5] = randomizeTiming;
    modulatableParameters[6] = velocityMode;
    modulatableParameters[7] = velocityCurve;
    modulatableParameters[8] = curveDepth;
    modulatableParameters[9] = curveDepth;
    modulatableParameters[10] = 100.0;
    modulatableParameters[11] = 100.0;
    modulatableParameters[12] = 100.0;
    modulatableParameters[13] = 100.0;
    modulatableParameters[14] = 100.0;
    modulatableParameters[15] = 100.0;
    modulatableParameters[16] = 100.0;
    modulatableParameters[17] = 100.0;
}

Sequencer::~Sequencer()
{
    delete metaRecorder;
    metaRecorder = nullptr;
    delete velocityHandler;
    velocityHandler = nullptr;

    for (unsigned i = 0; i < NUM_OCTAVE_MODES; i++) {
        delete octavePattern[i];
    }

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

void Sequencer::setMetaSpeed(int value)
{
    metaSpeed = value;
}

void Sequencer::setPanic(bool value)
{
    panic = value;
}

void Sequencer::setEnabled(bool value)
{
    sequencerEnabled = value;
}

void Sequencer::setRequestValueChange(int index, float value)
{
    bufferedRequests[0][requestIndex] = (float)index;
    bufferedRequests[1][requestIndex] = value;
    requestIndex++;
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

void Sequencer::setParameterRanges(int parameterIndex)
{
    switch (parameterIndex)
    {
        case DIVISION:
            parameterRange.min = 0.0;
            parameterRange.max = 12.0;
            break;
        case NOTELENGTHPARAM:
            parameterRange.min = 0.1;
            parameterRange.max = 1.0;
            break;
        case OCTAVESPREAD:
            parameterRange.min = 1.0;
            parameterRange.max = 4.0;
            break;
        case SWING:
            parameterRange.min = -1.0;
            parameterRange.max = 1.0;
            break;
        case RANDOMIZETIMMING:
            parameterRange.min = 0.0;
            parameterRange.max = 1.0;
            break;
        case VELOCITYMODE:
            parameterRange.min = 0.0;
            parameterRange.max = 2.0;
            break;
        case VELOCITYCURVE:
            parameterRange.min = 0.0;
            parameterRange.max = 70.0;
            break;
        case CURVEDEPTH:
            parameterRange.min = 0.0;
            parameterRange.max = 1.0;
            break;
        case CURVECLIP:
            parameterRange.min = 0.0;
            parameterRange.max = 1.0;
            break;
        case PATTERNVEL1:
            parameterRange.min = 0.0;
            parameterRange.max = 127.0;
            break;
        case PATTERNVEL2:
            parameterRange.min = 0.0;
            parameterRange.max = 127.0;
            break;
        case PATTERNVEL3:
            parameterRange.min = 0.0;
            parameterRange.max = 127.0;
            break;
        case PATTERNVEL4:
            parameterRange.min = 0.0;
            parameterRange.max = 127.0;
            break;
        case PATTERNVEL5:
            parameterRange.min = 0.0;
            parameterRange.max = 127.0;
            break;
        case PATTERNVEL6:
            parameterRange.min = 0.0;
            parameterRange.max = 127.0;
            break;
        case PATTERNVEL7:
            parameterRange.min = 0.0;
            parameterRange.max = 127.0;
            break;
        case PATTERNVEL8:
            parameterRange.min = 0.0;
            parameterRange.max = 127.0;
            break;
        default:
            parameterRange.min = 0.0;
            parameterRange.max = 127.0;
            break;
    }


}

//this function doesn't do any real type conversition because the parameter are all of type float in the end.
//Instead it floors all values of parameters that need to have an integer value.
float Sequencer::castParameters(int parameterIndex, float value)
{
    float parameterValue;

    switch (parameterIndex)
    {
        case DIVISION:
            parameterValue = round(value);
            break;
        case OCTAVESPREAD:
            parameterValue = round(value);
            break;
        case VELOCITYMODE:
            parameterValue = round(value);
            break;
        case VELOCITYCURVE:
            parameterValue = round(value);
            break;
        case CURVECLIP:
            parameterValue = round(value);
            break;
        case PATTERNVEL1:
            parameterValue = round(value);
            break;
        case PATTERNVEL2:
            parameterValue = round(value);
            break;
        case PATTERNVEL3:
            parameterValue = round(value);
            break;
        case PATTERNVEL4:
            parameterValue = round(value);
            break;
        case PATTERNVEL5:
            parameterValue = round(value);
            break;
        case PATTERNVEL6:
            parameterValue = round(value);
            break;
        case PATTERNVEL7:
            parameterValue = round(value);
            break;
        case PATTERNVEL8:
            parameterValue = round(value);
            break;
        default:
            parameterValue = value;
            break;
    }

    return parameterValue;
}

void Sequencer::setParameters()
{
    for (unsigned p = 1; p < 18; p++) {
        variables[p] = getModulatableParameters(p);
    }

    clock.setDivision((int)variables[1]);
    velocityHandler->setCurveFrequency(clock.getFrequency());
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

    int param = connectLfo1;
    if (param != prevParam1) {
        prevModulation1 = 0.0;
        prevParam1 = param;
    }
    if (param > 0) {
        setParameterRanges(param);
        variables[param]    -= prevModulation1;
        float lfoValue       = parameterRange.max * lfo1Depth * (lfo1Value * 0.002);
        float modulatedValue = variables[param] + lfoValue;
        modulatedValue       = castParameters(param, applyRange(modulatedValue, parameterRange.min, parameterRange.max));
        prevModulation1      = modulatedValue - variables[param];
        setRequestValueChange(parameterIndex[param], modulatedValue);
    }
    param = connectLfo2;
    if (param != prevParam2) {
        prevModulation2 = 0.0;
        prevParam2 = param;
    }
    if (param > 0) {
        variables[param]    -= prevModulation2;
        float lfoValue       = parameterRange.max * lfo1Depth * (lfo1Value * 0.1);
        float modulatedValue = variables[param] + lfoValue;
        modulatedValue       = castParameters(param, applyRange(modulatedValue, parameterRange.min, parameterRange.max));
        prevModulation2      = modulatedValue - variables[param];
        setRequestValueChange(parameterIndex[param], modulatedValue);
    }
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

int Sequencer::getMetaSpeed() const
{
    return metaSpeed;
}

bool Sequencer::getPanic() const
{
    return panic;
}

bool Sequencer::getEnabled() const
{
    return sequencerEnabled;
}

float Sequencer::getRequestValueChangeValue(int index) const
{
    return bufferedRequests[1][index];
}

int Sequencer::getRequestValueChangeParameterIndex(int index) const
{
    return (int)bufferedRequests[0][index];
}

int Sequencer::getNumRequests() const
{
    return requestIndex;
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
    notePlayed = 0;
    firstNote = false;

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

    memset(transposeRecLength, 0, sizeof(transposeRecLength));
    memset(transposeIndex, 0, sizeof(transposeIndex));

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

    requestIndex = 0;

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


    for (unsigned s = 0; s < n_frames; s++) {

        clock.tick();
        velocityHandler->process();

        if (clock.getGate()) {

            if (playing && numActiveNotes > 0 ) {

                if (midiNotes[notePlayed][MIDI_NOTE] > 0
                        && midiNotes[notePlayed][MIDI_NOTE] < 128
                        && midiNotes[notePlayed][NOTE_TYPE] > 0)
                {

                    if (metaRecorder->recordingQued()) {
                        for (int t = 0; t < metaRecorder->getNumTakes(); t++) {
                            for (int e = 0; e < metaRecorder->getRecLength(t); e++) {
                                transposeRecording[t][e] = metaRecorder->getRecordedTranspose(t, e);
                            }
                            transpose = 0;
                            transposeRecLength[t] = metaRecorder->getRecLength(t);
                            transposeIndex[t] = notePlayed % transposeRecLength[t];
                            metaRecorder->setQue(false);
                        }
                    }

                    //create MIDI note on message
                    uint8_t midiNote = midiNotes[notePlayed][MIDI_NOTE];
                    uint8_t channel = midiNotes[notePlayed][MIDI_CHANNEL];
                    uint8_t noteType = midiNotes[notePlayed][NOTE_TYPE];

                    if (sequencerEnabled) {

                        unsigned speed = (metaSpeed == DOUBLE_TIME) ? 2 : 1;

                        if (metaSpeed == NORMAL_SPEED || (metaSpeed == HALF_TIME && notePlayed % 2 == 0) || metaSpeed == DOUBLE_TIME) {
                            metaTranspose = 0;
                            for (unsigned t = 0; t < (unsigned)metaRecorder->getNumTakes(); t++) {
                                if (transposeRecLength[t] > 0) { //TODO can remove this?
                                    metaTranspose += transposeRecording[t][transposeIndex[t]];
                                    transposeIndex[t] = (transposeIndex[t] + speed) % transposeRecLength[t];
                                }
                            }
                        }

                        uint8_t octave = octavePattern[octaveMode]->getStep() * 12;
                        octavePattern[octaveMode]->goToNextStep();

                        midiNote = midiNote + octave + transpose + metaTranspose;

                        metaRecorder->record(transpose);

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
