#ifndef MEAP_DRUMRACKENVELOPE_H_
#define MEAP_DRUMRACKENVELOPE_H_

template <uint64_t mMAX_SAMPLE_LENGTH, uint8_t mNUM_SAMPLES, uint64_t mAUDIO_RATE>
class mDrumRack
{
public:
    mDrumRackEnvelope(const int8_t **sample_list, uint32_t *sample_lengths, uint8_t *base_address)
    {
        sample_list_ = sample_list;
        sample_lengths_ = sample_lengths;

        base_address_ = base_address;

        playing_ = false;

        message_type_ = 0;
        data1_ = 0;
        data2_ = 0;
        time_ = 0;

        pulse_counter_ = 0;

        default_freq_ = (float)mAUDIO_RATE / (float)mMAX_SAMPLE_LENGTH;

        for (uint8_t i; i < mNUM_SAMPLES; i++)
        {
            sample_gain_[i] = 255;
            velocity_[i] = 127;
            sample_bank_[i].setTable(sample_list_[i], sample_lengths_[i]);
        }
    };

    mDrumRackEnvelope(const int8_t **sample_list, uint32_t *sample_lengths)
    {
        sample_list_ = sample_list;
        sample_lengths_ = sample_lengths;

        base_address_ = NULL;

        playing_ = false;

        message_type_ = 0;
        data1_ = 0;
        data2_ = 0;
        time_ = 0;

        pulse_counter_ = 0;

        default_freq_ = (float)mAUDIO_RATE / (float)mMAX_SAMPLE_LENGTH;

        for (uint8_t i; i < mNUM_SAMPLES; i++)
        {
            sample_gain_[i] = 255;
            velocity_[i] = 127;
            sample_bank_[i].setTable(sample_list_[i], sample_lengths_[i]);
        }
    };

    void init(const int8_t **sample_list, uint32_t *sample_lengths, uint8_t *base_address)
    {
        sample_list_ = sample_list;
        sample_lengths_ = sample_lengths;

        base_address_ = base_address;

        playing_ = false;

        message_type_ = 0;
        data1_ = 0;
        data2_ = 0;
        time_ = 0;

        pulse_counter_ = 0;

        default_freq_ = (float)mAUDIO_RATE / (float)mMAX_SAMPLE_LENGTH;

        for (uint8_t i; i < mNUM_SAMPLES; i++)
        {
            sample_gain_[i] = 255;
            velocity_[i] = 127;
            sample_bank_[i].setTable(sample_list_[i], sample_lengths[i]);
        }
    }

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

    void stop()
    {
        // send noteoffs to everything
        playing_ = false;
        for (uint16_t i = 0; i < mNUM_SAMPLES; i++)
        {
            sample_bank_[i].noteOff(255);
        }
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
                    // sample_bank_[data1_].noteOff(255);
                    break;
                case 0x90: // note on
                    noteOn(data1_, data2_, default_freq_);
                    // sample_bank_[data1_].noteOn(default_freq_, 255);
                    // velocity_[data1_] = data2_;
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

    void noteOn(uint16_t sample_num, uint8_t vel, float freq)
    {
        sample_bank_[sample_num].noteOn(freq, vel);
        env_[sample_num].noteOn();
        // sample_bank_[data1_].noteOn(default_freq_, 255);
        // velocity_[data1_] = data2_;
    }

    void noteOn(uint16_t sample_num)
    {
        sample_bank_[sample_num].noteOn(default_freq_, 127);
        env_[sample_num].noteOn();
        // sample_bank_[data1_].noteOn(default_freq_, 255);
        // velocity_[data1_] = data2_;
    }

    void noteOff(uint16_t sample_num)
    {
        // sample_bank_[sample_num].noteOff(255);
        env_[sample_num].noteOff();
    }

    void setSampleVolume(uint8_t sample_num, float volume)
    {
        sample_gain_[sample_num] = volume;
    }

    int32_t next()
    {
        // updateMidi();
        uint32_t output_sample = 0;
        for (uint8_t i = 0; i < mNUM_SAMPLES; i++)
        {
            env_[i].update();
            output_sample += ((sample_bank_[i].next() * sample_gain_[i] * env_.next()));
        }
        return (output_sample >> 16);
    }

    ADSR<mAUDIO_RATE, mAUDIO_RATE> env_[mNUM_SAMPLES];

protected:
    // sampler voices
    mSampler<mMAX_SAMPLE_LENGTH, mAUDIO_RATE, int8_t> sample_bank_[mNUM_SAMPLES];

    const int8_t **sample_list_;
    uint32_t *sample_lengths_;
    uint16_t sample_gain_[mNUM_SAMPLES];
    uint16_t velocity_[mNUM_SAMPLES];

    uint8_t *base_address_;
    uint8_t *current_address_;

    // uint64_t start_time;
    uint64_t pulse_counter_;
    bool playing_;

    float default_freq_;

    // last midi message
    uint8_t message_type_;
    uint8_t data1_;
    uint8_t data2_;
    uint16_t time_;
};

#endif // MEAP_DRUMRACKENVELOPE_H_
