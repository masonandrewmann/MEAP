#ifndef MPULSE_H_
#define MPULSE_H_

template <uint32_t UPDATE_RATE = AUDIO_RATE>
class mPulse
{
public:
    mPulse(float freq_, float width_)
    {
    }

    void setWidth()
    {
    }

    void setFreq()
    {
    }

    int32_t next()
    {
        int32_t out_sample;
        return out_sample;
    }

    // CLASS VARIABLES
    float width;
    float freq;
};

#endif // MPULSE_H_
