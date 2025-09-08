#ifndef M_DRUM_RACK_INSTRUMENT_H_
#define M_DRUM_RACK_INSTRUMENT_H_

#include <dependencies/LinkedList/LinkedList.h>

#include "mInstrument.h"

template <uint16_t mNUM_SAMPLES, uint32_t mMAX_SAMPLE_LENGTH, uint16_t mPOLYPHONY = 4, class T = int8_t>
class mDrumRackInstrument : public mInstrument<mPOLYPHONY>
{
public:
    mDrumRackInstrument(const T **sample_list, uint32_t *sample_lengths, int32_t midi_offset = 0, uint8_t *midi_table_name = NULL) : mInstrument<mPOLYPHONY>(midi_table_name)
    {
        sample_list_ = sample_list;
        sample_lengths_ = sample_lengths;

        default_freq = (float)AUDIO_RATE / (float)mMAX_SAMPLE_LENGTH;

        midi_offset_ = midi_offset;

        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            sample_bank_[i].setTable(sample_list_[0]);
            sample_bank_[i].setEnd(sample_lengths_[0]);
            sample_bank_[i].setFreq(default_freq);
            sample_bank_[i].stop();
            voice_gains[i] = 1.0;
        }

        for (int16_t i = mNUM_SAMPLES; --i >= 0;)
        {
            sample_gains[i] = 1.0;
        }

        if (sizeof(T) == sizeof(int16_t))
        {
            shift_val_ = 15; // 8 bits down for sample birate + 7 bits for velocity
        }
        else
        {
            shift_val_ = 7; // just 7 bits down for velocity
        }
    }

    void setGain(int sample, float gain)
    {
        sample_gains[sample] = gain;
    }

    void midiStop()
    {
        mInstrument<mPOLYPHONY>::midiStop();
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            sample_bank_[i].stop();
        }
    }

    void noteOn(uint16_t note, uint16_t vel)
    {
        mInstrument<mPOLYPHONY>::noteOnVoiceHandler(note, vel);
        sample_bank_[this->_curr_voice].setTable(sample_list_[note - midi_offset_]);
        sample_bank_[this->_curr_voice].setEnd(sample_lengths_[note - midi_offset_]);
        sample_bank_[this->_curr_voice].start();
        voice_gains[this->_curr_voice] = sample_gains[note - midi_offset_] * (((float)vel) * 0.007874015748);
    }

    void noteOff(uint16_t note)
    {
        int16_t off_voice = mInstrument<mPOLYPHONY>::noteOffVoiceHandler(note);
        if (off_voice != -1)
        {
            // sample_bank_[off_voice].stop();
        }
    }

    void flush()
    {
        mInstrument<mPOLYPHONY>::flush();
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            sample_bank_[i].stop();
        }
    }

    int32_t next()
    {
        int32_t out_sample = 0;
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            out_sample += (sample_bank_[i].next() << 8) * voice_gains[i];
        }

        return out_sample;
    }

    // CLASS VARIABLES
    mSample<mMAX_SAMPLE_LENGTH, AUDIO_RATE, T> sample_bank_[mPOLYPHONY];

    float voice_gains[mPOLYPHONY];
    float sample_gains[mNUM_SAMPLES];

    float default_freq;
    int midi_offset_;

    const T **sample_list_;
    uint32_t *sample_lengths_;

    uint8_t shift_val_;
};

#endif // M_DRUM_RACK_INSTRUMENT_H_
