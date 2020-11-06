# MOD Step Sequencer

This step sequencer works with live MIDI input, and is created to be used without a GUI.

### Features
* 3 note types:
	* Rest
	* Note
	* Tie
* 3 record modes
	* Record
	* Record overwrite
	* Record Append
* 2 play modes
	* Play transpose
	* Play momentary
* Host syncable with a control to set the division.
* 3 ways of controlling the velocity
	* By LFO
	* By 8 controls that represent each step, this creates a pattern of max 8 steps that are iterated over the sequencer. There is also a control to set the length of this pattern
	* Use a static velocity
* A variable note length of each step
* A parameter for applying swing.
* A option to Randomize timing
* The ability to "meta" record the current sequence, this means that things like the
  transpose will be recorded and will create a new sequence

### Still needs to be implemented
* Que events based on division. For example, it would be useful when doing a "meta"
  recording that it would round everything that is played to a certain length for
  example a full bar.
