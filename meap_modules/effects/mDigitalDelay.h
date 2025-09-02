// an IIR comb filter with a linearly interpolated delay line

#ifndef MDIGITALDELAY_H_
#define MDIGITALDELAY_H_

#include <math.h>

template <class T = int32_t>
class mDigitalDelay
{
public:
    mDigitalDelay(float delay = 0, uint32_t max_delay = 4095, float feedback = 0.5, float mix = 0.5)
    {
        max_delay_samples_ = max_delay;
        delay_buffer_ = (T *)calloc(max_delay_samples_, sizeof(T));
        mix_ = mix;
        feedback_ = feedback;
        write_pointer_ = 0;
        setDelay(delay);
    };

    ~mDigitalDelay()
    {
        free(delay_buffer_);
    };

    void setDelay(float delay_samples)
    {
        delay_samples_ = delay_samples;
        read_pointer_ = (float)write_pointer_ - delay_samples;
        if (read_pointer_ < 0)
        {
            read_pointer_ += max_delay_samples_;
        }
    }

    // sets delay as a number of milliseconds
    int32_t setDelayMS(float ms)
    {
        setDelay(ms * (float)AUDIO_RATE * 0.001);
        return delay_samples_;
    }

    // sets delay as a number of sixteenth notes at a tempo
    // returns delay time in samples
    int32_t setDelaySixteenths(uint16_t num_sixteenths, float bpm)
    {
        setDelay(num_sixteenths * (15.f / bpm) * ((float)AUDIO_RATE));
        return delay_samples_;
    }

    void setFeedback(float feedback)
    {
        feedback_ = feedback;
    }

    void setMix(float mix)
    {
        mix_ = mix;
    }

    void clear()
    {
        for (int i = max_delay_samples_; --i >= 0;)
        {
            delay_buffer_[i] = 0;
        }
    }

    T next(T in_sample)
    {
        float fake_int;
        float frac_component = modf(read_pointer_, &fake_int);
        int int_component = (int)fake_int;
        int next_index = int_component + 1;
        if (next_index >= max_delay_samples_)
        {
            next_index -= max_delay_samples_;
        }
        T out_sample = delay_buffer_[int_component] + frac_component * (delay_buffer_[next_index] - delay_buffer_[int_component]);

        delay_buffer_[write_pointer_] = out_sample * feedback_ + in_sample;

        out_sample = out_sample * mix_ + (1 - mix_) * in_sample;

        write_pointer_ = (write_pointer_ + 1) % max_delay_samples_;
        read_pointer_ = read_pointer_ + 1;
        if (read_pointer_ >= max_delay_samples_)
        {
            read_pointer_ -= max_delay_samples_;
        }
        return out_sample;
    }

    // CLASS VARIABLES

    T *delay_buffer_;
    uint32_t max_delay_samples_; // max delay length in samples
    float delay_samples_;
    float read_pointer_;
    uint32_t write_pointer_;

    uint32_t int_component;
    float frac_component;

    float mix_;
    float feedback_;
};

#endif // MDIGITALDELAY_H_