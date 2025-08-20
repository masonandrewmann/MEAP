#ifndef MSCHROEDERLINEAR_ALLPASS
#define MSCHROEDERLINEAR_ALLPASS

#include <math.h>

template <class T = int32_t, meap_interpolation INTERP = mINTERP_NONE>
class mSchroederAllpass
{
public:
    mSchroederAllpass() {
    };

    mSchroederAllpass(int32_t max_delay_length, float delay_length, float g)
    {
        init(max_delay_length, delay_length, g);
    };

    void init(int32_t max_delay_length, float delay_length, float g)
    {
        max_delay_length_ = max_delay_length + 1;
        delay_buffer_ = (T *)calloc(max_delay_length_, sizeof(T));
        g_ = g;
        write_pointer_ = 0;
        read_pointer_ = max_delay_length_ - delay_length;
    };

    ~mSchroederAllpass()
    {
        free(delay_buffer_);
    };

    void setFeedback(float g)
    {
        g_ = g;
    }

    T at(float index)
    {
        float the_index = write_pointer_ - index;
        if (the_index < 0)
        {
            the_index += max_delay_length_;
        }

        if (INTERP == mINTERP_NONE)
        {
            return delay_buffer_[(int)the_index];
        }
        else if (INTERP == mINTERP_LINEAR)
        {
            int_component = (int)the_index;
            frac_component = the_index - int_component;

            int_component = int_component % max_delay_length_;
            next_sample = (int_component + 1) % max_delay_length_;

            return delay_buffer_[int_component] + frac_component * (delay_buffer_[next_sample] - delay_buffer_[int_component]);
        }
    }

    void setDelay(float delay_samples)
    {
        read_pointer_ = write_pointer_ - delay_samples;
        if (read_pointer_ < 0)
        {
            read_pointer_ += (float)max_delay_length_;
        }
    }

    T next(T in_sample)
    {
        if (INTERP == mINTERP_NONE)
        {
            out_sample = delay_buffer_[(int)read_pointer_];
            delay_buffer_[write_pointer_] = in_sample + g_ * out_sample;
            out_sample = -g_ * delay_buffer_[write_pointer_] + out_sample;
        }
        else if (INTERP == mINTERP_LINEAR)
        {
            int_component = (int)read_pointer_;
            frac_component = read_pointer_ - int_component;

            int_component = int_component % max_delay_length_;
            next_sample = (int_component + 1) % max_delay_length_;

            out_sample = delay_buffer_[int_component] + frac_component * (delay_buffer_[next_sample] - delay_buffer_[int_component]);

            delay_buffer_[write_pointer_] = in_sample + g_ * out_sample;
            out_sample = -g_ * delay_buffer_[write_pointer_] + out_sample;
        }

        write_pointer_ = (write_pointer_ + 1) % max_delay_length_;
        read_pointer_ = (read_pointer_ + 1);
        if (read_pointer_ >= max_delay_length_)
        {
            read_pointer_ -= max_delay_length_;
        }
        return out_sample;
    }

    // CLASS VARIABLES
    T *delay_buffer_;
    int32_t max_delay_length_;
    float read_pointer_;
    int32_t write_pointer_;

    float fake_int;
    float frac_component;
    int32_t next_sample;
    int int_component;
    T out_sample;

    float g_;
};

#endif // MSCHROEDERLINEAR_ALLPASS
