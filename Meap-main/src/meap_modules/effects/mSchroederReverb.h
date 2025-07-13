// a schroeder-style reverberator. four parallel combs with one-pole LPFs in the feedback loop followed by two series allpasses

#ifndef MSCHROEDER_REVERB
#define MSCHROEDER_REVERB

template <class T = int32_t>
class mSchroederReverb
{
public:
    mSchroederReverb(float decay = 0.5, float damping = 0.5, float mix = 0.5)
    {
        mix = mix_;

        ap1_.init(167, 0.5);       // 225
        ap2_.init(413, 0.5);       // 556
        c1_.init(1157, 0.97, 0.5); // 1557
        c2_.init(1201, 0.97, 0.5); // 1617
        c3_.init(1108, 0.97, 0.5); // 1491
        c4_.init(1057, 0.97, 0.5); // 1422

        c1_.setFeedback(decay);
        c2_.setFeedback(decay);
        c3_.setFeedback(decay);
        c4_.setFeedback(decay);

        c1_.setDamping(damping);
        c2_.setDamping(damping);
        c3_.setDamping(damping);
        c4_.setDamping(damping);
    };

    void setDecay(float decay)
    {
        c1_.setFeedback(decay);
        c2_.setFeedback(decay);
        c3_.setFeedback(decay);
        c4_.setFeedback(decay);
    }

    void setDamping(float damping)
    {
        c1_.setDamping(damping);
        c2_.setDamping(damping);
        c3_.setDamping(damping);
        c4_.setDamping(damping);
    }

    void setMix(float mix)
    {
        mix_ = mix;
    }

    T next(T in_sample)
    {
        T out_sample = c1_.next(in_sample) + c2_.next(in_sample) + c3_.next(in_sample) + c4_.next(in_sample);
        out_sample = ap2_.next(ap1_.next(out_sample));
        out_sample = mix_ * out_sample + (1.0 - mix_) * in_sample;
        return out_sample;
    };

    // CLASS VARIABLES

    float mix_;

    mSchroederAllpass<T> ap1_;
    mSchroederAllpass<T> ap2_;

    mNaturalComb<T> c1_;
    mNaturalComb<T> c2_;
    mNaturalComb<T> c3_;
    mNaturalComb<T> c4_;
};

#endif // MSCHROEDER_REVERB
