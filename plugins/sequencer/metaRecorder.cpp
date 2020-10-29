#include "metaRecorder.hpp"
#include <iostream>

MetaRecorder::MetaRecorder() :
beatPos(0.0),
recMode(0),
recLength(0)
{
}

MetaRecorder::~MetaRecorder()
{
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

void MetaRecorder::setBeatPos(float beatPos)
{
	this->beatPos = beatPos;
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
				}
				break;
			case START_RECORDING:
				recording = true;
				break;
		}
		prevRecMode = recMode;
	}

	if (recording) {
		midiBuffer[recIndex][MIDI_NOTE] = midiNote;
		midiBuffer[recIndex][MIDI_CHANNEL] = channel;
		midiBuffer[recIndex][NOTE_TYPE] = 1;

		recIndex = (recIndex + 1) % BUFFER_LENGTH;
	}
}
