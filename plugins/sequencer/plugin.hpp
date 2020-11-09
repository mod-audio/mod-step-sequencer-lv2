#ifndef _H_PLUGIN_SEQUENCER_
#define _H_PLUGIN_SEQUENCER_

#include "DistrhoPlugin.hpp"
#include "sequencer.hpp"
#include "../../common/clock.hpp"
#include "../../common/pattern.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

class PluginSequencer : public Plugin {
public:
	enum Parameters {
		paramNotemode = 0,
		paramMode,
		paramDivision,
		paramQuantizeMode,
		paramNoteLength,
		paramOctaveSpread,
		paramMaxLength,
		paramPlaymode,
		paramSwing,
		paramRandomizeTiming,
		paramVelocityMode,
		paramVelocityCurve,
		paramCurveDepth,
		paramCurveClip,
		paramCurveLength,
		paramPatternlength,
		paramVelocityNote1,
		paramVelocityNote2,
		paramVelocityNote3,
		paramVelocityNote4,
		paramVelocityNote5,
		paramVelocityNote6,
		paramVelocityNote7,
		paramVelocityNote8,
		paramConnectLfo1,
		paramLFO1depth,
		paramConnectLfo2,
		paramLFO2depth,
		paramMetaRecord,
		paramMetaMode,
		paramMetaQuantizeValue,
		paramPanic,
		paramEnabled,
		paramCount
	};

    PluginSequencer();
    ~PluginSequencer();

protected:
    // -------------------------------------------------------------------
    // Information

    const char* getLabel() const noexcept override {
        return "Sequencer";
    }

    const char* getDescription() const override {
        return "A MIDI sequencer";
    }

    const char* getMaker() const noexcept override {
        return "MOD";
    }

    const char* getHomePage() const override {
        return "";
    }

    const char* getLicense() const noexcept override {
        return "https://spdx.org/licenses/GPL-2.0-or-later";
    }

    uint32_t getVersion() const noexcept override {
        return d_version(1, 0, 0);
    }

    int64_t getUniqueId() const noexcept override {
        return d_cconst('M', 'O', 'S', 'Q');
    }

    // -------------------------------------------------------------------
    // Init

    void initParameter(uint32_t index, Parameter& parameter) override;

    // -------------------------------------------------------------------
    // Internal data

    float getParameterValue(uint32_t index) const override;
    void setParameterValue(uint32_t index, float value) override;

    // -------------------------------------------------------------------
    // Optional

    // Optional callback to inform the plugin about a sample rate change.
    void sampleRateChanged(double newSampleRate) override;

    // -------------------------------------------------------------------
    // Process

    void activate() override;

    void run(const float**, float**, uint32_t,
             const MidiEvent* midiEvents, uint32_t midiEventCount) override;


    // -------------------------------------------------------------------

private:
	Sequencer *sequencer;

	float fParams[paramCount];
	int active_notes[20];
	int next_note = 0;
	int Seq_counter = 0;
	int current_note = 0;
	double bpm = 120;
	double previousBpm = 0;
	double previousHostBpm = 0;
	uint8_t pitch = 0;

	bool trigger = false;
	int previousSyncMode = 0;

	DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginSequencer)
};

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif // _H_PLUGIN_SEQUENCER_
