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

#ifndef MSYNCOSC_H_
#define MSYNCOSC_H_

template <int NUM_TABLE_CELLS, int UPDATE_RATE, class T = int8_t, meap_interpolation INTERP = mINTERP_NONE>
class mSyncOsc
{

public:
    /** Constructor.
    @param TABLE_NAME the name of the array the Oscil will be using. This
    can be found in the table ".h" file if you are using a table made for
    Mozzi by the int8_t2mozzi.py python script in Mozzi's python
    folder.*/
    mSyncOsc(const T *TABLE_NAME) : table(TABLE_NAME)
    {
        length_over_Fs = ((float)NUM_TABLE_CELLS) / ((float)UPDATE_RATE);
        increment = 110;
        accumulator = 0;
        sync_accumulator = 0;
        sync_increment = 110;
        sync_mul = 1.0;
        setFreq(220.f);
    }

    /** Constructor.
    Declare an Oscil with template TABLE_NUM_CELLS and UPDATE_RATE
    parameters, without specifying a particular wave table for it to play.
    The table can be set or changed on the fly with setTable(). Any tables
    used by the Oscil must be the same size.
    */
    mSyncOsc()
    {

        length_over_Fs = ((float)NUM_TABLE_CELLS) / ((float)UPDATE_RATE);
        increment = 110;
        accumulator = 0;
        sync_accumulator = 0;
        sync_increment = 110;
        sync_mul = 1.0;
        setFreq(220.f);
    }

    /** Updates the phase according to the current frequency and returns the sample at the new phase position.
    @return the next sample.
    */
    inline T next()
    {
        incrementPhase();
        return readTable();
    }

    /** Change the sound table which will be played by the Oscil.
    @param TABLE_NAME is the name of the array in the table ".h" file you're using.
    */
    void setTable(const T *TABLE_NAME)
    {
        table = TABLE_NAME;
    }

    void setPhase(uint32_t phase)
    {
        // phase_fractional = (uint32_t)phase << OSCIL_F_BITS;
    }

    // input 0-100 (corresponds to 1-4)
    void setSync(float sync_percent)
    {
        sync_mul = sync_percent * 0.07 + 1; // rescale to range 1-4
        sync_increment = increment * sync_mul;
    }

    void setFreq(float my_freq)
    {
        increment = my_freq * length_over_Fs;
        sync_increment = increment * sync_mul;
    }

    /** Increments the phase of the oscillator without returning a sample.
     */
    inline void incrementPhase()
    {
        accumulator += increment;
        if (accumulator >= NUM_TABLE_CELLS)
        {
            accumulator -= NUM_TABLE_CELLS;
            sync_accumulator = 0;
        }

        sync_accumulator += sync_increment;
        if (sync_accumulator >= NUM_TABLE_CELLS)
        {
            sync_accumulator -= NUM_TABLE_CELLS;
        }
    }

    /** Returns the current sample.
     */
    inline T readTable()
    {
        if (INTERP == mINTERP_LINEAR)
        {
            uint32_t index = (int)sync_accumulator;
            uint32_t next_index = (index + 1) % NUM_TABLE_CELLS;
            float fractional = sync_accumulator - index;
            int64_t out = table[index];
            out += fractional * (table[next_index] - out);
            return out;
        }
        else
        {
            return table[(int)sync_accumulator];
        }
    }

    const T *table;
    float increment;
    float sync_increment;
    float sync_mul;
    float accumulator;
    float sync_accumulator;

    float length_over_Fs;
};

#endif /* MSYNCOSC_H_ */
