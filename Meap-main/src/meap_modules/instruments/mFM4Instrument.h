#ifndef MFM4INST_H_
#define MFM4INST_H_

#include <tables/sin8192_int8.h> // table for Oscils to play
#include <dependencies/LinkedList/LinkedList.h>
#include "mFM4Voice.h"
#include "mInstrument.h"

template <uint32_t mNUM_CELLS = SIN8192_NUM_CELLS, uint16_t mPOLYPHONY = 4, class T = int8_t>
class mFM4Instrument : public mInstrument<mPOLYPHONY>
{
public:
    mFM4Instrument(const int8_t *TABLE_NAME = SIN8192_DATA, uint8_t *midi_table_name = NULL) : mInstrument<mPOLYPHONY>(midi_table_name)
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
