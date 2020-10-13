#ifndef _H_SEQUENCER_
#define _H_SEQUENCER_

#include <cstdint>

#include "../../common/clock.hpp"
#include "../../common/pattern.hpp"
#include "../../common/midiHandler.hpp"
#include "utils.hpp"
#include "velocityhandler.hpp"


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
        PLAY_LATCH_TRANSPOSE
	};
	enum noteTypes {
        REST = 0,
        NOTE,
        TIE,
	};
	Sequencer(double sampleRate);
	~Sequencer();

	void setSampleRate(float sampleRate);
	void setNotemode(int value);
	void setMode(int value);
	void setDivision(int value);
	void setNoteLength(float value);
	void setOctaveSpread(int value);
	void setPlaymode(int value);
	void setSwing(float value);
	void setRandomizeTiming(float value);
	void setVelocityMode(int value);
	void setVelocityCurve(float value);
	void setCurveDepth(float value);
	void setCurveClip(bool value);
	void setCurveLength(int value);
	void setPatternlength(int value);
	void setVelocityNote(int index, int value);
	void setConnectLfo1(int value);
	void setLFO1depth(float value);
	void setConnectLfo2(int value);
	void setLFO2depth(float value);
	void setPanic(bool value);
	void setEnabled(bool value);
	int getNotemode() const;
	int getMode() const;
	int getDivision() const;
	float getNoteLength() const;
	int getOctaveSpread() const;
	int getPlaymode() const;
	float getSwing() const;
	float getRandomizeTiming() const;
	int getVelocityMode() const;
	float getVelocityCurve() const;
	float getCurveDepth() const;
	bool getCurveClip() const;
	int getCurveLength() const;
	int getPatternlength() const;
	int getVelocityNote(int index) const;
	int getConnectLfo1() const;
	float getLFO1depth() const;
	int getConnectLfo2() const;
	float getLFO2depth() const;
	bool getPanic() const;
	bool getEnabled() const;

	void transmitHostInfo(const bool playing, const float beatsPerBar,
	const int beat, const float barBeat, const double bpm);
	void reset();
	void clear();
	void emptyMidiBuffer();
	struct MidiBuffer getMidiBuffer();
	void process(const MidiEvent* event, uint32_t eventCount, uint32_t n_frames);
private:

	int notesPressed = 0;
	int activeNotes = 0;
	int notePlayed = 0;
	int octaveMode = 0;

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
	int activeNotesBypassed = 0;
	int timeOutTime = 5000;
	int firstNoteTimer = 0;
	int numActiveNotes = 0;
	float barBeat;
	float sampleRate = 48000;

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

	int overwriteIndex = 0;
	int noteMode = 0;
	int mode = 0;
	int division = 0;
	float noteLength = 0.8;
	int octaveSpread = 1;
	int playMode = 0;
	float swing = 0.5;
	int randomizeTiming = 0;
	int velocityMode = 0;
	int velocityCurve = 0;
	float curveDepth = 0.0;
	int curveClip = 0;
	int curveLength = 1;
	int patternlength = 1;
	int velocityNote[8];
	int connectLfo1 = 0;
	int lFO1depth = 0;
	int connectLfo2 = 0;
	int lFO2depth = 0;
	bool sequencerEnabled = true;
	bool panic = false;

	bool recording = false;
	bool playing = false;

	SeqUtils utils;
	Pattern **octavePattern;
	MidiHandler midiHandler;
	PluginClock clock;
	VelocityHandler *velocityHandler;
};

#endif // _H_SEQUENCER_
