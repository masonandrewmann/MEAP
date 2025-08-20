#ifndef MDELAYLINE_H_
#define MDELAYLINE_H_

template <class T = int32_t, meap_interpolation INTERP = mINTERP_NONE>
class mDelayLine
{
public:
    mDelayLine() {
    };

    mDelayLine(int32_t max_delay_length_, float delay_length_)
    {
        init(max_delay_length_, delay_length_);
    };

    ~mDelayLine()
    {
        free(delay_buffer);
    }

    void init(int32_t max_delay_length_, float delay_length_)
    {
        max_delay_length = max_delay_length_ + 1;
        delay_buffer = (T *)calloc(max_delay_length, sizeof(T));
        write_pointer = 0;
        read_pointer = max_delay_length - delay_length_;
    };

    void setDelay(float delay_length_)
    {
        read_pointer = (float)write_pointer - delay_length_;
        if (read_pointer < 0)
        {
            read_pointer += max_delay_length;
        }
    }

    T at(float index)
    {
        frac_component = write_pointer - index;
        if (frac_component < 0)
        {
            frac_component += max_delay_length;
        }
        if (INTERP == mINTERP_NONE)
        {
            out_sample = delay_buffer[(int)frac_component];
        }
        else if (INTERP == mINTERP_LINEAR)
        {
            int_component = (int)frac_component;
            frac_component = frac_component - int_component;

            next_sample = (int_component + 1) % max_delay_length;
            out_sample = delay_buffer[int_component] + frac_component * (delay_buffer[next_sample] - delay_buffer[int_component]);
        }
        return out_sample;
    }

    T next(T in_sample)
    {
        if (INTERP == mINTERP_NONE)
        {
            out_sample = delay_buffer[(int)read_pointer];
            delay_buffer[write_pointer] = in_sample;
        }
        else if (INTERP == mINTERP_LINEAR)
        {
            int_component = (int)read_pointer;
            frac_component = read_pointer - int_component;
            next_sample = (int_component + 1) % max_delay_length;
            out_sample = delay_buffer[int_component] + frac_component * (delay_buffer[next_sample] - delay_buffer[int_component]);

            delay_buffer[write_pointer] = in_sample;
        }
        read_pointer++;
        if (read_pointer >= max_delay_length)
        {
            read_pointer -= max_delay_length;
        }

        write_pointer = (write_pointer + 1) % max_delay_length;
        return out_sample;
    }

    T *delay_buffer;
    int32_t max_delay_length;
    float read_pointer;
    int32_t write_pointer;
    float fake_int;
    float frac_component;
    int32_t next_sample;
    int32_t int_component;
    T out_sample;
};

#endif // MDELAYLINE_H_