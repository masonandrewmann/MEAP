#ifndef MSAMPLER_INSTRUMENT_H_
#define MSAMPLER_INSTRUMENT_H_

#include <dependencies/LinkedList/LinkedList.h>
#include "mInstrument.h"
#include <meap_modules/clutter/mSampler.h>

// A polyphonic sampler instrument

template <uint64_t TABLE_SIZE, class T = int8_t, uint16_t mPOLYPHONY = 4, uint8_t INTERP = mINTERP_NONE>
class mSamplerInstrument : public mInstrument<mPOLYPHONY>
{
public:
    mSamplerInstrument(const T *TABLE_NAME, uint8_t *midi_table_name = NULL) : mInstrument<mPOLYPHONY>(midi_table_name)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].init(TABLE_NAME);
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
        if (off_voice != -1)
        {
            voices[off_voice].noteOff();
        }
    }

    void flush()
    {
        mInstrument<mPOLYPHONY>::flush();
        for (uint8_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].noteOff();
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
    mSampler<TABLE_SIZE, T, INTERP> voices[mPOLYPHONY];
};

#endif // MSAMPLER_INSTRUMENT_H_
