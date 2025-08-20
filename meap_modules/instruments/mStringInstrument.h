#ifndef MSTRING_INSTRUMENT_H_
#define MSTRING_INSTRUMENT_H_

#include <dependencies/LinkedList/LinkedList.h>
#include "mInstrument.h"
#include "mStringVoice.h"

// A polyphonic karplus-strong instrument

template <uint16_t mPOLYPHONY = 4>
class mStringInstrument : public mInstrument<mPOLYPHONY>
{
public:
    mStringInstrument(float lowest_freq = 50, uint8_t *midi_table_name = NULL) : mInstrument<mPOLYPHONY>(midi_table_name)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].init(lowest_freq, lowest_freq);
        }
    };

    void midiStop()
    {
        mInstrument<mPOLYPHONY>::midiStop();
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].noteOff();
        }
    }

    void noteOn(uint16_t note, uint16_t vel)
    {
        mInstrument<mPOLYPHONY>::noteOnVoiceHandler(note, vel);
        voices[this->_curr_voice].noteOn(note, vel);
    }

    void noteOff(uint16_t note)
    {
        int16_t off_voice = mInstrument<mPOLYPHONY>::noteOffVoiceHandler(note);
    }

    void flush()
    {
        mInstrument<mPOLYPHONY>::flush();
        for (uint8_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].noteOff();
        }
    }

    void setLoopGain(float loop_gain)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setLoopGain(loop_gain);
        }
    }

    int32_t next()
    {
        int32_t out_sample = 0;
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            out_sample += voices[i].next();
        }
        return out_sample;
    }

    // CLASS VARIABLES
    mStringVoice voices[mPOLYPHONY];
};

#endif // MSTRING_INSTRUMENT_H_
