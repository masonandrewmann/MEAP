#ifndef MFLANGER_H_
#define MFLANGER_H_

template <class T = int32_t>
class mFlanger
{
public:
    mFlanger()
    {
        max_delay_length = 200;
        delay_length = 100;
        delay_buffer = (T *)calloc(max_delay_length, sizeof(T));
        read_index = 0;
        write_index = 100;
        gain = 0.7;
        feedback = 0.7;
        mix = 0.5;
        mod_depth = 1.f;
        lfo.init(0.2, 98);
    }

    void setGain(float gain_)
    {
        gain = gain_;
    }

    // 0 to 1
    void setFeedback(float feedback_)
    {
        feedback = feedback_;
    }

    // in Hz
    void setModFreq(float freq)
    {
        lfo.setFreq(freq);
    }

    // 0 to 1
    void setModDepth(float depth_)
    {
        mod_depth = depth_;
    }

    // 0 to 1
    void setMix(float mix_)
    {
        mix = mix_;
    }

    T next(T in_sample)
    {

        // update delay length from LFO
        delay_length = 100 + mod_depth * lfo.next();
        // calculate read index (with modulo)
        read_index = write_index - delay_length;
        if (read_index < 0)
        {
            read_index += max_delay_length;
        }

        frac_component = modf(read_index, &fake_int);
        int_component = (int)fake_int;
        // linear interpolation
        next_sample = (int_component + 1) % max_delay_length;
        delay_output = delay_buffer[int_component] + frac_component * (delay_buffer[next_sample] - delay_buffer[int_component]);
        T out_sample = in_sample + gain * delay_output;

        // write to delay line
        delay_buffer[write_index] = in_sample + feedback * delay_output;
        // increment write index
        write_index = (write_index + 1) % max_delay_length;
        return out_sample * mix + (1.f - mix) * in_sample;
    }

    T *delay_buffer;
    T delay_output;
    float delay_length;
    int32_t max_delay_length;
    int32_t write_index;
    float read_index;
    float fake_int;
    float frac_component;
    int32_t int_component;
    int32_t next_sample;

    float gain;
    float feedback;
    float mix;
    float mod_depth;

    mSineLFO<AUDIO_RATE> lfo;
};

#endif // MFLANGER_H_