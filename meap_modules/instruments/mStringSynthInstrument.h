#ifndef MSTRINGSYNTH_INSTRUMENT_H_
#define MSTRINGSYNTH_INSTRUMENT_H_

#include <dependencies/LinkedList/LinkedList.h>
#include "mInstrument.h"
#include "mStringSynthInstrument.h"

template <uint16_t mPOLYPHONY = 4>
class mStringSynthInstrument : public mInstrument<mPOLYPHONY>
{
public:
    mStringSynthInstrument(uint8_t *midi_table_name = NULL) : mInstrument<mPOLYPHONY>(midi_table_name)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].init();
        }
    };

    void setADSR(uint32_t attack_time, uint32_t decay_time, uint16_t sustain_level, uint32_t release_time)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setADSR(attack_time, decay_time, sustain_level, release_time);
        }
    }

    void setCutoffFreqAndResonance(uint8_t cutoff, uint8_t resonance)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setCutoffFreqAndResonance(cutoff, resonance);
        }
    }

    void setDetune(float detune)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setDetune(detune);
        }
    }

    void midiStop()
    {
        mInstrument<mPOLYPHONY>::midiStop();
        for (int16_t i = 0; i < mPOLYPHONY; i++)
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
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].noteOff();
        }
    }

    void update()
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
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
    mStringSynthVoice voices[mPOLYPHONY];
};

#endif // MSTRINGSYNTH_INSTRUMENT_H_