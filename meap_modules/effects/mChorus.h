#ifndef MCHORUS_H_
#define MCHORUS_H_

template <class T = int32_t>
class mChorus
{
public:
    mChorus()
    {
        delay_buffer1 = (T *)calloc(3200, sizeof(T));
        delay_buffer2 = (T *)calloc(3200, sizeof(T));

        lfo1.init(0.2, 1.f);
        lfo2.init(0.22, 1.f);

        mod_depth = 0.05;
        mix = 0.5;
        write_index = 0;
    }

    void setModFreq(float freq)
    {
        lfo1.setFreq(freq);
        lfo2.setFreq(freq * 1.1111f);
    }

    void setModDepth(float depth_)
    {
        mod_depth = depth_;
    }

    T next(T in_sample)
    {
        // calculate L sample

        float deviation = 1500 * (1.f + mod_depth * lfo1.next());
        read_index = write_index - deviation;
        // Serial.println(deviation);
        if (read_index < 0)
        {
            read_index += 3200;
        }

        frac_component = modf(read_index, &fake_int);
        int_component = (int)fake_int;
        // linear interpolation
        next_sample = (int_component + 1) % 3200;

        l_sample = mix * (delay_buffer1[int_component] + frac_component * (delay_buffer1[next_sample] - delay_buffer1[int_component])) + (1.f - mix) * in_sample;

        // calculate R sample
        read_index = write_index - 1125 * (1.f - mod_depth * lfo2.next());
        if (read_index < 0)
        {
            read_index += 3200;
        }

        frac_component = modf(read_index, &fake_int);
        int_component = (int)fake_int;
        // linear interpolation
        next_sample = (int_component + 1) % 3200;

        r_sample = mix * (delay_buffer2[int_component] + frac_component * (delay_buffer2[next_sample] - delay_buffer2[int_component])) + (1.f - mix) * in_sample;

        delay_buffer1[write_index] = in_sample;
        delay_buffer2[write_index] = in_sample;
        write_index = (write_index + 1) % 3200;
        return l_sample;
    }

    T *delay_buffer1;
    T *delay_buffer2;

    mSineLFO<AUDIO_RATE> lfo1;
    mSineLFO<AUDIO_RATE> lfo2;

    float mix;
    float mod_depth;

    T l_sample;
    T r_sample;

    int32_t write_index;
    int32_t read_index;
    float fake_int;
    float frac_component;
    int32_t int_component;
    int32_t next_sample;
};
#endif // MCHORUS_H_