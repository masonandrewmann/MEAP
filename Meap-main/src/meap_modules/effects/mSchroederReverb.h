// a schroeder-style reverberator. four parallel combs with one-pole LPFs in the feedback loop followed by two series allpasses

#ifndef MSCHROEDER_REVERB
#define MSCHROEDER_REVERB

template <class T = int16_t>
class mSchroederReverb
{
public:
    mSchroederReverb(float feedback = 0.5, float damping = 0.5)
    {
        ap1_.init(167, 0.5);       // 225
        ap2_.init(413, 0.5);       // 556
        c1_.init(1157, 0.97, 0.5); // 1557
        c2_.init(1201, 0.97, 0.5); // 1617
        c3_.init(1108, 0.97, 0.5); // 1491
        c4_.init(1057, 0.97, 0.5); // 1422

        c1_.setFeedback(feedback);
        c2_.setFeedback(feedback);
        c3_.setFeedback(feedback);
        c4_.setFeedback(feedback);

        c1_.setDamping(damping);
        c2_.setDamping(damping);
        c3_.setDamping(damping);
        c4_.setDamping(damping);
    };

    void setFeedback(float feedback)
    {
        c1_.setFeedback(feedback);
        c2_.setFeedback(feedback);
        c3_.setFeedback(feedback);
        c4_.setFeedback(feedback);
    }

    void setDamping(float damping)
    {
        c1_.setDamping(damping);
        c2_.setDamping(damping);
        c3_.setDamping(damping);
        c4_.setDamping(damping);
    }

    T next(T in_sample)
    {
        T out_sample = c1_.next(in_sample) + c2_.next(in_sample) + c3_.next(in_sample) + c4_.next(in_sample);
        out_sample = ap1_.next(out_sample);
        return ap2_.next(out_sample);
    };

    // CLASS VARIABLES

    mSchroederAllpass<T> ap1_;
    mSchroederAllpass<T> ap2_;

    mNaturalComb<T> c1_;
    mNaturalComb<T> c2_;
    mNaturalComb<T> c3_;
    mNaturalComb<T> c4_;
};

#endif // MSCHROEDER_REVERB
