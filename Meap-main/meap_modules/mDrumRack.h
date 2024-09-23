#ifndef MEAP_DRUMRACK_H_
#define MEAP_DRUMRACK_H_

template <uint64_t mMAX_SAMPLE_LENGTH, uint8_t mNUM_SAMPLES>
class mDrumRack
{
public:
    mDrumRack(const int8_t **sample_list, uint32_t *sample_lengths, uint8_t *base_address)
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

        default_freq_ = (float)AUDIO_RATE / (float)mMAX_SAMPLE_LENGTH;

        for (int i = mNUM_SAMPLES; --i >= 0;)
        {
            sample_gain_[i] = 255;
            sample_bank_[i].setTable(sample_list_[i], sample_lengths_[i]);
        }
    };

    mDrumRack(const int8_t **sample_list, uint32_t *sample_lengths)
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

        default_freq_ = (float)AUDIO_RATE / (float)mMAX_SAMPLE_LENGTH;

        for (int i = mNUM_SAMPLES; --i >= 0;)
        {
            sample_gain_[i] = 255;
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

        default_freq_ = (float)AUDIO_RATE / (float)mMAX_SAMPLE_LENGTH;

        for (int i = mNUM_SAMPLES; --i >= 0;)
        {
            sample_gain_[i] = 255;
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
        for (int i = mNUM_SAMPLES; --i >= 0;)
        {
            sample_bank_[i].noteOff(255);
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
                // case 0x80: // note off
                //     break;
                case 0x90: // note on
                    noteOn(data1_, data2_, default_freq_);
                    break;
                case 255: // end of file
                    playing_ = false;
                    return;
                    break;
                }
                pulse_counter_ = 0;

                current_address_ += 5;
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

    void noteOn(uint16_t sample_num, uint16_t vel, float freq)
    {
        sample_bank_[sample_num].noteOn(freq, ((vel * sample_gain_[sample_num]) >> 8));
    }

    void noteOn(uint16_t sample_num)
    {
        sample_bank_[sample_num].noteOn(default_freq_, sample_gain_[sample_num] >> 1);
    }

    void noteOff(uint16_t sample_num)
    {
        sample_bank_[sample_num].noteOff();
    }

    void setSampleVolume(uint8_t sample_num, float volume)
    {
        sample_gain_[sample_num] = volume;
    }

    void update()
    {
        for (int i = mNUM_SAMPLES; --i >= 0;)
        {
            sample_bank_[i].update();
        }
    }

    int32_t next()
    {
        uint64_t output_sample = 0;
        for (int i = mNUM_SAMPLES; --i >= 0;)
        {
            // output_sample += (sample_bank_[i].next() * sample_gain_[i]);
            output_sample += sample_bank_[i].next();
        }
        return output_sample;
    }

protected:
    // sampler voices
    mSampler<mMAX_SAMPLE_LENGTH, int8_t> sample_bank_[mNUM_SAMPLES];
    const int8_t **sample_list_;
    uint32_t *sample_lengths_;
    uint16_t sample_gain_[mNUM_SAMPLES];

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

#endif // MEAP_DRUMRACK_H_
