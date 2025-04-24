// an IIR comb filter with a linearly interpolated delay line and a 1st order lowpass filter in the feedback loop

#ifndef MDIGITALDELAY_H_
#define MDIGITALDELAY_H_

#include <math.h>

template <class T = int32_t>
class mDigitalDelay
{
public:
    mDigitalDelay(float delay = 0, uint32_t max_delay = 4095, float feedback = 0.5, float mix = 0.5)
    {
        init(delay, max_delay, feedback, mix);
    };

    mDigitalDelay() {

    };

    void init(float delay = 0, uint32_t max_delay = 4095, float feedback = 0.5, float mix = 0.5)
    {
        max_delay_samples_ = max_delay;
        delay_samples_ = delay;
        delay_buffer_ = (T *)calloc(max_delay_samples_, sizeof(T));
        mix_ = mix;
        feedback_ = feedback;
        write_pointer_ = 0;
        read_pointer_ = ((float)write_pointer_ + ((float)max_delay_samples_ - delay_samples_));
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

    void setFeedback(float feedback)
    {
        feedback_ = feedback;
    }

    void setMix(float mix)
    {
        mix_ = mix;
    }

    T next(T in_sample)
    {
        float fake_int;
        float frac_component = modf(read_pointer_, &fake_int);
        int int_component = (int)fake_int;
        int next_index = int_component + 1;
        if (next_index > max_delay_samples_)
        {
            next_index -= max_delay_samples_;
        }

        // T out_sample = frac_component * delay_buffer_[next_index] + (1 - frac_component) * delay_buffer_[int_component];
        T out_sample = delay_buffer_[int_component] + frac_component * (delay_buffer_[next_index] + delay_buffer_[int_component]);

        // delay_buffer_[write_pointer_] = (out_sample * feedback_) * lpf_in_ + lpf_mem_ * lpf_fb_; // lowpassed feedback
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

    T lpf_mem_;

    float mix_;
    float feedback_;
};

#endif // MDIGITALDELAY_H_