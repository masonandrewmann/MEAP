#ifndef MENV_SECTION_H_
#define MENV_SECTION_H_

#include "fast_pow2.h"

enum MEAP_ENV_CURVES
{
    mCURVE_LINEAR,
    mCURVE_EXPONENTIAL,
};

template <uint32_t UPDATE_RATE = CONTROL_RATE>
class mEnvSection
{
public:
    mEnvSection(uint32_t length_ms, float starting_level, float ending_level, MEAP_ENV_CURVES curve_type = mCURVE_LINEAR, float exp = -4.3)
    {
        ms_to_samples = 0.001 * UPDATE_RATE;
        setExpRate(exp);
        setLength(length_ms);
        setLevels(starting_level, ending_level);
        sample_count = length_samples; // start at ending point of envelope before triggering it,, prob should rework this!
        curve = curve_type;
    };

    void trigger()
    {
        sample_count = 0;
    }

    void setLength(uint32_t length_ms)
    {
        length_samples = length_ms * ms_to_samples;
        t1_inv = 1.f / (float)length_samples;
    }

    // sets curve for exponential segments
    // 0 is linear, positive pulls curve to right, negative pulls curve to left
    void setExpRate(float rate)
    {
        if (rate == 0)
        {
            rate = 0.00000000001; // to prevent divide by zero
        }
        exp_rate = rate;
        exp_overshoot = 1.0 / (1.0 - exp2_lut(exp_rate));
    }

    void setStartLevel(float start_level)
    {
        v0 = start_level;
        level_diff = v1 - v0;
    }

    void setEndLevel(float end_level)
    {
        v1 = end_level;
        level_diff = v1 - v0;
    }

    void setLevels(float start_level, float end_level)
    {
        v0 = start_level;
        v1 = end_level;
        level_diff = v1 - v0;
    }

    // return if envelope section is done
    bool done()
    {
        return (sample_count > length_samples);
    }

    float next()
    {
        switch (curve)
        {
        case mCURVE_EXPONENTIAL:
            if (sample_count <= length_samples)
            {
                sample_count++;
                // return v0 + level_diff * (1.f - pow(2, exp_rate * (float)sample_count * t1_inv));
                return v0 + level_diff * (1.f - exp2_lut(exp_rate * (float)sample_count * t1_inv)) * exp_overshoot;
            }
            else
            {
                return v1;
            }
            break;
        case mCURVE_LINEAR:
            if (sample_count <= length_samples)
            {
                sample_count++;
                return v0 + level_diff * (float)sample_count * t1_inv;
            }
            else
            {
                return v1;
            }
            break;
        }
    }

    float ms_to_samples;
    int32_t length_samples;
    float t1_inv;
    float v0;
    float v1;
    float level_diff;
    float exp_rate;
    float exp_overshoot;
    int32_t sample_count;
    MEAP_ENV_CURVES curve;
};

#endif // MENV_SECTION_H_