#ifndef MPLUCKVOICE_H_
#define MPLUCKVOICE_H_

class mPluckVoice
{
public:
    mPluckVoice(float lowest_freq)
    {
        max_delay_length = (float)AUDIO_RATE / lowest_freq;
        delay_buffer = (int32_t *)calloc((int32_t)max_delay_length, sizeof(int32_t));
        write_pointer_ = 0;
        setFreq(lowest_freq * 2);
        setLoopGain(0.999);
        env.setADLevels(255, 0);
        env.setTimes(1, 100, 1, 1);
    };

    void noteOn(uint16_t note, uint16_t vel)
    {
        setFreq(mtof(note));
        trigger(vel);
    }

    void noteOff(uint16_t note)
    {
    }

    void setFreq(float freq)
    {
        delay_length_ = (float)AUDIO_RATE / freq; // aka fqs
        if (delay_length >= max_delay_length)
        {
            delay_length = max_delay_length - 1;
        }
        read_pointer_ = write_pointer_ - delay_length_;
        if (read_pointer_ < 0)
        {
            read_pointer_ += max_delay_length_;
        }
    }

    void trigger(int velocity)
    {
        env.noteOn();
    }

    // to be called in audio loop
    int32_t next()
    {
        int32_t out_sample = 0;
        return out_sample;
    }

    // CLASS VARIABLES
    mNoise noise;
    ADSR<AUDIO_RATE, AUDIO_RATE> env;
    int32_t *delay_buffer;
    float max_delay_length;
};

#endif // MPLUCKVOICE_H_