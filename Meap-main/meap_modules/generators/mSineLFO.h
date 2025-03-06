// Second modified coupled form oscillator from dattorro effect design part 3
// may have slight deviations in amplitude between frequencies
// two multiplies

#ifndef MSINELFO_H_
#define MSINELFO_H_

#include <math.h>
#include <Meap.h>

// template for audio vs control rate
template <uint32_t UPDATE_RATE>
class mSineLFO
{
public:
    mSineLFO() {

    };

    mSineLFO(float freq, float peak_amplitude = 1.0)
    {
        init(freq, peak_amplitude);
    };

    void init(float freq, float peak_amplitude = 1.0)
    {
        normalized_freq_mul = 2 * 3.14159265358979 / ((float)UPDATE_RATE);
        freq = freq * normalized_freq_mul;
        e = 2 * sin(freq / 2);
        last_out = peak_amplitude;
        last_out_quad = 0;
    }

    void setFreq(float freq)
    {
        freq = freq * normalized_freq_mul;
        e = 2 * sin(freq / 2);
    }

    float quadratureOutput()
    {
        return last_out_quad;
    }

    float next()
    {
        last_out_quad = last_out_quad - e * last_out;
        last_out = e * last_out_quad + last_out;
        return last_out;
    }

protected:
    float e;
    float last_out;
    float last_out_quad;
    float normalized_freq_mul;
};
#endif // MSINELFO_H_