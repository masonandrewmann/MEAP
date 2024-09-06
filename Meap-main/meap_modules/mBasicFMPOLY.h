#ifndef MEAP_BASICFMPOLY_H_
#define MEAP_BASICFMPOLY_H_

#include <tables/whitenoise8192_int8.h>
#include <tables/BandLimited_SQUARE/2048/square_max_68_at_16384_2048_int8.h>

// implements a hardcoded FM synth
// algorithm
//   ___  ___
//   |B|  |C|
//   ________
//    |
//    v
//   ___
//   |A|
//   ___

template <uint32_t NUM_CELLS, uint16_t mPOLYPHONY>
class mBasicFMPoly
{
public:
    mBasicFMPoly(const int8_t *TABLE_NAME, uint8_t *base_address)
    {

        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            oscA_[i].setTable(TABLE_NAME);
            envA_[i].setTimes(1, 100, 4294967295, 1);
            envA_[i].setADLevels(255, 0);
            gainA_[i] = 0;

            oscB_[i].setTable(WHITENOISE8192_DATA);
            envB_[i].setTimes(1, 100, 4294967295, 1);
            envB_[i].setADLevels(255, 0);
            gainB_[i] = 0;

            oscC_[i].setTable(SQUARE_MAX_68_AT_16384_2048_DATA);
            envC_[i].setTimes(1, 10000, 4294967295, 500);
            envC_[i].setADLevels(255, 255);
            gainC_[i] = 0;
            velocity_[i] = 255;
        }

        b_depth_ = 255;
        c_depth_ = 255;
        mod_depth_ = Q7n0_to_Q7n8(127);
        curr_voice_ = 0;

        base_address_ = base_address;

        playing_ = false;

        message_type_ = 0;
        data1_ = 0;
        data2_ = 0;
        time_ = 0;

        pulse_counter_ = 0;
    };

    void begin()
    {
        // start_time = millis();
        playing_ = true;
        current_address_ = base_address_;
        pulse_counter_ = 0;
        message_type_ = current_address_[0];
        data1_ = current_address_[1];
        data2_ = current_address_[2];
        time_ = (current_address_[3] << 8) + current_address_[4];
    }

    void setTimes(uint8_t operator_num, uint32_t a, uint32_t d, uint32_t s, uint32_t r)
    {

        if (operator_num == 0)
        { // operator A
            for (uint16_t i = 0; i < mPOLYPHONY; i++)
            {
                envA_[i].setTimes(a, d, s, r);
            }
        }
        else if (operator_num == 1)
        {
            for (uint16_t i = 0; i < mPOLYPHONY; i++)
            {
                envB_.setTimes(a, d, s, r);
            }
        }
    }

    // 0 to 127 input!
    void setModDepth(uint8_t depth)
    {
        mod_depth_ = Q7n0_to_Q7n8(depth);
    }

    void setBDepth(uint32_t depth)
    {
        b_depth_ = depth;
    }

    void setCDepth(uint32_t depth)
    {
        c_depth_ = depth;
    }

    void setDecayTime(uint8_t operator_num, uint32_t d_t)
    {
        if (operator_num == 0)
        { // operator A
            for (uint16_t i = 0; i < mPOLYPHONY; i++)
            {
                envA_[i].setDecayTime(d_t);
            }
        }
        else if (operator_num == 1)
        {
            for (uint16_t i = 0; i < mPOLYPHONY; i++)
            {
                envB_[i].setDecayTime(d_t);
            }
        }
        else if (operator_num == 2)
        {
            for (uint16_t i = 0; i < mPOLYPHONY; i++)
            {
                envC_[i].setDecayTime(d_t);
            }
        }
    }

    void stop()
    {
        // send noteoffs to everything
        playing_ = false;
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            envA_[i].noteOff();
            envB_[i].noteOff();
            envC_[i].noteOff();
        }
    }

    void updateMidi()
    {
        if (playing_)
        {
            pulse_counter_ += 1;
            while (pulse_counter_ >= time_)
            {
                switch (message_type_) // notes are indexed from 0 on sample_bank starting from C-1 (0)
                {
                case 0x80: // note off
                    noteOff(data1_);
                    break;
                case 0x90: // note on
                    if (data1_ != 127)
                    {
                        noteOn(data1_, data2_);
                    }
                    break;
                case 255: // end of file
                    playing_ = false;
                    break;
                }
                current_address_ += 5;
                pulse_counter_ = 0;
                message_type_ = current_address_[0];
                data1_ = current_address_[1];
                data2_ = current_address_[2];
                time_ = (current_address_[3] << 8) + current_address_[4];
            }
        }
    }

    bool isPlaying()
    {
        return playing_;
    }

    void noteOn(uint16_t sample_num, float freq)
    {
        float my_freq = mtof(data1_);
        float low_freq = mtof(data1_ - 12);
        oscA_[curr_voice_].setFreq(my_freq);
        envA_[curr_voice_].noteOn();
        oscB_[curr_voice_].setFreq(my_freq);
        envB_[curr_voice_].noteOn();
        oscC_[curr_voice_].setFreq(low_freq);
        envC_[curr_voice_].noteOn();
        note_num_[curr_voice_] = data1_;
        velocity_[curr_voice_] = data2_;
        curr_voice_ = (curr_voice_ + 1) % mPOLYPHONY;
    }

    void noteOff(uint16_t sample_num)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            if (note_num_[i] == data1_)
            {
                envA_[i].noteOff();
                envB_[i].noteOff();
                envC_[i].noteOff();
            }
        }
    }

    int32_t next()
    {
        int32_t out_sample = 0;
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            envA_[i].update();
            gainA_[i] = envA_[i].next();

            envB_[i].update();
            gainB_[i] = envB_[i].next();

            envC_[i].update();
            gainC_[i] = envC_[i].next();

            int32_t bVal = oscB_[i].next() * gainB_[i] * b_depth_ >> 16;
            int32_t cVal = oscC_[i].next() * gainC_[i] * c_depth_ >> 16;
            Q15n16 modulation = mod_depth_ * (bVal + cVal) >> 8; // calculate mod depth in 15n16 signed int

            out_sample += (gainA_[i] * oscA_[i].phMod(modulation * 4) * velocity_[i]) >> 8; // scale by factor of 4 and apply phase mod to carrier oscillator
        }

        return out_sample;
    }

    uint8_t *current_address_;

protected:
    Oscil<NUM_CELLS, AUDIO_RATE> oscA_[mPOLYPHONY];
    ADSR<AUDIO_RATE, AUDIO_RATE> envA_[mPOLYPHONY];
    uint16_t gainA_[mPOLYPHONY];

    Oscil<WHITENOISE8192_NUM_CELLS, AUDIO_RATE> oscB_[mPOLYPHONY];
    ADSR<AUDIO_RATE, AUDIO_RATE> envB_[mPOLYPHONY];
    uint16_t gainB_[mPOLYPHONY];

    Oscil<SQUARE_MAX_68_AT_16384_2048_NUM_CELLS, AUDIO_RATE> oscC_[mPOLYPHONY];
    ADSR<AUDIO_RATE, AUDIO_RATE> envC_[mPOLYPHONY];
    uint16_t gainC_[mPOLYPHONY];

    uint8_t velocity_[mPOLYPHONY];

    uint8_t note_num_[mPOLYPHONY];

    uint16_t curr_voice_;

    // reading midi file
    uint8_t *base_address_;

    uint64_t pulse_counter_;

    bool playing_;

    // last midi message
    uint8_t message_type_;
    uint8_t data1_;
    uint8_t data2_;
    uint16_t time_;

    Q16n16 mod_depth_;
    uint32_t b_depth_;
    uint32_t c_depth_;
};

#endif // MEAP_BASICFMPOLY_H_
