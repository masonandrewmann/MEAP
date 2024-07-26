#ifndef MEAP_SUBSYNTH_H_
#define MEAP_SUBSYNTH_H_

#include <tables/saw2048_int8.h>

template <uint64_t mAUDIO_RATE>
class mSubSynth
{
public:
    mSubSynth(uint8_t *base_address)
    {
        osc.setTable(SAW2048_DATA);
        freq_ = 220;
        osc.setFreq(220);
        adsr_.setTimes(1, 100, 4294967295, 1);
        adsr_.setADLevels(255, 0);
        gain_ = 0;

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

    void setTimes(uint64_t a, uint64_t d, uint64_t s, uint64_t r)
    {
        adsr_.setTimes(a, d, s, r);
    }

    void setDecayTime(uint64_t d_t)
    {
        adsr_.setDecayTime(d_t);
    }

    void setAttackTime(uint64_t a_t)
    {
        adsr_.setAttackTime(a_t);
    }

    void setADLevels(int a_t, int d_t)
    {
        adsr_.setADLevels(a_t, d_t);
    }

    void stop()
    {
        // send noteoffs to everything
        playing_ = false;
        adsr_.noteOff();
    }

    void updateMidi()
    {
        if (playing_)
        {
            pulse_counter_ += 1;
            // Serial.println(pulse_counter_);

            while (pulse_counter_ >= time_)
            {

                Serial.print(message_type_);
                Serial.print(" ");
                Serial.print(data1_);
                Serial.print(" ");
                Serial.print(data2_);
                Serial.print(" ");
                Serial.print(current_address_[3]);
                Serial.print(" ");
                Serial.print(current_address_[4]);
                Serial.print(" ");
                Serial.println(time_);

                switch (message_type_) // notes are indexed from 0 on sample_bank starting from C-1 (0)
                {
                case 0x80: // note off
                    // sample_bank_[data1_].noteOff(255);
                    adsr_.noteOff();
                    break;
                case 0x90: // note on
                    osc.setFreq(mtof(data1_));
                    adsr_.noteOn();
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

    void noteOn(float freq)
    {
        adsr_.noteOn();
        freq_ = freq;
        osc.setFreq(freq_);
    }

    void setFreq(float freq){
        freq_ = freq;
        osc.setFreq(freq_);
    }

    void noteOff()
    {
        adsr_.noteOff();
    }

    int32_t next()
    {
        adsr_.update();
        gain_ = adsr_.next();

        int32_t output_sample = osc.next() * gain_;
        return output_sample;
    }

protected:
    Oscil<SAW2048_NUM_CELLS, AUDIO_RATE> osc;

    // Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

    ADSR<mAUDIO_RATE, mAUDIO_RATE> adsr_;
    uint16_t gain_;

    // reading midi file
    uint8_t *base_address_;
    uint8_t *current_address_;
    uint64_t pulse_counter_;

    bool playing_;

    float freq_;

    // last midi message
    uint8_t message_type_;
    uint8_t data1_;
    uint8_t data2_;
    uint16_t time_;
};

#endif // MEAP_SUBSYNTH_H_
