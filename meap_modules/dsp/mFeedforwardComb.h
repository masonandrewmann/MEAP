#ifndef M_FEEDFORWARD_COMB_H_
#define M_FEEDFORWARD_COMB_H_

#include <math.h>

template <class T = int32_t, meap_interpolation INTERP = mINTERP_NONE>

class mFeedforwardComb
{
public:
    mFeedforwardComb(uint32_t max_delay, float delay, float feedforward = 0.5)
    {
        init(max_delay, delay, feedforward);
    };

    mFeedforwardComb() {

    };

    void init(uint32_t max_delay, float delay, float feedforward = 0.5)
    {
        max_delay_samples_ = max_delay;
        delay_samples_ = delay;

        delay_buffer_ = (T *)calloc(max_delay_samples_, sizeof(T));
        feedforward_ = feedforward;
        write_pointer_ = 0;
        read_pointer_ = ((float)write_pointer_ + ((float)max_delay_samples_ - delay_samples_));
    };

    ~mFeedforwardComb()
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

    void setFeedforward(float feedforward)
    {
        feedforward_ = feedforward;
    }

    T next(T in_sample)
    {
        if (INTERP == mINTERP_NONE)
        {
            out_sample = in_sample + feedforward_ * delay_buffer_[(int)read_pointer_];
            delay_buffer_[write_pointer_] = in_sample;
        }
        else if (INTERP == mINTERP_LINEAR)
        {

            int_component = (int)read_pointer_;
            frac_component = read_pointer_ - int_component;
            next_sample = (int_component + 1) % max_delay_samples_;

            out_sample = in_sample + delay_buffer_[int_component] + frac_component * (delay_buffer_[next_sample] - delay_buffer_[int_component]);
            delay_buffer_[write_pointer_] = in_sample;
        }
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

    T out_sample;

    float fake_int;
    float frac_component;
    int32_t next_sample;
    int32_t int_component;

    float feedforward_;
};

#endif // M_FEEDFORWARD_COMB_H_