#ifndef MRESONZ_H_
#define MRESONZ_H_

class mResonz
{
public:
    mResonz(float freq, float rq)
    {
        init(freq, rq);
    };

    void init(float freq, float rq)
    {
        m_b1 = 0.f;
        m_b2 = 0.f;
        m_y1 = 0;
        m_y2 = 0;
        m_x1 = 0;
        m_x2 = 0;
        m_freq = 1000;
        // m_decay_time = 0.f;
        m_rq = 0;

        radians_per_sample = 6.28318530717958 / AUDIO_RATE;

        updateParameters(freq, rq);
    }

    // center freq and rq = bandwidth / centerFreq (reciprocal of Q)
    void updateParameters(float freq, float rq)
    {
        float ffreq = freq * radians_per_sample; // 2pi/AUDIORATE
        float B = ffreq * rq;
        float R = 1.f - B * 0.5f;
        float twoR = 2.f * R;
        float R2 = R * R;
        float cost = (twoR * cos(ffreq)) / (1.f + R2);
        m_b1 = twoR * cost;
        m_b2 = -R2;

        m_freq = freq;
        m_rq = rq;
    }

    int32_t next(int32_t in_sample)
    {
        int32_t out_sample;

        // maybe update parameters her
        out_sample = in_sample - m_x2 + m_b1 * m_y1 + m_b2 * m_y2;

        m_y2 = m_y1;
        m_y1 = out_sample;

        m_x2 = m_x1;
        m_x1 = in_sample;

        return out_sample;
    }

    // CLASS VARIABLES
    float m_freq;
    float m_decay_time;
    float m_rq;

    int32_t m_y1;
    int32_t m_y2;

    int32_t m_x1;
    int32_t m_x2;

    float m_b1;
    float m_b2;

    float radians_per_sample;
};

#endif // MRESONZ_H_