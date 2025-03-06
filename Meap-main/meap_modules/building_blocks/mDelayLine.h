#ifndef MDELAYLINE_H_
#define MDELAYLINE_H_

template <class T = int32_t>
class mDelayLine
{
public:
    mDelayLine() {
    };

    mDelayLine(int32_t delay_length)
    {
        init(delay_length);
    };

    void init(int32_t delay_length)
    {
        delay_length_ = delay_length;
        // delay_buffer_ = (T *)malloc(delay_length_ * sizeof(T));
        // for (int i = delay_length_; --i >= 0;)
        // {
        //     delay_buffer_[i] = 0;
        // }
        delay_buffer_ = (T *)calloc(delay_length_, sizeof(T));
        rw_pointer_ = 0;
    };

    T at(int32_t index)
    {
        int32_t the_index = rw_pointer_ - index;
        if (the_index < 0)
        {
            the_index += delay_length_;
        }
        return delay_buffer_[the_index];
    }

    T next(T in_sample)
    {
        T out_sample = delay_buffer_[rw_pointer_];
        delay_buffer_[rw_pointer_] = in_sample;
        rw_pointer_ = (rw_pointer_ + 1) % delay_length_;
        return out_sample;
    }

protected:
    T *delay_buffer_;
    int32_t delay_length_;
    int32_t rw_pointer_;
};

#endif // MDELAYLINE_H_