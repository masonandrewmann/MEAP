#ifndef MFM4INST_H_
#define MFM4INST_H_

#include <tables/sin8192_int16.h> // table for Oscils to play
#include <dependencies/LinkedList/LinkedList.h>
#include "mFM4Voice.h"
#include "mInstrument.h"

// polyphonic 4 operator FM instrument

template <uint16_t mPOLYPHONY = 4, uint32_t mNUM_CELLS = sin8192_int16_NUM_CELLS, class T = int16_t>
class mFM4Instrument : public mInstrument<mPOLYPHONY>
{
public:
    mFM4Instrument(const T *TABLE_NAME = sin8192_int16_DATA, uint8_t *midi_table_name = NULL) : mInstrument<mPOLYPHONY>(midi_table_name)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].init(TABLE_NAME);
        }
    };

    void midiStop()
    {
        // send noteoffs to everything
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
        int8_t off_voice = mInstrument<mPOLYPHONY>::noteOffVoiceHandler(note);
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

    void setAlgorithm(uint16_t a)
    {
        if (a < 0)
        {
            a = 0;
        }
        else if (a > 10)
        {
            a = 10;
        }
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setAlgorithm(a);
        }
    }

    void setFreqRatios(float one_r, float two_r, float three_r, float four_r)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setFreqRatios(one_r, two_r, three_r, four_r);
        }
    }

    void setADSR(uint16_t operator_num, uint32_t attack_time, uint32_t decay_time, uint16_t sustain_level, uint32_t release_time)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setADSR(operator_num, attack_time, decay_time, sustain_level, release_time);
        }
    }

    void setTable(uint16_t operator_num, const T *TABLE_NAME)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setTable(operator_num, TABLE_NAME);
        }
    }

    void setFixedFreq(uint16_t operator_num, float fixed_freq)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setFixedFreq(operator_num, fixed_freq);
        }
    }

    void setLoopingOn(uint16_t operator_num)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setLoopingOn(operator_num);
        }
    }

    void setLoopingOff(uint16_t operator_num)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setLoopingOff(operator_num);
        }
    }

    void setDroneOn(uint16_t operator_num)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].seDroneOn(operator_num);
        }
    }

    void setDroneOff(uint16_t operator_num)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setDroneOn(operator_num);
        }
    }

    void setGain(uint16_t operator_num, uint16_t gain)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setGain(operator_num, gain);
        }
    }

    void update()
    {
        for (int i = mPOLYPHONY; --i >= 0;)
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
    mFM4Voice<mNUM_CELLS, T> voices[mPOLYPHONY];
};

#endif // MFM4INST_H_
