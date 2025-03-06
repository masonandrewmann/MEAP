#ifndef MSCHROEDERLINEAR_ALLPASS
#define MSCHROEDERLINEAR_ALLPASS

#include <math.h>

template <class T = int32_t>
class mSchroederAllpassLinear
{
public:
    mSchroederAllpassLinear() {
    };

    mSchroederAllpassLinear(int32_t delay_length, int32_t max_delay_length, float g)
    {
        init(delay_length, max_delay_length, g);
    };

    void init(int32_t delay_length, int32_t max_delay_length, float g)
    {
        max_delay_length_ = max_delay_length;
        // delay_buffer_ = (T *)malloc(max_delay_length_ * sizeof(T));
        // for (int i = max_delay_length_; --i >= 0;)
        // {
        //     delay_buffer_[i] = 0;
        // }
        delay_buffer_ = (T *)calloc(max_delay_length_, sizeof(T));
        g_ = g;
        write_pointer_ = 0;
        read_pointer_ = max_delay_length - delay_length;
    };

    ~mSchroederAllpassLinear()
    {
        free(delay_buffer_);
    };

    void setFeedback(float g)
    {
        g_ = g;
    }

    T at(int32_t index)
    {
        int32_t the_index = write_pointer_ - index;
        if (the_index < 0)
        {
            the_index = the_index + max_delay_length_;
        }
        return delay_buffer_[the_index];
    }

    void setDelayLength(float new_length)
    {
        read_pointer_ = write_pointer_ - new_length;
        if (read_pointer_ < 0)
        {
            read_pointer_ += (float)max_delay_length_;
        }
    }

    T next(T in_sample)
    {
        float fake_int;
        float frac_component = modf(read_pointer_, &fake_int);
        int int_component = (int)fake_int;
        int_component = int_component % max_delay_length_;
        int32_t next_sample = (int_component + 1) % max_delay_length_;

        T delay_output = frac_component * delay_buffer_[next_sample] + (1 - frac_component) * delay_buffer_[int_component];
        T middle = in_sample + g_ * delay_output;
        T out_sample = -g_ * middle + delay_output;
        delay_buffer_[write_pointer_] = middle;
        write_pointer_ = (write_pointer_ + 1) % max_delay_length_;
        read_pointer_ = (read_pointer_ + 1);
        if (read_pointer_ >= max_delay_length_)
        {
            read_pointer_ -= max_delay_length_;
        }
        return out_sample;
    }

protected:
    T *delay_buffer_;
    int32_t max_delay_length_;
    float read_pointer_;
    int32_t write_pointer_;

    float g_;
};

#endif // MSCHROEDERLINEAR_ALLPASS
