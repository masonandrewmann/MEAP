// Basic karplus-strong string model

#ifndef MSTRING_H_
#define MSTRING_H_

#include <math.h>
#include <Meap.h>
#include <mozzi_midi.h>

// template for audio vs control rate
template <class T = int32_t>
class mString
{
public:
    mString() {

    };

    mString(float freq, float lowest_freq)
    {
        init(freq, lowest_freq);
    };

    ~mString()
    {
        free(delay_buffer_);
    };

    void init(float freq, float lowest_freq)
    {
        lowest_freq_ = lowest_freq;
        max_delay_length_ = (float)AUDIO_RATE / lowest_freq_;
        delay_buffer_ = (T *)calloc((int)max_delay_length_, sizeof(T));
        write_pointer_ = 0;
        setFreq(freq);
        setLoopGain(0.999);

        filter_sample_ = 0;
    }

    void setFreq(float freq)
    {
        if (freq < lowest_freq_)
        {
            freq = lowest_freq_;
        }
        delay_length_ = (float)AUDIO_RATE / freq - 0.5;
        read_pointer_ = write_pointer_ - delay_length_;
        if (read_pointer_ < 0)
        {
            read_pointer_ += max_delay_length_;
        }
    }

    void noteOn(int note, int32_t velocity)
    {
        setFreq(mtof(note));
        trigger(velocity);
    }

    // 0 - 127
    void trigger(int velocity)
    {
        velocity = velocity << 7;
        T last_sample = 0;
        for (int i = max_delay_length_; --i >= 0;)
        {
            delay_buffer_[i] = 0.1 * Meap::irand(-velocity, velocity) + 0.9 * last_sample;
            last_sample = delay_buffer_[i];
        }
    }

    void setLoopGain(float loop_gain)
    {
        loop_gain_ = loop_gain;
    }

    float next()
    {
        float fake_int;
        float frac_component = modf(read_pointer_, &fake_int);
        int int_component = (int)fake_int;
        int_component = int_component % max_delay_length_;
        int32_t next_sample = (int_component + 1) % max_delay_length_;

        T delay_output = delay_buffer_[int_component] + frac_component * (delay_buffer_[next_sample] - delay_buffer_[int_component]);

        T out_sample = (delay_output >> 1) + (filter_sample_ >> 1);
        filter_sample_ = delay_output;
        delay_buffer_[write_pointer_] = out_sample * loop_gain_;
        write_pointer_ = (write_pointer_ + 1) % max_delay_length_;
        read_pointer_ = read_pointer_ + 1;
        if (read_pointer_ >= max_delay_length_)
        {
            read_pointer_ -= max_delay_length_;
        }
        return out_sample;
    }

    // CLASS VARIABLES

    float lowest_freq_;
    int32_t delay_length_;
    int32_t max_delay_length_;
    T *delay_buffer_;
    float read_pointer_;
    int32_t write_pointer_;
    T filter_sample_;
    float loop_gain_;
};
#endif // MSTRING_H_