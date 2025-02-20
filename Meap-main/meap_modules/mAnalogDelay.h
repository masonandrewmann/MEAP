#ifndef MANALOG_DELAY_H_
#define MANALOG_DELAY_H_

template <class T = int16_t>
class mAnalogDelay
{
public:
    mAnalogDelay(uint32_t delay = 0, uint32_t max_delay = 4095)
    {
        max_delay_samples_ = max_delay;
        delay_samples_ = delay;
        delay_buffer_ = (T *)malloc(max_delay_samples_ * sizeof(T));
        for (int i = max_delay_samples_; --i >= 0;)
        {
            delay_buffer_[i] = 0;
        }
        cursor = 0;
        feedback = 0.5;
        delta = 0.5;
    };

    ~mAnalogDelay()
    {
        free(delay_buffer_);
    };

    void setDelta(float d)
    {
        delta = d;
    }

    void setFeedback(float f)
    {
        feedback = f;
    }

    T next(T in_sample)
    {
        T out_sample;
        T x = in_sample;
        T y = delay_buffer_[(int)cursor];

        delay_buffer_[(int)cursor] = x + (T)(y * feedback);

        cursor += delta;

        if (cursor >= max_delay_samples_)
        {
            cursor -= max_delay_samples_;
        }

        out_sample = x + y;

        return out_sample;
    }

protected:
    T *delay_buffer_;
    uint32_t max_delay_samples_; // max delay length in samples

    uint32_t delay_samples_;

    float delta;
    float cursor;
    float feedback;
};

#endif // MANALOG_DELAY_H_
