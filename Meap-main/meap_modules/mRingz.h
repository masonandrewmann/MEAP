#ifndef MRINGZ_H_
#define MRINGZ_H_

class mRingz
{
public:
    mRingz()
    {
        init();
    };

    void init()
    {
        m_b1 = 0.f;
        m_b2 = 0.f;
        m_y1 = 0;
        m_y2 = 0;
        m_freq = 1000;
        m_decay_time = 0.f;

        radians_per_sample = 6.28318530717958 / AUDIO_RATE;
    }

    // center frequency and decay time (RT60) in seconds
    void updateParameters(float freq, float decay_time)
    {
        float ffreq = freq * radians_per_sample;                                                  // 2pi/AUDIORATE
        float R = decay_time == 0.f ? 0.f : exp(-6.907755278982137f / (decay_time * AUDIO_RATE)); // .... log001 = ln(0.001)
        float twoR = 2.f * R;
        float R2 = R * R;
        float cost = (twoR * cos(ffreq)) / (1.f + R2);
        m_b1 = twoR * cost;
        m_b2 = -R2;

        m_freq = freq;
        m_decay_time = decay_time;
    }

    int32_t next(int32_t in_sample)
    {
        int32_t out_sample;

        out_sample = in_sample + m_b1 * m_y1 + m_b2 * m_y2;

        m_y2 = m_y1;
        m_y1 = out_sample;

        return out_sample;
    }

protected:
    float m_freq;
    float m_decay_time;

    int32_t m_y1;
    int32_t m_y2;

    float m_b1;
    float m_b2;

    float radians_per_sample;
};

#endif // MRINGZ_H_