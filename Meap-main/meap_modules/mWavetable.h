/*
 * Oscil.h
 *
 * Oscil.h owes much to AF_precision_synthesis.pde, 2009, Adrian Freed.
 *
 * Copyright 2012 Tim Barrass, 2009 Adrian Freed.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#ifndef MEAP_WAVETABLE_H
#define MEAP_WAVETABLE_H

// fractional bits for oscillator index precision
#define OSCIL_F_BITS 16 
#define OSCIL_F_BITS_AS_MULTIPLIER 65536

// phmod_proportion is an 15n16 fixed-point number
#define OSCIL_PHMOD_BITS 16

/**
Oscil plays a wavetable, cycling through the table to generate an audio or
control signal. The frequency of the signal can be set or changed with
setFreq(), and the output of an Oscil can be produced with next() for a simple
cycling oscillator, or atIndex() for a particular sample in the table.
@tparam NUM_TABLE_CELLS This is defined in the table ".h" file the Oscil will be
using. It's important that it's a power of 2, and either a literal number (eg. "8192") or a
defined macro, rather than a const or int, for the Oscil to run fast enough.
@tparam UPDATE_RATE This will be AUDIO_RATE if the Oscil is updated in
updateAudio(), or CONTROL_RATE if it's updated each time updateControl() is
called. It could also be a fraction of CONTROL_RATE if you are doing some kind
of cyclic updating in updateControl(), for example, to spread out the processor load.
@todo Use conditional compilation to optimise setFreq() variations for different table
sizes.
@note If you #define OSCIL_DITHER_PHASE before you #include <Oscil.h>,
the phase increments will be dithered, which reduces spurious frequency spurs
in the audio output, at the cost of some extra processing and memory.
@section int8_t2mozzi
Converting soundfiles for Mozzi
There is a python script called char2mozzi.py in the Mozzi/python folder.
The usage is:
char2mozzi.py infilename outfilename tablename samplerate
*/
// template <uint16_t NUM_TABLE_CELLS, uint16_t UPDATE_RATE, bool DITHER_PHASE=false>
template <uint32_t FRAME_SIZE, uint32_t NUM_FRAMES, uint16_t UPDATE_RATE, uint64_t NUM_TABLE_CELLS>
class mWavetable
{

public:
  /** Constructor.
    @param TABLE_NAME the name of the array the Oscil will be using. This
    can be found in the table ".h" file if you are using a table made for
    Mozzi by the int8_t2mozzi.py python script in Mozzi's python
    folder.*/
  mWavetable(const int16_t *TABLE_NAME)
      : table(TABLE_NAME)
  {
  }

  /** Constructor.
    Declare an Oscil with template TABLE_NUM_CELLS and UPDATE_RATE
    parameters, without specifying a particular wave table for it to play.
    The table can be set or changed on the fly with setTable(). Any tables
    used by the Oscil must be the same size.
    */
  mWavetable()
  {
  }

  /** Updates the phase according to the current frequency and returns the sample at the new phase position.
    @return the next sample.
    */
  inline int16_t next()
  {

    incrementPhase();
    // if (phase_fractional > endpos_fractional) {
    //   phase_fractional = startpos_fractional + (phase_fractional - endpos_fractional);
    // }

    // WARNNG this is hard coded for when SAMPLE_F_BITS is 16
    // uint64_t index = phase_fractional >> SAMPLE_F_BITS;

    uint64_t index = frame_offset + ((phase_fractional >> OSCIL_F_BITS) & (NUM_TABLE_CELLS - 1));

    int64_t out = FLASH_OR_RAM_READ<const int16_t>(table + index);
    int64_t difference = FLASH_OR_RAM_READ<const int16_t>((table + 1) + index) - out;

    int64_t fractional = phase_fractional & 65535; // remove integer component leaving only fraction, maybe i could just cast to uint16_t

    int64_t diff_fraction = (difference * fractional) >> 16;

    // .... alpha = (unsigned int)phase_fractional) >> 8)
    // multiply difference by fractional proportion (should between 0 and 1 i think!)
    // add result onto ut and return

    // iiii iiii iiii iiii ffff ffff ffff ffff
    // NUM_TABLE_CELLS - 1 mask
    // 0000 0000 0000 0000 0000 0111 1111 1111
    // i want
    // 0000 0111 1111 1111 0000 0000 0000 0000
    // so i just bitshift up by 16
    // uint32_t masked_phase_fractional = phase_fractional & (((uint32_t)NUM_TABLE_CELLS - 1)<<16);

    // stk interp

    // find round down index (iIndex)
    // find fractional roundoff (alpha)
    // find value at iIndex (tmp)
    // find difference between tmp and value at iIndex + 1
    // multiply difference by alpha and add to tmp
    // return tmp

    // int8_t diff_fraction = (int8_t)(((((unsigned int)phase_fractional) >> 8) * difference) >> 8);  // (unsigned int) phase_fractional keeps low word, then>> for only 8 bit precision
    out += diff_fraction;

    return out;
  }

  //   /** Returns the current sample.
  //    */
  // inline int16_t readTable() {
  //   return FLASH_OR_RAM_READ<const int16_t>(table + frame_offset + ((phase_fractional >> OSCIL_F_BITS) & (NUM_TABLE_CELLS - 1)));
  // }

  inline void setFrame(uint16_t frame_num)
  {
    if (frame_num >= NUM_FRAMES)
    {
      frame_num = NUM_FRAMES - 1;
    }
    frame_offset = FRAME_SIZE * frame_num;

    // uint64_t start = FRAME_SIZE * frame_num;
    // uint64_t end = start + FRAME_SIZE - 1;

    // uint64_t my_pos = phase_fractional - startpos_fractional;

    // startpos_fractional = (uint64_t)start << SAMPLE_F_BITS;
    // endpos_fractional = (uint64_t)end << SAMPLE_F_BITS;

    // phase_fractional = startpos_fractional + my_pos;

    // phase_fractional = startpos_fractional;
  }

  /** Change the sound table which will be played by the Oscil.
    @param TABLE_NAME is the name of the array in the table ".h" file you're using.
    */
  void setTable(const int16_t *TABLE_NAME)
  {
    table = TABLE_NAME;
  }

  /** Set the phase of the Oscil.  This does the same thing as Sample::start(offset).  Just different ways of thinking about oscillators and samples.
    @param phase a position in the wavetable.
    */
  // This could be called in the control interrupt, so phase_fractional should really be volatile,
  // but that could limit optimisation.  Since phase_fractional gets changed often in updateAudio()
  // (in loop()), it's probably worth keeping it nonvolatile until it causes problems
  void setPhase(uint64_t phase)
  {
    phase_fractional = (uint64_t)phase << OSCIL_F_BITS;
  }

  /** Set the phase of the Oscil.  Might be useful with getPhaseFractional().
    @param phase a position in the wavetable.
    */
  // This could be called in the control interrupt, so phase_fractional should really be volatile,
  // but that could limit optimisation.  Since phase_fractional gets changed often in updateAudio()
  // (in loop()), it's probably worth keeping it nonvolatile until it causes problems
  void setPhaseFractional(uint64_t phase)
  {
    phase_fractional = phase;
  }

  /** Get the phase of the Oscil in fractional format.
    @return position in the wavetable, shifted left by OSCIL_F_BITS (which is 16 when this was written).
    */
  uint64_t getPhaseFractional()
  {
    return phase_fractional;
  }

  /** Set the oscillator frequency with an uint16_t. This is faster than using a
    float, so it's useful when processor time is tight, but it can be tricky with
    low and high frequencies, depending on the size of the wavetable being used. If
    you're not getting the results you expect, try explicitly using a float, or try
    setFreq_Q24n8() or or setFreq_Q16n16().
    @param frequency to play the wave table.
    */
  inline void setFreq(int frequency)
  {
    // TB2014-8-20 change this following Austin Grossman's suggestion on user list
    // https://groups.google.com/forum/?utm_medium=email&utm_source=footer#!msg/mozzi-users/u4D5NMzVnQs/pCmiWInFvrkJ
    // phase_increment_fractional = ((((uint32_t)NUM_TABLE_CELLS<<ADJUST_FOR_NUM_TABLE_CELLS)*frequency)/UPDATE_RATE) << (OSCIL_F_BITS - ADJUST_FOR_NUM_TABLE_CELLS);
    // to this:
    phase_increment_fractional = ((uint64_t)frequency) * ((OSCIL_F_BITS_AS_MULTIPLIER * NUM_TABLE_CELLS) / UPDATE_RATE);
  }

  /** Set the oscillator frequency with a float. Using a float is the most reliable
    way to set frequencies, -Might- be slower than using an int but you need either
    this, setFreq_Q24n8() or setFreq_Q16n16() for fractional frequencies.
    @param frequency to play the wave table.
     */
  inline void setFreq(float frequency)
  { // 1 us - using float doesn't seem to incur measurable overhead with the oscilloscope
    phase_increment_fractional = (uint64_t)((((float)NUM_TABLE_CELLS * frequency) / UPDATE_RATE) * OSCIL_F_BITS_AS_MULTIPLIER);
  }

  /** Set the frequency using Q24n8 fixed-point number format.
    This might be faster than the float version for setting low frequencies such as
    1.5 Hz, or other values which may not work well with your table size. A Q24n8
    representation of 1.5 is 384 (ie. 1.5 * 256). Can't be used with UPDATE_RATE
    less than 64 Hz.
    @param frequency in Q24n8 fixed-point number format.
    */
  inline void setFreq_Q24n8(Q24n8 frequency)
  {
    // phase_increment_fractional = (frequency* (NUM_TABLE_CELLS>>3)/(UPDATE_RATE>>6)) << (F_BITS-(8-3+6));
    // TB2016-10-2 line below might have been left in accidentally while making the 2014 change below, remove for now
    //      phase_increment_fractional = (((((uint32_t)NUM_TABLE_CELLS<<ADJUST_FOR_NUM_TABLE_CELLS)>>3)*frequency)/(UPDATE_RATE>>6))
    //                                   << (OSCIL_F_BITS - ADJUST_FOR_NUM_TABLE_CELLS - (8-3+6));

    // TB2014-8-20 change this following Austin Grossman's suggestion on user list
    // https://groups.google.com/forum/?utm_medium=email&utm_source=footer#!msg/mozzi-users/u4D5NMzVnQs/pCmiWInFvrkJ
    if ((256UL * NUM_TABLE_CELLS) >= UPDATE_RATE)
    {
      phase_increment_fractional = ((uint64_t)frequency) * ((256UL * NUM_TABLE_CELLS) / UPDATE_RATE);
    }
    else
    {
      phase_increment_fractional = ((uint64_t)frequency) / (UPDATE_RATE / (256UL * NUM_TABLE_CELLS));
    }
  }

  /** Set the frequency using Q16n16 fixed-point number format. This is useful in
    combination with Q16n16_mtof(), a fast alternative to mtof(), using Q16n16
    fixed-point format instead of floats.
    @note This should work OK with tables 2048 cells or smaller and
    frequencies up to 4096 Hz.  Can't be used with UPDATE_RATE less than 64 Hz.
    @note This didn't run faster than float last time it was tested, after 2014 code changes.  Need to see if 2014 changes improved or worsened performance.
    @param frequency in Q16n16 fixed-point number format.
    */
  inline void setFreq_Q16n16(Q16n16 frequency)
  {
    // phase_increment_fractional = ((frequency * (NUM_TABLE_CELLS>>7))/(UPDATE_RATE>>6)) << (F_BITS-16+1);
    //  TB2014-8-20 change this following Austin Grossman's suggestion on user list
    //  https://groups.google.com/forum/?utm_medium=email&utm_source=footer#!msg/mozzi-users/u4D5NMzVnQs/pCmiWInFvrkJ
    // phase_increment_fractional = (((((uint32_t)NUM_TABLE_CELLS<<ADJUST_FOR_NUM_TABLE_CELLS)>>7)*frequency)/(UPDATE_RATE>>6))
    //                              << (OSCIL_F_BITS - ADJUST_FOR_NUM_TABLE_CELLS - 16 + 1);
    if (NUM_TABLE_CELLS >= UPDATE_RATE)
    {
      phase_increment_fractional = ((uint64_t)frequency) * (NUM_TABLE_CELLS / UPDATE_RATE);
    }
    else
    {
      phase_increment_fractional = ((uint64_t)frequency) / (UPDATE_RATE / NUM_TABLE_CELLS);
    }
  }
  /*
        inline
        void setFreqMidi(int8_t note_num) {
            setFreq_Q16n16(mtof(note_num));
        }
    */

  /** Set a specific phase increment.  See phaseIncFromFreq().
    @param phaseinc_fractional a phase increment value as calculated by phaseIncFromFreq().
     */
  inline void setPhaseInc(uint64_t phaseinc_fractional)
  {
    phase_increment_fractional = phaseinc_fractional;
  }

private:
  /** Used for shift arithmetic in setFreq() and its variations.
   */
  static const uint16_t ADJUST_FOR_NUM_TABLE_CELLS = (NUM_TABLE_CELLS < 2048) ? 8 : 0;

  /** Increments the phase of the oscillator without returning a sample.
   */
  inline void incrementPhase()
  {
    // phase_fractional += (phase_increment_fractional | 1); // odd phase incr, attempt to reduce frequency spurs in output
    phase_fractional += phase_increment_fractional;
  }

  // /** Returns the current sample.
  //    */
  // inline int16_t readTable() {
  //   return FLASH_OR_RAM_READ<const int16_t>(table + ((phase_fractional >> OSCIL_F_BITS) & (NUM_TABLE_CELLS - 1)));
  // }

  /** Returns the current sample.
   */
  inline int16_t readTable()
  {
    return FLASH_OR_RAM_READ<const int16_t>(table + frame_offset + ((phase_fractional >> OSCIL_F_BITS) & (NUM_TABLE_CELLS - 1)));
  }

  // location & NUM_TABLE_CELLS - 1 // this is their weird way of doing the modulo

  uint64_t phase_fractional;
  uint64_t phase_increment_fractional;
  uint64_t startpos_fractional, endpos_fractional;
  uint64_t frame_offset;
  const int16_t *table;
};

/**
@example 01.Basics/Vibrato/Vibrato.ino
This is an example using Oscil::phMod to produce vibrato using phase modulation.
*/

#endif /* MEAP_WAVETABLE_H */
