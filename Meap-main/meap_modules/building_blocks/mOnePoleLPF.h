
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
        last_val_ = 0;
    }

    void setDamping(float damping)
    {
        damping_ = damping;
        d1_ = 1 - damping;
    }

    T next(T in_sample)
    {
        T out_sample = in_sample * d1_ + last_val_ * damping_;
        last_val_ = out_sample;
        return out_sample;
    }

protected:
    float damping_;
    float d1_;
    T last_val_;
};

#endif // MONEPOLELPF_H_
