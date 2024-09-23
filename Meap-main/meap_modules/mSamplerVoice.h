#ifndef MEAP_SAMPLER_VOICE_H_
#define MEAP_SAMPLER_VOICE_H_

template <uint64_t mMAX_SAMPLE_LENGTH, class T = int16_t>
class mSamplerVoice
{
public:
    mSamplerVoice(const T *TABLE_NAME, uint64_t TABLE_SIZE, uint8_t *base_address)
    {
        default_freq_ = (float)AUDIO_RATE / (float)mMAX_SAMPLE_LENGTH;
        for (int i = 0; i < 128; i++)
        {
            m_freq_table[i] = default_freq_ * (float)pow(2.f, ((float)(60 - i) * -1.0) / 12.f);
        }
        sample.setTable(TABLE_NAME, TABLE_SIZE);

        base_address_ = base_address;

        playing_ = false;

        message_type_ = 0;
        data1_ = 0;
        data2_ = 0;
        time_ = 0;

        pulse_counter_ = 0;

        transpose_ = 0;
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

    void setTranspose(int8_t transpose)
    {
        transpose_ = transpose;
    }

    // void setTimes(uint64_t a, uint64_t d, uint64_t s, uint64_t r)
    // {
    //     adsr_.setTimes(a, d, s, r);
    // }

    // void setDecayTime(uint64_t d_t)
    // {
    //     adsr_.setDecayTime(d_t);
    // }

    // void setADLevels(int a_t, int d_t)
    // {
    //     adsr_.setADLevels(a_t, d_t);
    // }

    void stop()
    {
        // send noteoffs to everything
        playing_ = false;
        sample.noteOff(255);
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
                    // sample_bank_[data1_].noteOff(255);
                    sample.noteOff(255);
                    break;
                case 0x90: // note on
                    sample.noteOn(m_freq_table[data1_ + transpose_], 255);
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

    void setDecayTime(uint32_t decay)
    {
        sample.setDecayTime(decay);
    }

    void noteOn(float freq)
    {
        sample.noteOn(freq);
    }

    void noteOff()
    {
        sample.noteOff();
    }

    int32_t next()
    {
        int32_t output_sample = sample.next();
        return output_sample;
    }

protected:
    mSampler<mMAX_SAMPLE_LENGTH, T> sample;

    // reading midi file
    uint8_t *base_address_;
    uint8_t *current_address_;
    uint64_t pulse_counter_;

    bool playing_;

    // last midi message
    uint8_t message_type_;
    uint8_t data1_;
    uint8_t data2_;
    uint16_t time_;

    float default_freq_;
    float m_freq_table[128];
    int8_t transpose_;
};

#endif // MEAP_SAMPLER_VOICE_H_
