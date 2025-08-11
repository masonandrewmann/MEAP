#ifndef MDUST_H_
#define MDUST_H_

#include <Meap.h>

template <uint32_t UPDATE_RATE = CONTROL_RATE>
class mDust
{
public:
    mDust(float density_ = 1)
    {
        sample_dur = 1.f / (float)UPDATE_RATE;
        setDensity(density_);
    }

    // average number of impulses per second
    void setDensity(float density_)
    {
        thresh = sample_dur * density_;
        if (thresh > 0)
        {
            scale = 255.f / thresh; // scales output to range 0-255
        }
        else
        {
            scale = 0;
        }
    }

    int32_t next()
    {
        int32_t out_sample = 0;
        float sample = Meap<MEAP_DEFAULT_VERSION>::frand(); // 0 to 1

        if (sample < thresh)
        {
            out_sample = sample * scale;
        }
        return out_sample;
    }

    // CLASS VARIABLES
    float thresh;
    float scale;
    float sample_dur;
};

#endif // MDUST_H_
