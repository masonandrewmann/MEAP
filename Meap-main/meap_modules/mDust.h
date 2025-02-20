#ifndef MDUST_H_
#define MDUST_H_

template <uint32_t UPDATE_RATE = CONTROL_RATE>
class mDust
{
public:
    mDust()
    {
        sample_dur = 1.f / (float)UPDATE_RATE;
        setDensity(1);
    };

    mDust(float density_)
    {
        sample_dur = 1.f / (float)UPDATE_RATE;
        setDensity(density_);
    }

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
        int32_t out_sample;
        float sample = Meap::frand(); // 0 to 1

        if (sample < thresh)
        {
            out_sample = sample * scale;
        }
        else
        {
            out_sample = 0;
        }

        return out_sample;
    }

protected:
    float thresh;
    float scale;
    float sample_dur;
};

#endif // MDUST_H_
