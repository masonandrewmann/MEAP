// an IIR comb filter with a linearly interpolated delay line and a 1st order lowpass filter in the feedback loop

#ifndef MNAT_COM
#define MNAT_COM

#include <math.h>

template <class T = int32_t>
class mNaturalComb
{
public:
    mNaturalComb() {
    };

    mNaturalComb(float delay, float feedback, float damping)
    {
        init(delay, feedback, damping);
    };

    void init(float delay, float feedback, float damping)
    {
        delay_samples_ = delay;
        delay_buffer_ = (T *)malloc((delay_samples_) * sizeof(T));
        for (int i = delay_samples_; --i >= 0;)
        {
            delay_buffer_[i] = 0;
        }
        feedback_ = feedback;
        rw_pointer_ = 0;

        lpf_in_ = 1 - damping;
        lpf_fb_ = damping;

        lpf_mem_ = 0;
    }

    ~mNaturalComb()
    {
        free(delay_buffer_);
    };

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
        T out_sample = in_sample + delay_buffer_[rw_pointer_];
        delay_buffer_[rw_pointer_] = (out_sample * feedback_) * lpf_in_ + lpf_mem_ * lpf_fb_; // lowpassed feedback
        lpf_mem_ = delay_buffer_[rw_pointer_];
        rw_pointer_ = (rw_pointer_ + 1) % delay_samples_;
        return out_sample;
    }

    // CLASS VARIABLES
    T *delay_buffer_;
    uint32_t delay_samples_;
    uint32_t rw_pointer_;

    float lpf_in_;
    float lpf_fb_;
    T lpf_mem_;

    float feedback_;
};

#endif // MNAT_COM