// an IIR comb filter with a linearly interpolated delay line and a 1st order lowpass filter in the feedback loop

#ifndef MSCHROEDER_ALLPASS
#define MSCHROEDER_ALLPASS

#include <math.h>

template <class T = int32_t>
class mSchroederAllpass
{
public:
    mSchroederAllpass() {
    };

    mSchroederAllpass(uint32_t delay_length, float g)
    {
        init(delay_length, g);
    };

    void init(uint32_t delay_length, float g)
    {
        delay_length_ = delay_length;
        // delay_buffer_ = (T *)malloc(delay_length_ * sizeof(T));
        // for (int i = delay_length_; --i >= 0;)
        // {
        //     delay_buffer_[i] = 0;
        // }
        delay_buffer_ = (T *)calloc(delay_length_, sizeof(T));
        BL_ = -g;
        FB_ = g;
        rw_pointer_ = 0;
    };

    ~mSchroederAllpass()
    {
        free(delay_buffer_);
    };

    void setFeedback(float g)
    {
        BL_ = -g;
        FB_ = g;
    }

    T at(uint32_t index)
    {
        int32_t the_index = rw_pointer_ - index;
        if (the_index < 0)
        {
            the_index = the_index + delay_length_;
        }
        return delay_buffer_[the_index];
    }

    T next(T in_sample)
    {
        T xh = in_sample + FB_ * delay_buffer_[rw_pointer_];
        T out_sample = BL_ * xh + delay_buffer_[rw_pointer_];
        delay_buffer_[rw_pointer_] = xh;
        rw_pointer_ = (rw_pointer_ + 1) % delay_length_;
        return out_sample;
    }

protected:
    T *delay_buffer_;
    int32_t delay_length_; // max delay length in samples
    float delay_samples_;
    int32_t rw_pointer_;

    float BL_;
    float FB_;
};

#endif // MSCHROEDER_ALLPASS
