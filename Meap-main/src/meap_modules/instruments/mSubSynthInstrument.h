#ifndef MSUBSYNTH_INSTRUMENT_H_
#define MSUBSYNTH_INSTRUMENT_H_

#include <dependencies/LinkedList/LinkedList.h>
#include "mInstrument.h"
#include "mSubSynthVoice.h"

template <uint32_t mNUM_CELLS, uint32_t mNUM_OSC = 1, uint16_t mPOLYPHONY = 4, class T = int8_t>
class mSubSynthInstrument : public mInstrument<mPOLYPHONY>
{
public:
    mSubSynthInstrument(const T *TABLE_NAME = NULL, uint8_t *midi_table_name = NULL) : mInstrument<mPOLYPHONY>(midi_table_name)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
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

    void setOscGain(uint16_t osc_num, uint16_t osc_gain)
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setOscGain(osc_num, osc_gain);
        }
    }

    void setOscSemitones(uint16_t osc_num, uint16_t semitone_offset)
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setOscSemitones(osc_num, semitone_offset);
        }
    }

    void setOscDetune(uint16_t osc_num, float amount)
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setOscDetune(osc_num, amount);
        }
    }

    // 0 - 255
    void setCutoff(uint16_t cutoff_val)
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setCutoff(cutoff_val);
        }
    }

    // 0 - 255
    void setResonance(uint16_t resonance_val)
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setResonance(resonance_val);
        }
    }

    void setAttackTime(uint32_t a_)
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setAttackTime(a_);
        }
    }

    void setDecayTime(uint32_t d_)
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setDecayTime(d_);
        }
    }

    void setSustainLevel(uint32_t s_)
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setSustainLevel(s_);
        }
    }

    void setReleaseTime(uint32_t r_)
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setReleaseTime(r_);
        }
    }

    void setNoiseGain(uint32_t noise_gain)
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setNoiseGain(noise_gain);
        }
    }

    void setFilterEnvAmount(uint32_t amount)
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setFilterEnvAmount(amount);
        }
    }

    void setFilterAttackTime(uint32_t time)
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setFilterAttackTime(time);
        }
    }

    void setFilterReleaseTime(uint32_t time)
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setFilterReleaseTime(time);
        }
    }

    void setFilterDecayTime(uint32_t time)
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setFilterDecayTime(time);
        }
    }

    void setFilterSustainLevel(uint32_t level)
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setFilterSustainLevel(level);
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
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            out_sample += voices[i].next();
        }

        return out_sample;
    }

    // CLASS VARIABLES
    mSubSynthVoice<mNUM_CELLS, mNUM_OSC, T> voices[mPOLYPHONY];
};

#endif // MSUBSYNTH_INSTRUMENT_H_
