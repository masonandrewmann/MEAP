// an IIR comb filter with a linearly interpolated delay line and a 1st order lowpass filter in the feedback loop

#ifndef MNAT_COM_LIN
#define MNAT_COM_LIN

#include <math.h>

template <class T = int32_t>
class mNaturalCombLinear
{
public:
    mNaturalCombLinear(float delay = 0, uint32_t max_delay = 4095, float feedback = 0.5, float damping = 0.5)
    {
        init(delay, max_delay, feedback, damping);
    };

    mNaturalCombLinear() {

    };

    void init(float delay = 0, uint32_t max_delay = 4095, float feedback = 0.5, float damping = 0.5)
    {
        max_delay_samples_ = max_delay;
        delay_samples_ = delay;
        // delay_buffer_ = (T *)malloc((max_delay_samples_ + 1) * sizeof(T));
        // for (int i = max_delay_samples_; --i >= 0;)
        // {
        //     delay_buffer_[i] = 0;
        // }
        delay_buffer_ = (T *)calloc(max_delay_samples_, sizeof(T));
        feedback_ = feedback;
        write_pointer_ = 0;
        read_pointer_ = ((float)write_pointer_ + ((float)max_delay_samples_ - delay_samples_));

        lpf_in_ = 1 - damping;
        lpf_fb_ = damping;

        lpf_mem_ = 0;
    };

    ~mNaturalCombLinear()
    {
        free(delay_buffer_);
    };

    void setDelay(float delay_samples)
    {
        delay_samples_ = delay_samples;
        read_pointer_ = ((float)write_pointer_ + ((float)max_delay_samples_ - delay_samples_));
        if (read_pointer_ > max_delay_samples_)
        {
            read_pointer_ -= max_delay_samples_;
        }
    }

    void setFeedback(float feedback)
    {
        feedback_ = feedback;
    }

    void setDamping(float damping)
    {
        lpf_in_ = 1 - damping;
        lpf_fb_ = damping;
    }

    T next(T in_sample)
    {
        float fake_int;
        float frac_component = modf(read_pointer_, &fake_int);
        int int_component = (int)fake_int;

        T out_sample = in_sample + frac_component * delay_buffer_[int_component + 1] + (1 - frac_component) * delay_buffer_[int_component];

        delay_buffer_[write_pointer_] = (out_sample * feedback_) * lpf_in_ + lpf_mem_ * lpf_fb_; // lowpassed feedback

        lpf_mem_ = delay_buffer_[write_pointer_];

        write_pointer_ = (write_pointer_ + 1) % max_delay_samples_;
        read_pointer_ = read_pointer_ + 1;
        if (read_pointer_ >= max_delay_samples_)
        {
            read_pointer_ -= max_delay_samples_;
        }
        return out_sample;
    }

protected:
    T *delay_buffer_;
    uint32_t max_delay_samples_; // max delay length in samples
    float delay_samples_;
    float read_pointer_;
    uint32_t write_pointer_;

    float lpf_in_;
    float lpf_fb_;
    T lpf_mem_;

    float feedback_;
};

#endif // MNAT_COM_LIN