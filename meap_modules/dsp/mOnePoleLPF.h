
#ifndef MONEPOLELPF_H_
#define MONEPOLELPF_H_

#include <math.h>

template <class T = int32_t>
class mOnePoleLPF
{
public:
    mOnePoleLPF()
    {
    }

    mOnePoleLPF(float damping)
    {
        init(damping);
    }

    void init(float damping)
    {
        damping_ = damping;
        d1_ = 1 - damping;
        out_sample = 0;
    }

    void setDamping(float damping)
    {
        damping_ = damping;
        d1_ = 1 - damping;
    }

    T next(T in_sample)
    {
        out_sample = in_sample * d1_ + out_sample * damping_;
        return out_sample;
    }

    // CLASS VARIABLES
    float damping_;
    float d1_;
    T out_sample;
};

#endif // MONEPOLELPF_H_