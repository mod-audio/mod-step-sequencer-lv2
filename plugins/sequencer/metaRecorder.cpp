#include "metaRecorder.hpp"
#include <iostream>

MetaRecorder::MetaRecorder(uint32_t *period, uint32_t *clockPos, int *division) :
beatPos(0.0),
recMode(0),
recLength(0)
{
	this->period   = period;
	this->clockPos = clockPos;
	this->division = division;
}

MetaRecorder::~MetaRecorder()
{
	delete period;
	period = nullptr;
	delete clockPos;
	clockPos = nullptr;
	delete division;
	division = nullptr;
}

bool MetaRecorder::recordingQued()
{
	return recQued;
}

int MetaRecorder::getRecordingMode() const
{
	return recMode;
}

uint8_t MetaRecorder::getMidiBuffer(int index, int noteType)
{
	return midiBuffer[index][noteType];
}

void MetaRecorder::setQue(bool recQued)
{
	this->recQued = recQued;
}

int MetaRecorder::getRecLength()
{
	return recLength;
}

void MetaRecorder::calculateQuantizeCoef()
{
	coef = pow((0.5 + (*division / 24)) * 0.707107, 2);
}

void MetaRecorder::clearRecording()
{
	recIndex = 0;
	recLength = 0;
}

void MetaRecorder::setRecordingMode(int recMode)
{
	this->recMode = recMode;
}

void MetaRecorder::record(uint8_t midiNote, uint8_t channel, uint8_t noteType)
{

	preRecordMidiBuffer[MIDI_NOTE]    = midiNote;
	preRecordMidiBuffer[MIDI_CHANNEL] = channel;
	preRecordMidiBuffer[NOTE_TYPE]    = noteType;

	if (recMode != prevRecMode && (recIndex % 2 == 0))
	{
		switch (recMode)
		{
			case STOP_RECORDING:
				if (recording && recIndex > 0)
				{
					recLength = recIndex;
					recQued = true;
					recording = false;
					recIndex = 0;
				}
				break;
			case START_RECORDING:
				recording = true;
				if (*clockPos < *period - (*period * coef)) {
					midiBuffer[recIndex][MIDI_NOTE]    = preRecordMidiBuffer[MIDI_NOTE];
					midiBuffer[recIndex][MIDI_CHANNEL] = preRecordMidiBuffer[MIDI_CHANNEL];
					midiBuffer[recIndex][NOTE_TYPE]    = preRecordMidiBuffer[NOTE_TYPE];
					recIndex = (recIndex + 1) % BUFFER_LENGTH;
				}
				break;
		}
		prevRecMode = recMode;
	}

	if (recording) {
		midiBuffer[recIndex][MIDI_NOTE]    = midiNote;
		midiBuffer[recIndex][MIDI_CHANNEL] = channel;
		midiBuffer[recIndex][NOTE_TYPE]    = noteType;

		recIndex = (recIndex + 1) % BUFFER_LENGTH;
	}
}
