/* Arduino Synth from
  https://janostman.wordpress.com/2016/01/15/how-to-build-your-very-own-string-synth/
*/

/* Original code by Jan Ostman,  Modified by Dave Field of Take The Moon
 *
        https://moroccodave.com/
        https://takethemoon.com/

    Modifications are licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
    To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/4.0/.

    MODS:
    *****

    1. Keyboard port scanning removed - this version is MIDI-only
    2. Potentiometer inputs assigned to lower analog pins for ATMega328 compatibility
    3. DIP switch on digital pins 6, 7, 8, 9 added to allow MIDI channel selection
    4. MIDI handler re-written to use stock MIDI library and to be channel-aware
    5. Any resulting redundant code and variables removed

    Also modified by LAURI'S DIY CHANNEL TV: There's a control for clock rate / pitch and lfo rate

*/

// ANALYSIS OF JAN OSTMAN's SOLINA

// generate 8 saw waves and distribute them among the keys played on keyboard
// I THINK EACH ONE IS A PAIR OF DETUNED OSCILLATORS

// paraphonic - envelope is shared by all voices

// 3 pots: modulation, ensemble (phaser) and envelope

// envelope uses a preset contour where one knob sets a,d,s, and r

// phaser: delay line of 50ms where the delay time is modulated by an lfo
//     - pot controls how much of phaser signal is mixed into dry signal

// LFO
//  - one for pitch, one for phaser delay

// inputs

// one knob controls mod and detune
// one knob for phaser mix
// one knob for adsr