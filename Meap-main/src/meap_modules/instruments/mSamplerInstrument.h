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

        for (uint16_t i = 0; i < 127; i++)
        {

            sample_frequencies[i] = voices[0].default_freq_ * pow(2.f, ((float)(i - 60)) / 12.f);
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
        voices[this->_curr_voice].noteOn(sample_frequencies[note], vel);
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

    void setADSR(uint32_t attack_time, uint32_t decay_time, uint32_t sustain_level, uint32_t release_time)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setADLevels(255, sustain_level);
            voices[i].setTimes(attack_time, decay_time, 100000000, release_time);
        }
    }

    void setStart(uint64_t start_sample)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setStart(start_sample);
        }
    }

    void update()
    {
        for (uint8_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].update();
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
    float sample_frequencies[127];
};

#endif // MSAMPLER_INSTRUMENT_H_
