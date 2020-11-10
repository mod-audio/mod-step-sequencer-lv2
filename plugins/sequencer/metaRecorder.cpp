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
}

bool MetaRecorder::recordingQued()
{
	return recQued;
}

int MetaRecorder::getRecordingMode() const
{
	return recMode;
}

uint8_t MetaRecorder::getRecordedTranspose(int index)
{
	return recordedTranspose[index];
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


void MetaRecorder::record(uint8_t transpose)
{
	if (recMode != prevRecMode && ((recIndex % 4 == 0) || (prevRecMode && (recIndex % 4 == 1) && (*clockPos < (*period * 0.5)))))
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
					recordedTranspose[recIndex] = transpose;
					recIndex = (recIndex + 1) % BUFFER_LENGTH;
				}
				break;
		}
		prevRecMode = recMode;
	}

	if (recording) {
		recordedTranspose[recIndex] = transpose;
		recIndex = (recIndex + 1) % BUFFER_LENGTH;
	}
}
