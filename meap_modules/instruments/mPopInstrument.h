#ifndef MPOP_INST_H_
#define MPOP_INST_H_

#include "mInstrument.h"
#include "mPopVoice.h"
#include <dependencies/LinkedList/LinkedList.h>

// polyphonic recreation of fors pop max for live device

template <uint16_t mPOLYPHONY = 4, class T = int32_t>
class mPopInstrument : public mInstrument<mPOLYPHONY>
{
public:
    mPopInstrument(uint8_t *midi_table_name = NULL) : mInstrument<mPOLYPHONY>(midi_table_name)
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

    void setReleaseTime(uint32_t release_time)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setReleaseTime(release_time);
        }
    }

    void setModAttack(uint32_t attack_time)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setModAttack(attack_time);
        }
    }

    void setModDecay(uint32_t decay_time)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setModDecay(decay_time);
        }
    }

    void setModSustain(uint32_t sustain_level)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setModSustain(sustain_level);
        }
    }

    void setModAmount(float modulation_amount)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setModAmount(modulation_amount);
        }
    }

    void setFeedback(float fdbk)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setFeedback(fdbk);
        }
    }

    void setTimbre(float timbre)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setTimbre(timbre);
        }
    }

    void setSpectraMap(int index)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setSpectraMap(index);
        }
    }

    void setSpectra(int32_t index)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setSpectra(index);
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

    int32_t next()
    {
        int32_t out_sample = 0;
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            out_sample += voices[i].next();
        }

        return out_sample;
    }

    mPopVoice<T> voices[mPOLYPHONY];
};

#endif // MPOP_INST_H_
