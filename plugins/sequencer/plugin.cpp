#include "plugin.hpp"
#include <iostream>

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

PluginSequencer::PluginSequencer()
    : Plugin(paramCount, 0, 0), sequencer(new Sequencer(getSampleRate()))
{
	sequencer->transmitHostInfo(0, 4, 1, 1, 120.0);
	sequencer->setSampleRate(static_cast<float>(getSampleRate()));
	sequencer->setDivision(7);
}

PluginSequencer::~PluginSequencer()
{
	delete sequencer;
	sequencer = nullptr;
}

// -----------------------------------------------------------------------
// Init

void PluginSequencer::initParameter(uint32_t index, Parameter& parameter)
{
	if (index >= paramCount) return;

	switch (index) {
		case paramNotemode:
			parameter.hints = kParameterIsAutomable | kParameterIsInteger;
			parameter.name = "Notemode";
			parameter.symbol = "notemode";
			parameter.ranges.def = 1;
			parameter.ranges.min = 0;
			parameter.ranges.max = 2;
			parameter.enumValues.count = 3;
			parameter.enumValues.restrictedMode = true;
			{
				ParameterEnumerationValue* const channels = new ParameterEnumerationValue[13];
				parameter.enumValues.values = channels;
				channels[0].label = "Rest";
				channels[0].value = 0;
				channels[1].label = "Note";
				channels[1].value = 1;
				channels[2].label = "Tie";
				channels[2].value = 2;
			}
			break;
		case paramMode:
			parameter.hints = kParameterIsAutomable | kParameterIsInteger;
			parameter.name = "Mode";
			parameter.symbol = "mode";
			parameter.ranges.def = 1;
			parameter.ranges.min = 0;
			parameter.ranges.max = 6;
			parameter.enumValues.count = 7;
			parameter.enumValues.restrictedMode = true;
			{
				ParameterEnumerationValue* const channels = new ParameterEnumerationValue[7];
				parameter.enumValues.values = channels;
				channels[0].label = "Clear All";
				channels[0].value = 0;
				channels[1].label = "Stop";
				channels[1].value = 1;
				channels[2].label = "Record";
				channels[2].value = 2;
				channels[3].label = "Play";
				channels[3].value = 3;
				channels[4].label = "Record Overwrite";
				channels[4].value = 4;
				channels[5].label = "Record Append";
				channels[5].value = 5;
				channels[6].label = "Undo Last";
				channels[6].value = 6;
			}
			break;
		case paramDivision:
			parameter.hints = kParameterIsAutomable | kParameterIsInteger;
			parameter.name = "Divison";
			parameter.symbol = "Divisons";
			parameter.ranges.def = 9;
			parameter.ranges.min = 0;
			parameter.ranges.max = 12;
			parameter.enumValues.count = 13;
			parameter.enumValues.restrictedMode = true;
			{
				ParameterEnumerationValue* const channels = new ParameterEnumerationValue[13];
				parameter.enumValues.values = channels;
				channels[0].label = "1/1";
				channels[0].value = 0;
				channels[1].label = "1/2";
				channels[1].value = 1;
				channels[2].label = "1/3";
				channels[2].value = 2;
				channels[3].label = "1/4";
				channels[3].value = 3;
				channels[4].label = "1/4.";
				channels[4].value = 4;
				channels[5].label = "1/4T";
				channels[5].value = 5;
				channels[6].label = "1/8";
				channels[6].value = 6;
				channels[7].label = "1/8.";
				channels[7].value = 7;
				channels[8].label = "1/8T";
				channels[8].value = 8;
				channels[9].label = "1/16";
				channels[9].value = 9;
				channels[10].label = "1/16.";
				channels[10].value = 10;
				channels[11].label = "1/16T";
				channels[11].value = 11;
				channels[12].label = "1/32";
				channels[12].value = 12;
			}
			break;
		case paramNoteLength:
			parameter.hints      = kParameterIsAutomable;
			parameter.name       = "Note Length";
			parameter.symbol     = "noteLength";
			parameter.unit       = "";
			parameter.ranges.def = 0.8f;
			parameter.ranges.min = 0.f;
			parameter.ranges.max = 1.f;
			break;
		case paramOctaveSpread:
			parameter.hints = kParameterIsAutomable | kParameterIsInteger;
			parameter.name = "Octave Spread";
			parameter.symbol = "octaveSpread";
			parameter.ranges.def = 1;
			parameter.ranges.min = 1;
			parameter.ranges.max = 4;
			parameter.enumValues.count = 4;
			parameter.enumValues.restrictedMode = true;
			{
				ParameterEnumerationValue* const channels = new ParameterEnumerationValue[4];
				parameter.enumValues.values = channels;
				channels[0].label = "1 oct";
				channels[0].value = 1;
				channels[1].label = "2 oct";
				channels[1].value = 2;
				channels[2].label = "3 oct";
				channels[2].value = 3;
				channels[3].label = "4 oct";
				channels[3].value = 4;
			}
			break;
		case paramPlaymode:
			parameter.hints = kParameterIsAutomable | kParameterIsInteger;
			parameter.name = "Play Mode";
			parameter.symbol = "playMode";
			parameter.ranges.def = 1;
			parameter.ranges.min = 0;
			parameter.ranges.max = 2;
			parameter.enumValues.count = 3;
			parameter.enumValues.restrictedMode = true;
			{
				ParameterEnumerationValue* const channels = new ParameterEnumerationValue[3];
				parameter.enumValues.values = channels;
				channels[0].label = "Momentary";
				channels[0].value = 0;
				channels[1].label = "Latch-Transpose";
				channels[1].value = 1;
				channels[2].label = "Latch-PlayNotes";
				channels[2].value = 2;
			}
			break;
		case paramSwing:
			parameter.hints      = kParameterIsAutomable;
			parameter.name       = "Swing";
			parameter.symbol     = "swing";
			parameter.unit       = "";
			parameter.ranges.def = 0.f;
			parameter.ranges.min = -1.0f;
			parameter.ranges.max = 1.0f;
			break;
		case paramRandomizeTiming:
			parameter.hints      = kParameterIsAutomable;
			parameter.name       = "Randomize Timing";
			parameter.symbol     = "randomizeTiming";
			parameter.unit       = "";
			parameter.ranges.def = 0.f;
			parameter.ranges.min = 0.f;
			parameter.ranges.max = 1.f;
			break;
		case paramVelocityMode:
			parameter.hints = kParameterIsAutomable | kParameterIsInteger;
			parameter.name = "Velocity Mode";
			parameter.symbol = "velocityMode";
			parameter.ranges.def = 0;
			parameter.ranges.min = 0;
			parameter.ranges.max = 2;
			parameter.enumValues.count = 3;
			parameter.enumValues.restrictedMode = true;
			{
				ParameterEnumerationValue* const channels = new ParameterEnumerationValue[3];
				parameter.enumValues.values = channels;
				channels[0].label = "None";
				channels[0].value = 0;
				channels[1].label = "Velocity Curve";
				channels[1].value = 1;
				channels[2].label = "Velocity Pattern";
				channels[2].value = 2;
			}
			break;
		case paramVelocityCurve:
			parameter.hints      = kParameterIsAutomable | kParameterIsInteger;
			parameter.name       = "Velocity Curve";
			parameter.symbol     = "velocityCurve";
			parameter.unit       = "";
			parameter.ranges.def = 0.f;
			parameter.ranges.min = 0.f;
			parameter.ranges.max = 70.f;
			break;
		case paramCurveDepth:
			parameter.hints      = kParameterIsAutomable;
			parameter.name       = "Curve Depth";
			parameter.symbol     = "curveDepth";
			parameter.unit       = "";
			parameter.ranges.def = 0.f;
			parameter.ranges.min = 0.f;
			parameter.ranges.max = 1.f;
			break;
		case paramCurveClip:
			parameter.hints      = kParameterIsAutomable | kParameterIsBoolean;
			parameter.name       = "Curve Clip";
			parameter.symbol     = "curveClip";
			parameter.unit       = "";
			parameter.ranges.def = 0.f;
			parameter.ranges.min = 0.f;
			parameter.ranges.max = 1.f;
			break;
		case paramCurveLength:
			parameter.hints      = kParameterIsAutomable | kParameterIsInteger;
			parameter.name       = "Curve Length";
			parameter.symbol     = "curveLength";
			parameter.unit       = "";
			parameter.ranges.def = 3;
			parameter.ranges.min = 1;
			parameter.ranges.max = 16;
			break;
		case paramPatternlength:
			parameter.hints      = kParameterIsAutomable | kParameterIsInteger;
			parameter.name       = "Pattern length";
			parameter.symbol     = "patternLength";
			parameter.unit       = "";
			parameter.ranges.def = 1;
			parameter.ranges.min = 1;
			parameter.ranges.max = 8;
			break;
		case paramVelocityNote1:
			parameter.hints      = kParameterIsAutomable | kParameterIsInteger;
			parameter.name       = "Velocity note 1";
			parameter.symbol     = "velocityNote1";
			parameter.unit       = "";
			parameter.ranges.def = 100;
			parameter.ranges.min = 0;
			parameter.ranges.max = 127;
			break;
		case paramVelocityNote2:
			parameter.hints      = kParameterIsAutomable | kParameterIsInteger;
			parameter.name       = "Velocity note 2";
			parameter.symbol     = "velocityNote2";
			parameter.unit       = "";
			parameter.ranges.def = 100;
			parameter.ranges.min = 0;
			parameter.ranges.max = 127;
			break;
		case paramVelocityNote3:
			parameter.hints      = kParameterIsAutomable | kParameterIsInteger;
			parameter.name       = "Velocity note 3";
			parameter.symbol     = "velocityNote3";
			parameter.unit       = "";
			parameter.ranges.def = 100;
			parameter.ranges.min = 0;
			parameter.ranges.max = 127;
			break;
		case paramVelocityNote4:
			parameter.hints      = kParameterIsAutomable | kParameterIsInteger;
			parameter.name       = "Velocity note 4";
			parameter.symbol     = "velocityNote4";
			parameter.unit       = "";
			parameter.ranges.def = 100;
			parameter.ranges.min = 0;
			parameter.ranges.max = 127;
			break;
		case paramVelocityNote5:
			parameter.hints      = kParameterIsAutomable | kParameterIsInteger;
			parameter.name       = "Velocity note 5";
			parameter.symbol     = "velocityNote5";
			parameter.unit       = "";
			parameter.ranges.def = 100;
			parameter.ranges.min = 0;
			parameter.ranges.max = 127;
			break;
		case paramVelocityNote6:
			parameter.hints      = kParameterIsAutomable | kParameterIsInteger;
			parameter.name       = "Velocity note 6";
			parameter.symbol     = "velocityNote6";
			parameter.unit       = "";
			parameter.ranges.def = 100;
			parameter.ranges.min = 0;
			parameter.ranges.max = 127;
			break;
		case paramVelocityNote7:
			parameter.hints      = kParameterIsAutomable | kParameterIsInteger;
			parameter.name       = "Velocity note 7";
			parameter.symbol     = "velocityNote7";
			parameter.unit       = "";
			parameter.ranges.def = 100;
			parameter.ranges.min = 0;
			parameter.ranges.max = 127;
			break;
		case paramVelocityNote8:
			parameter.hints      = kParameterIsAutomable | kParameterIsInteger;
			parameter.name       = "Velocity note 8";
			parameter.symbol     = "velocityNote8";
			parameter.unit       = "";
			parameter.ranges.def = 100;
			parameter.ranges.min = 0;
			parameter.ranges.max = 127;
			break;
		case paramConnectLfo1:
			parameter.hints = kParameterIsAutomable | kParameterIsInteger;
			parameter.name = "LFO 1 Destination";
			parameter.symbol = "lfo1Destination";
			parameter.ranges.def = 0;
			parameter.ranges.min = 0;
			parameter.ranges.max = 17;
			parameter.enumValues.count = 18;
			parameter.enumValues.restrictedMode = true;
			{
				ParameterEnumerationValue* const channels = new ParameterEnumerationValue[18];
				parameter.enumValues.values = channels;
				channels[0].label = "none";
				channels[0].value = 0;
				channels[1].label = "division";
				channels[1].value = 1;
				channels[2].label = "noteLengthParam";
				channels[2].value = 2;
				channels[3].label = "octaveSpread";
				channels[3].value = 3;
				channels[4].label = "swing";
				channels[4].value = 4;
				channels[5].label = "randomizeTimming";
				channels[5].value = 5;
				channels[6].label = "velocityMode";
				channels[6].value = 6;
				channels[7].label = "velocityCurve"   ;
				channels[7].value = 7;
				channels[8].label = "curveDepth";
				channels[8].value = 8;
				channels[9].label = "curveClip";
				channels[9].value = 9;
				channels[10].label = "patternVel1";
				channels[10].value = 10;
				channels[11].label = "patternVel2";
				channels[11].value = 11;
				channels[12].label = "patternVel3";
				channels[12].value = 12;
				channels[13].label = "patternVel4";
				channels[13].value = 13;
				channels[14].label = "patternVel5";
				channels[14].value = 14;
				channels[15].label = "patternVel6";
				channels[15].value = 15;
				channels[16].label = "patternVel7";
				channels[16].value = 16;
				channels[17].label = "patternVel8";
				channels[17].value = 17;
			}
			break;
		case paramLFO1depth:
			parameter.hints = kParameterIsAutomable;
			parameter.name       = "LFO 1 Depth";
			parameter.symbol     = "lfo1Depth";
			parameter.unit       = "";
			parameter.ranges.def = 0.f;
			parameter.ranges.min = 0.f;
			parameter.ranges.max = 100.f;
			break;
		case paramConnectLfo2:
			parameter.hints = kParameterIsAutomable | kParameterIsInteger;
			parameter.name = "LFO 2 Destination";
			parameter.symbol = "lfo2Destination";
			parameter.ranges.def = 0;
			parameter.ranges.min = 0;
			parameter.ranges.max = 17;
			parameter.enumValues.count = 18;
			parameter.enumValues.restrictedMode = true;
			{
				ParameterEnumerationValue* const channels = new ParameterEnumerationValue[18];
				parameter.enumValues.values = channels;
				channels[0].label = "none";
				channels[0].value = 0;
				channels[1].label = "division";
				channels[1].value = 1;
				channels[2].label = "noteLengthParam";
				channels[2].value = 2;
				channels[3].label = "octaveSpread";
				channels[3].value = 3;
				channels[4].label = "swing";
				channels[4].value = 4;
				channels[5].label = "randomizeTimming";
				channels[5].value = 5;
				channels[6].label = "velocityMode";
				channels[6].value = 6;
				channels[7].label = "velocityCurve"   ;
				channels[7].value = 7;
				channels[8].label = "curveDepth";
				channels[8].value = 8;
				channels[9].label = "curveClip";
				channels[9].value = 9;
				channels[10].label = "patternVel1";
				channels[10].value = 10;
				channels[11].label = "patternVel2";
				channels[11].value = 11;
				channels[12].label = "patternVel3";
				channels[12].value = 12;
				channels[13].label = "patternVel4";
				channels[13].value = 13;
				channels[14].label = "patternVel5";
				channels[14].value = 14;
				channels[15].label = "patternVel6";
				channels[15].value = 15;
				channels[16].label = "patternVel7";
				channels[16].value = 16;
				channels[17].label = "patternVel8";
				channels[17].value = 17;
			}
			break;
		case paramLFO2depth:
			parameter.hints      = kParameterIsAutomable;
			parameter.name       = "LFO 2 Depth";
			parameter.symbol     = "lfo2Depth";
			parameter.unit       = "";
			parameter.ranges.def = 0.f;
			parameter.ranges.min = 0.f;
			parameter.ranges.max = 100.f;
			break;
		case paramPanic:
			parameter.hints      = kParameterIsBoolean | kParameterIsTrigger;
			parameter.name       = "Panic";
			parameter.symbol     = "panic";
			parameter.unit       = "";
			parameter.ranges.def = 0;
			parameter.ranges.min = 0;
			parameter.ranges.max = 1;
			break;
		case paramEnabled:
			parameter.hints      = kParameterIsBoolean;
			parameter.name       = "Enabled";
			parameter.symbol     = "enabled";
			parameter.unit       = "";
			parameter.ranges.def = 0.f;
			parameter.ranges.min = 0.f;
			parameter.ranges.max = 1.f;
			break;
	}
}

// -----------------------------------------------------------------------
// Internal data

/**
  Optional callback to inform the plugin about a sample rate change.
*/
void PluginSequencer::sampleRateChanged(double newSampleRate)
{
    (void) newSampleRate;

	sequencer->setSampleRate(static_cast<float>(newSampleRate));
}

/**
  Get the current value of a parameter.
*/
float PluginSequencer::getParameterValue(uint32_t index) const
{
	switch (index)
	{
		case paramNotemode:
			return sequencer->getNotemode();
		case paramMode:
			return sequencer->getMode();
		case paramDivision:
			return sequencer->getDivision();
		case paramNoteLength:
			return sequencer->getNoteLength();
		case paramOctaveSpread:
			return sequencer->getOctaveSpread();
		case paramPlaymode:
			return sequencer->getPlaymode();
		case paramSwing:
			return sequencer->getSwing();
		case paramRandomizeTiming:
			return sequencer->getRandomizeTiming();
		case paramVelocityMode:
			return sequencer->getVelocityMode();
		case paramVelocityCurve:
			return sequencer->getVelocityCurve();
		case paramCurveDepth:
			return sequencer->getCurveDepth();
		case paramCurveClip:
			return sequencer->getCurveClip();
		case paramCurveLength:
			return sequencer->getCurveLength();
		case paramPatternlength:
			return sequencer->getPatternlength();
		case paramVelocityNote1:
			return sequencer->getVelocityNote(0);
		case paramVelocityNote2:
			return sequencer->getVelocityNote(1);
		case paramVelocityNote3:
			return sequencer->getVelocityNote(2);
		case paramVelocityNote4:
			return sequencer->getVelocityNote(3);
		case paramVelocityNote5:
			return sequencer->getVelocityNote(4);
		case paramVelocityNote6:
			return sequencer->getVelocityNote(5);
		case paramVelocityNote7:
			return sequencer->getVelocityNote(6);
		case paramVelocityNote8:
			return sequencer->getVelocityNote(7);
		case paramConnectLfo1:
			return sequencer->getConnectLfo1();
		case paramLFO1depth:
			return sequencer->getLFO1depth();
		case paramConnectLfo2:
			return sequencer->getConnectLfo2();
		case paramLFO2depth:
			return sequencer->getLFO2depth();
		case paramPanic:
			return sequencer->getPanic();
		case paramEnabled:
			return sequencer->getEnabled();
	}
}

/**
  Change a parameter value.
*/
void PluginSequencer::setParameterValue(uint32_t index, float value)
{
	switch (index)
	{
		case paramNotemode:
			sequencer->setNotemode(value);
			break;
		case paramMode:
			sequencer->setMode(value);
			break;
		case paramDivision:
			sequencer->setDivision(value);
			break;
		case paramNoteLength:
			sequencer->setNoteLength(value);
			break;
		case paramOctaveSpread:
			sequencer->setOctaveSpread(value);
			break;
		case paramPlaymode:
			sequencer->setPlaymode(value);
			break;
		case paramSwing:
			sequencer->setSwing(value);
			break;
		case paramRandomizeTiming:
			sequencer->setRandomizeTiming(value);
			break;
		case paramVelocityMode:
			sequencer->setVelocityMode(value);
			break;
		case paramVelocityCurve:
			sequencer->setVelocityCurve(value);
			break;
		case paramCurveDepth:
			sequencer->setCurveDepth(value);
			break;
		case paramCurveClip:
			sequencer->setCurveClip(value);
			break;
		case paramCurveLength:
			sequencer->setCurveLength(value);
			break;
		case paramPatternlength:
			sequencer->setPatternlength(value);
			break;
		case paramVelocityNote1:
			sequencer->setVelocityNote(0, value);
			break;
		case paramVelocityNote2:
			sequencer->setVelocityNote(1, value);
			break;
		case paramVelocityNote3:
			sequencer->setVelocityNote(2, value);
			break;
		case paramVelocityNote4:
			sequencer->setVelocityNote(3, value);
			break;
		case paramVelocityNote5:
			sequencer->setVelocityNote(4, value);
			break;
		case paramVelocityNote6:
			sequencer->setVelocityNote(6, value);
			break;
		case paramVelocityNote7:
			sequencer->setVelocityNote(6, value);
			break;
		case paramVelocityNote8:
			sequencer->setVelocityNote(7, value);
			break;
		case paramConnectLfo1:
			sequencer->setConnectLfo1(value);
			break;
		case paramLFO1depth:
			sequencer->setLFO1depth(value);
			break;
		case paramConnectLfo2:
			sequencer->setConnectLfo2(value);
			break;
		case paramLFO2depth:
			sequencer->setLFO1depth(value);
			break;
		case paramPanic:
			sequencer->setPanic(value);
			break;
		case paramEnabled:
			sequencer->setEnabled(value);
			break;
	}
}

// -----------------------------------------------------------------------
// Process

void PluginSequencer::activate()
{
    // plugin is activated
}

void PluginSequencer::run(const float**, float**, uint32_t n_frames,
                              const MidiEvent* events, uint32_t eventCount)
{
	sequencer->emptyMidiBuffer();

	// Check if host supports Bar-Beat-Tick position
	const TimePosition& position = getTimePosition();
	if (!position.bbt.valid) return;
	sequencer->transmitHostInfo(position.playing, position.bbt.beatsPerBar, position.bbt.beat, position.bbt.barBeat, static_cast<float>(position.bbt.beatsPerMinute));

	sequencer->process(events, eventCount, n_frames);

	struct MidiBuffer buffer = sequencer->getMidiBuffer();
	for (unsigned x = 0; x < buffer.numBufferedEvents + buffer.numBufferedThroughEvents; x++) {
		writeMidiEvent(buffer.bufferedEvents[x]); //needs to be one struct or array?
	}
}

// -----------------------------------------------------------------------

Plugin* createPlugin()
{
    return new PluginSequencer();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
