// an IIR comb filter with a linearly interpolated delay line

#ifndef MANALOGDELAY_H_
#define MANALOGDELAY_H_

#include <math.h>

template <class T = int32_t>
class mAnalogDelay
{
public:
    mAnalogDelay(float delay = 0, uint32_t max_delay = 4095, float feedback = 0.5, float mix = 0.5)
    {
        max_delay_samples_ = max_delay;
        delay_buffer = (T *)calloc(max_delay_samples_, sizeof(T));
        mix_ = mix;
        feedback_ = feedback;

        read_pointer = 0;
        write_pointer = 0;
        increment = 1;
        setDelay(delay);
        last_output = 0;
    };

    ~mAnalogDelay()
    {
        free(delay_buffer);
    };

    void setDelay(int32_t delay_)
    {
        delay = delay_;
    }

    // sets delay as a number of milliseconds
    int32_t setDelayMS(float ms)
    {
        setDelay(ms * (float)AUDIO_RATE * 0.001);
        return delay;
    }

    // sets delay as a number of sixteenth notes at a tempo
    // returns delay time in samples
    int32_t setDelaySixteenths(uint16_t num_sixteenths, uint16_t bpm)
    {
        setDelay(num_sixteenths * (15.f / (float)bpm) * ((float)AUDIO_RATE));
        return delay;
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
            delay_buffer[i] = 0;
        }
    }

    T next(T in_sample)
    {

        int_component = (int)read_pointer;
        frac_component = read_pointer - int_component;
        next_sample = (int_component + 1) % max_delay_samples_;

        // out_sample = delay_buffer[int_component] + frac_component * (delay_buffer[next_sample] - delay_buffer[int_component]); // linear

        // out_sample = delay_buffer[next_sample] + (1.f - frac_component) * delay_buffer[int_component] - (1.f - frac_component) * last_output; // incorrect attempt at allpass (sounds cool tho)
        out_sample = frac_component * (delay_buffer[next_sample] - delay_buffer[int_component]) + delay_buffer[int_component];
        last_output = out_sample;

        diff = write_pointer - read_pointer;
        while (diff < 0)
        {
            diff += max_delay_samples_;
        }

        increment = 1.0f + (diff - delay) / delay;

        read_pointer += increment;

        while (read_pointer >= max_delay_samples_)
        {
            read_pointer -= max_delay_samples_;
        }

        delay_buffer[write_pointer] = in_sample + feedback_ * out_sample;
        write_pointer = (write_pointer + 1) % max_delay_samples_;

        out_sample = out_sample * mix_ + (1 - mix_) * in_sample;
        return out_sample;
    }

    // CLASS VARIABLES

    T *delay_buffer;
    uint32_t max_delay_samples_; // max delay length in samples
    T out_sample;
    T last_output;

    int32_t write_pointer;
    float read_pointer;
    float increment;
    float delay;
    float diff;

    uint32_t int_component;
    uint32_t next_sample;
    float frac_component;

    float mix_;
    float feedback_;
};

#endif // MANALOGDELAY_H_