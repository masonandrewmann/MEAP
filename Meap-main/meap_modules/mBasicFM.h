#ifndef MEAP_BASICFM_H_
#define MEAP_BASICFM_H_

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

template <uint32_t NUM_CELLS>
class mBasicFM
{
public:
    mBasicFM(const int8_t *TABLE_NAME, uint8_t *base_address)
    {
        oscA_.setTable(TABLE_NAME);
        envA_.setTimes(1, 100, 4294967295, 1);
        envA_.setADLevels(255, 0);
        gainA_ = 0;

        oscB_.setTable(WHITENOISE8192_DATA);
        envB_.setTimes(1, 100, 4294967295, 1);
        envB_.setADLevels(255, 0);
        gainB_ = 0;

        oscC_.setTable(SQUARE_MAX_68_AT_16384_2048_DATA);
        envC_.setTimes(1, 10000, 4294967295, 500);
        envC_.setADLevels(255, 255);
        gainC_ = 0;

        b_depth_ = 255;
        c_depth_ = 255;
        mod_depth_ = Q7n0_to_Q7n8(127);

        base_address_ = base_address;

        playing_ = false;

        message_type_ = 0;
        data1_ = 0;
        data2_ = 0;
        time_ = 0;

        pulse_counter_ = 0;

        velocity_ = 255;
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
            envA_.setTimes(a, d, s, r);
        }
        else if (operator_num == 1)
        {
            envB_.setTimes(a, d, s, r);
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
            envA_.setDecayTime(d_t);
        }
        else if (operator_num == 1)
        {
            envB_.setDecayTime(d_t);
        }
    }

    void stop()
    {
        // send noteoffs to everything
        playing_ = false;
        envA_.noteOff();
        envB_.noteOff();
        envC_.noteOff();
    }

    void updateMidi()
    {
        if (playing_)
        {
            pulse_counter_ += 1;
            // Serial.println(pulse_counter_);

            while (pulse_counter_ >= time_)
            {
                switch (message_type_) // notes are indexed from 0 on sample_bank starting from C-1 (0)
                {
                case 0x80: // note off
                    envA_.noteOff();
                    envB_.noteOff();
                    envC_.noteOff();
                    break;
                case 0x90: // note on
                    if (data1_ != 127)
                    {
                        velocity_ = data2_;
                        oscA_.setFreq(mtof(data1_));
                        envA_.noteOn();
                        oscB_.setFreq(mtof(data1_));
                        envB_.noteOn();
                        oscC_.setFreq(mtof(data1_ - 12));
                        envC_.noteOn();
                    }

                    break;
                case 255: // end of file
                    playing_ = false;
                    break;
                }
                pulse_counter_ = 0;

                message_type_ = current_address_[0];
                data1_ = current_address_[1];
                data2_ = current_address_[2];
                time_ = (current_address_[3] << 8) + current_address_[4];
                current_address_ += 5;
            }
        }
    }

    bool isPlaying()
    {
        return playing_;
    }

    void noteOn(uint16_t sample_num, float freq)
    {
        envA_.noteOn();
        oscA_.setFreq(freq);

        envB_.noteOn();
        oscB_.setFreq(freq);

        envC_.noteOn();
        oscC_.setFreq(freq / 2);
    }

    void noteOff(uint16_t sample_num)
    {
        envA_.noteOff();
        envB_.noteOff();
        envC_.noteOff();
    }

    int32_t next()
    {
        envA_.update();
        gainA_ = envA_.next();

        envB_.update();
        gainB_ = envB_.next();

        envC_.update();
        gainC_ = envC_.next();

        int32_t bVal = oscB_.next() * gainB_ * b_depth_ >> 16;
        int32_t cVal = oscC_.next() * gainC_ * c_depth_ >> 16;

        Q15n16 modulation = mod_depth_ * (bVal + cVal) >> 8;       // calculate mod depth in 15n16 signed int
        int32_t out_sample = gainA_ * oscA_.phMod(modulation * 4); // scale by factor of 4 and apply phase mod to carrier oscillator

        return (out_sample * velocity_) >> 7;
    }

    uint8_t *current_address_;

protected:
    Oscil<NUM_CELLS, AUDIO_RATE> oscA_;
    ADSR<AUDIO_RATE, AUDIO_RATE> envA_;
    uint16_t gainA_;

    Oscil<WHITENOISE8192_NUM_CELLS, AUDIO_RATE> oscB_;
    ADSR<AUDIO_RATE, AUDIO_RATE> envB_;
    uint16_t gainB_;

    Oscil<SQUARE_MAX_68_AT_16384_2048_NUM_CELLS, AUDIO_RATE> oscC_;
    ADSR<AUDIO_RATE, AUDIO_RATE> envC_;
    uint16_t gainC_;

    uint8_t velocity_;

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

#endif // MEAP_BASICFM_H_
