#ifndef _H_META_RECORDER_
#define _H_META_RECORDER_

#define MIDI_NOTE 0
#define MIDI_CHANNEL 1
#define NOTE_TYPE 2

#include <cstdint>

#include "../../common/midiHandler.hpp"


#define BUFFER_LENGTH 2048 //TODO choose default

class MetaRecorder {
public:
	MetaRecorder(uint32_t *period, uint32_t *clockPos, int *division);
	MetaRecorder();
	~MetaRecorder();
	bool recordingQued();
	uint8_t getMidiBuffer(int index, int noteType);
	int getRecLength();
	int getRecordingMode() const;
	void setQue(bool recQued);
	void setBeatPos(float beatPos);
	void clearRecording();
	void calculateQuantizeCoef();
	void setRecordingMode(int mode);
	void record(uint8_t midiNote, uint8_t channel, uint8_t noteType);
private:
	enum MetaRecModes {
		STOP_RECORDING = 0,
		START_RECORDING
	};
	float coef;
	float beatPos;
	bool recording = false;
	bool newRecording = false;
	bool recQued = false;
	int recMode = 0;
	int prevRecMode = 0;
	int recLength;
	int recIndex = 0;
	uint8_t preRecordMidiBuffer[NOTE_TYPE + 1];
	uint8_t midiBuffer[BUFFER_LENGTH][NOTE_TYPE + 1];


	int *division;
	uint32_t *clockPos;
	uint32_t *period;
};

#endif //_H_META_RECORDER_
