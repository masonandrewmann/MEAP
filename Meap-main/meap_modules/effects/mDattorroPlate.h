// implements the plate reverb described by Jon Dattorro in his paper "Effect Design Part 1: Reverberator and Other Filters"
#ifndef MDATTORRO_PLATE_H_
#define MDATTORRO_PLATE_H_

#include <meap_modules/generators/mSineLFO.h>

template <class T = int32_t>
class mDattorroPlate
{
public:
    mDattorroPlate() {
    };

    mDattorroPlate(float g5, float damping, float bandwidth)
    {
        init(g5, damping, bandwidth);
    };

    void setDamping(float d)
    {
        lpf2.setDamping(d);
        lpf3.setDamping(d);
    }

    void setBandwidth(float bw)
    {
        lpf1.setDamping(1 - bw);
    }

    void setDecay(float g)
    {
        g5_ = g;
    }

    void init(float g5, float damping, float bandwidth)
    {
        g5_ = g5;

        pre_delay.init(372); // 500
        lpf1.init(1 - bandwidth);
        apf1.init(156, 0.75);  // 210
        apf2.init(117, 0.75);  // 158
        apf3.init(417, 0.625); // 561
        apf4.init(305, 0.625); // 410

        mapf1.init(998, 1050, 0.7); // 1343
        delay1.init(4637);          // 6241
        lpf2.init(damping);
        apf5.init(2921, 0.5); // 3931
        delay2.init(3478);    // 4681

        mapf2.init(739, 1100, 0.7); // 995
        delay3.init(4897);          // 6590 -------3989 doesnt fit!!!!!!!!!!!!!!!!!!!!!!!!!!!
        lpf3.init(damping);
        apf6.init(1797, 0.5); // 2664
        delay4.init(4090);    // 5505

        lfo1.init(0.6, 12);
        lfo2.init(0.4, 12);
    };

    T next(T in_sample)
    {
        float l1val = lfo1.next();
        float l2val = lfo2.next();

        mapf1.setDelayLength(998 + l1val);
        mapf2.setDelayLength(739 + l2val);

        in_sample = apf4.next(apf3.next(apf2.next(apf1.next(lpf1.next(pre_delay.next(in_sample))))));

        T one_input = in_sample + g5_ * two_output;
        T two_input = in_sample + g5_ * one_output;

        one_output = delay2.next(apf5.next(lpf2.next(delay1.next(mapf1.next(one_input)))));
        two_output = delay4.next(apf6.next(lpf3.next(delay3.next(mapf2.next(two_input)))));

        l_sample = delay1.at(293) + delay1.at(3270) - apf5.at(2103) + delay2.at(2195) - delay3.at(2188) - apf6.at(206) - delay4.at(792);
        r_sample = delay3.at(388) + delay3.at(3989) - apf6.at(1350) + delay4.at(2939) - delay1.at(2321) - apf5.at(369) - delay2.at(133);
        return l_sample;
    };

    T l_sample;
    T r_sample;

    T one_output;
    T two_output;

    float g5_;
    float damping_;
    float bandwidth_;

    mDelayLine<int32_t> pre_delay;
    mOnePoleLPF<int32_t> lpf1;
    mSchroederAllpass<int32_t> apf1;
    mSchroederAllpass<int32_t> apf2;
    mSchroederAllpass<int32_t> apf3;
    mSchroederAllpass<int32_t> apf4;

    mSchroederAllpassLinear<int32_t> mapf1;
    mDelayLine<int32_t> delay1;
    mOnePoleLPF<int32_t> lpf2;
    mSchroederAllpass<int32_t> apf5;
    mDelayLine<int32_t> delay2;

    mSchroederAllpassLinear<int32_t> mapf2;
    mDelayLine<int32_t> delay3;
    mOnePoleLPF<int32_t> lpf3;
    mSchroederAllpass<int32_t> apf6;
    mDelayLine<int32_t> delay4;

    mSineLFO<AUDIO_RATE> lfo1;
    mSineLFO<AUDIO_RATE> lfo2;
};

#endif // MDATTORRO_PLATE_H_