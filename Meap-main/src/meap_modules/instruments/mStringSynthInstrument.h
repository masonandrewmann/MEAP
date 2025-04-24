#ifndef MSTRING_SYNTH_INTRUMENT_H_
#define MSTRING_SYNTH_INTRUMENT_H_

#include <dependencies/LinkedList/LinkedList.h>

#include "mInstrument.h"
#include "mStringSynthVoice.h"

template <uint16_t mPOLYPHONY = 4, class T = int8_t>
class mStringSynthInstrument : public mInstrument<mPOLYPHONY>
{
public:
    mStringSynthInstrument(uint8_t *midi_table_name = NULL) : mInstrument<mPOLYPHONY>(midi_table_name)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].init();
        }
    }

    void setADSR(uint32_t a_t, uint32_t d_t, uint16_t s_l, uint32_t r_t)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setADSR(a_t, d_t, s_l, r_t);
        }
    }

    void setCutoffFreqAndResonance(uint32_t c, uint32_t r)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setCutoffFreqAndResonance(c, r);
        }
    }

    void setDetune(uint32_t d_v)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setDetune(d_v);
        }
    }

    void setEnsembleMix(uint32_t e_v)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setEnsembleMix(e_v);
        }
    }

    void setPhaserFreq(float p_f)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setPhaserFreq(p_f);
        }
    }

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

    void update()
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].update();
        }
    }

    int32_t next()
    {
        int32_t out_sample = 0;
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            out_sample += voices[i].next();
        }

        return out_sample;
    }

    // CLASS VARIABLES
    mStringSynthVoice<T> voices[mPOLYPHONY];
};

#endif // MSTRING_SYNTH_INTRUMENT_H_
