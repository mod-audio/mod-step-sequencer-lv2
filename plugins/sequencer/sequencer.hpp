#ifndef _H_SEQUENCER_
#define _H_SEQUENCER_

#include <cstdint>

#include "../../common/clock.hpp"
#include "../../common/pattern.hpp"
#include "../../common/midiHandler.hpp"
#include "utils.hpp"
#include "velocityhandler.hpp"
#include "metaRecorder.hpp"

#define NUM_VOICES 200
#define NUM_NOTE_OFF_SLOTS 200
#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/sequencer"

#define MIDI_NOTEOFF 0x80
#define MIDI_NOTEON  0x90

#define MIDI_NOTE 0
#define MIDI_CHANNEL 1
#define NOTE_TYPE 2
#define TIMER 3

#define NUM_ARP_MODES 6
#define NUM_OCTAVE_MODES 5
#define NUM_STATES 40

#define NUM_MIDI_CHANNELS 16 //TODO check how many are needed

//#define MIDI_BUFFER_SIZE 300

#define ONE_OCT_UP_PER_CYCLE 4

class Sequencer {
public:
	enum SeqModes {
		ARP_UP = 0,
		ARP_DOWN,
		ARP_UP_DOWN,
		ARP_UP_DOWN_ALT,
		ARP_PLAYED,
		ARP_RANDOM
	};
	enum SeqStates {
        STATE_CLEAR_ALL = 0,
        STATE_STOP,
        STATE_RECORD,
        STATE_PLAY,
        STATE_RECORD_OVERWRITE,
        STATE_RECORD_APPEND,
        STATE_UNDO_LAST
	};
	enum playModes {
        PLAY_MOMENTARY = 0,
        PLAY_LATCH_TRANSPOSE,
        PLAY_NOTES
	};
	enum noteTypes {
        REST = 0,
        NOTE,
        TIE
	};
	enum metaMode {
        START_RECORDING_NOTE_ON = 0,
        START_RECORDING_WHEN_ENABLED
	};
	enum metaSpeed {
        HALF_TIME = 0,
        NORMAL_SPEED,
        DOUBLE_TIME
	};
	Sequencer(double sampleRate);
	~Sequencer();

	void  setSampleRate(float sampleRate);
	void  setNotemode(int value);
	void  setMode(int value);
	void  setDivision(int value);
	void  setQuantizeMode(float value);
	void  setNoteLength(float value);
	void  setOctaveSpread(int value);
	void  setMaxLength(int value);
	void  setPlaymode(int value);
	void  setModulatableParameters(const float value, int index);
	void  setSwing(float value);
	void  setRandomizeTiming(float value);
	void  setVelocityMode(int value);
	void  setVelocityCurve(float value);
	void  setCurveDepth(float value);
	void  setCurveClip(bool value);
	void  setCurveLength(int value);
	void  setPatternlength(int value);
	void  setVelocityNote(int index, int value);
	void  setConnectLfo1(int value);
	void  setLFO1depth(float value);
	void  setConnectLfo2(int value);
	void  setLFO2depth(float value);
	void  setMetaRecord(bool value);
	void  setMetaMode(int value);
	void  setMetaQuantizeValue(int value);
	void  setMetaSpeed(int value);
	void  setPanic(bool value);
	void  setEnabled(bool value);
	void  setRequestValueChange(int index, float value);
	float applyRange(float numberToCheck, float min, float max);
	void  setParameters();
	float getModulatableParameters(int index) const;
	int   getNotemode() const;
	int   getMode() const;
	int   getDivision() const;
	float getQuantizeMode() const;
	float getNoteLength() const;
	int   getOctaveSpread() const;
	int   getMaxLength() const;
	int   getPlaymode() const;
	float getSwing() const;
	float getRandomizeTiming() const;
	int   getVelocityMode() const;
	float getVelocityCurve() const;
	float getCurveDepth() const;
	bool  getCurveClip() const;
	int   getCurveLength() const;
	int   getPatternlength() const;
	int   getVelocityNote(int index) const;
	int   getConnectLfo1() const;
	float getLFO1depth() const;
	int   getConnectLfo2() const;
	float getLFO2depth() const;
	bool  getMetaRecord() const;
	int   getMetaMode() const;
	int   getMetaQuantizeValue() const;
	int   getMetaSpeed() const;
	bool  getPanic() const;
	bool  getEnabled() const;
	float getRequestValueChangeValue(int index) const;
	int   getRequestValueChangeParameterIndex(int index) const;
	int   getNumRequests() const;
	void  transmitHostInfo(const bool playing, const float beatsPerBar,
	const int beat, const float barBeat, const double bpm);
	void  reset();
	void  clear();
	void  emptyMidiBuffer();
	void  process(const float **cvInputs, const MidiEvent* event, uint32_t eventCount, uint32_t n_frames);
	struct MidiBuffer getMidiBuffer();
private:
	struct PrevState {
		uint8_t midiNotes[NUM_STATES][NUM_VOICES][3];
		int numAcitveNotes;
	};

	uint32_t* period;
	uint32_t* clockPos;
	int* division;
	float quantizeMultiplier;

	int notesPressed = 0;
	int activeNotes = 0;
	int notePlayed = 0;
	int octaveMode = 0;

	int transposeIndex[NUM_TAKES];
	int transposeRecLength[NUM_TAKES];
    uint8_t metaTranspose = 0;
    uint8_t transposeRecording[NUM_TAKES][NUM_VOICES];
    uint8_t midiNotes[NUM_VOICES][3];
    uint8_t midiNotesBypassed[NUM_VOICES];
    uint32_t noteOffBuffer[NUM_NOTE_OFF_SLOTS][4];

	int SeqMode = 0;

	uint8_t pitch = 0;
	uint8_t previousMidiNote = 0;
	uint8_t velocity = 80;
	int transpose = 0;
	int previousSyncMode = 0;
	int activeNotesIndex = 0;
	int stateIndex = 0;
	int activeNotesBypassed = 0;
	int timeOutTime = 5000;
	int firstNoteTimer = 0;
	int numActiveNotes = 0;
	float barBeat;
	float sampleRate = 48000;

	int metaSpeed = 1;
	bool metaRecordingEnabled = false;
	bool metaRecording = false;
	bool pluginEnabled = true;
	bool first = false;
	bool SeqEnabled = true;
	bool latchMode = false;
	bool previousLatch = false;
	bool latchPlaying = false;
	bool trigger = false;
	bool firstNote = false;
	bool quantizedStart = false;
	bool resetPattern = false;
	bool midiNotesCopied = false;
	bool overwrite = false;

	int metaRecordMode = 0;
	int metaQuantizeValue = 0;
	int overwriteIndex = 0;
	int noteMode = 0;
	int mode = 0;
	float noteLength = 0.8;
	int octaveSpread = 1;
	int playMode = 0;
	float swing = 0.5;
	int randomizeTiming = 0;
	int velocityMode = 0;
	int velocityCurve = 0;
	int maxLength = 9;
	float curveDepth = 0.0;
	int curveClip = 0;
	int curveLength = 1;
	int patternlength = 1;
	int velocityNote[8];
	int connectLfo1 = 0;
	int lfo1Depth = 0;
	int connectLfo2 = 0;
	int lfo2Depth = 0;
	bool sequencerEnabled = true;
	bool panic = false;
	float modulatableParameters[18];
	float variables[18];
	float prevParameter[18];
	float lfo1Value = 0.0;
	float lfo2Value = 0.0;
	float prevModulation1 = 0.0;
	float prevModulation2 = 0.0;

	bool recording = false;
	bool playing = false;
	bool paramModulated1 = false;
	bool paramModulated2 = false;
	int  requestIndex = 0;

	float bufferedRequests[2][4];

    float minParamValue[18] = {0, 0, 0, 1, 25, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};
    float maxParamValue[18] = {0, 12, 0.99, 4, 75, 1, 2, 70, 1, 0, 127, 127, 127, 127, 127, 127, 127, 127};
    float parameterIndex[18] = {0, 6, 9, 11, 13, 14, 15, 17, 18, 20, 21, 22, 23, 24, 25, 26, 27, 28};

	PrevState prevState;
	SeqUtils utils;
	Pattern **octavePattern;
	MidiHandler midiHandler;
	PluginClock clock;
	VelocityHandler *velocityHandler;
	MetaRecorder *metaRecorder;
};

#endif // _H_SEQUENCER_
