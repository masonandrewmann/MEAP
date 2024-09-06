#ifndef MFM2VOICE_H_
#define MFM2VOICE_H_

#include "tables/m_sin1024_uint8.h" // table for crossfading between filter types
#include <tables/sin8192_int8.h>    // table for Oscils to play

// ALGORITHMS
// 0: modulator fm modulates carrier
// 1: mod and carrier both to output, no modulation!

template <uint32_t mNUM_CELLS = SIN8192_NUM_CELLS, bool FILTER_ACTIVE = false, class T = int8_t>
class mFM2Voice
{
public:
    mFM2Voice()
    {
        init(SIN8192_DATA);
    };

    mFM2Voice(const T *TABLE_NAME)
    {
        init(TABLE_NAME);
    };

    void init(const T *TABLE_NAME)
    {
        carrier.init(TABLE_NAME);
        modulator.init(TABLE_NAME);
        carrier.setLoopingOff();
        modulator.setLoopingOff();
        last_output[0] = 0;
        last_output[1] = 0;
        algorithm = 0;

        if (FILTER_ACTIVE)
        {
            filter_env_amount_ = 0;
            filter_sustain_level_ = 255;
            filter_env_.setADLevels(filter_env_amount_, (filter_env_amount_ * filter_sustain_level_) >> 8);
            filter_env_.setTimes(1, 1, 4294967295, 1);
            filter_.setCutoffFreq(255);
            filter_.setResonance(0);
            setFilterType(0);
        }
    }

    void setAlgorithm(int alg)
    {
        algorithm = alg;
    }

    void setModTable(const T *TABLE_NAME)
    {
        modulator.setTable(TABLE_NAME);
    }

    void setCarrierTable(const T *TABLE_NAME)
    {
        carrier.setTable(TABLE_NAME);
    }

    void setModADLevels(uint32_t a_l, uint32_t d_l)
    {
        modulator.setADLevels(a_l, d_l);
    }

    void setCarrierADLevels(uint32_t a_l, uint32_t d_l)
    {
        carrier.setADLevels(a_l, d_l);
    }

    void setModTimes(uint32_t a_t, uint32_t d_t, uint32_t s_t, uint32_t r_t)
    {
        modulator.setTimes(a_t, d_t, s_t, r_t);
    }

    void setCarrierTimes(uint32_t a_t, uint32_t d_t, uint32_t s_t, uint32_t r_t)
    {
        carrier.setTimes(a_t, d_t, s_t, r_t);
    }

    void setRatio(float mod_ratio)
    {
        modulator.setFreqRatio(mod_ratio);
    }

    // 0-255
    void setFMAmount(int gain)
    {
        modulator.setGain(gain);
    }

    // 0 - 255
    void setCutoff(uint16_t cutoff_val)
    {
        cutoff_ = cutoff_val;
    }

    // 0 - 255
    void setResonance(uint16_t resonance_val)
    {
        resonance_ = resonance_val;
    }

    // 0-255
    void setFilterEnvAmount(uint16_t f_amt)
    {
        filter_env_amount_ = f_amt;
        filter_env_.setADLevels(filter_env_amount_, (filter_env_amount_ * filter_sustain_level_) >> 8);
    }

    void setFilterEnvTimes(uint32_t a_, uint32_t d_, uint32_t s_, uint32_t r_)
    {
        filter_env_.setTimes(a_, d_, s_, r_);
    }

    void setFilterAttackTime(uint32_t f_a)
    {
        filter_env_.setAttackTime(f_a);
    }

    void setFilterDecayTime(uint32_t time)
    {
        filter_env_.setDecayTime(time);
    }

    void setFilterReleaseTime(uint32_t f_a)
    {
        filter_env_.setReleaseTime(f_a);
    }

    // 0-255
    void setFilterSustainLevel(uint16_t level)
    {
        filter_sustain_level_ = level;
        filter_env_.setADLevels(filter_env_amount_, (filter_env_amount_ * filter_sustain_level_) >> 8);
    }

    // 0-255, fades between lowpass at 0, highpass at 127, and bandpass at 255
    void setFilterType(uint16_t f_t)
    {
        filter_type_ = f_t;
        if (filter_type_ < 128)
        {
            filter_low_amt_ = m_sin1024_uint_DATA[(127 - filter_type_) << 1];
            filter_high_amt_ = m_sin1024_uint_DATA[filter_type_ << 1];
            filter_band_amt_ = 0;
        }
        else
        {
            filter_high_amt_ = m_sin1024_uint_DATA[(255 - filter_type_) << 1];
            filter_band_amt_ = m_sin1024_uint_DATA[(filter_type_ - 127) << 1];
            filter_low_amt_ = 0;
        }
    }

    void update()
    {
        modulator.update();
        carrier.update();

        if (FILTER_ACTIVE)
        {
            filter_env_.update();
            int32_t cutoff_val = filter_env_.next() + cutoff_;
            if (cutoff_val > 255)
            {
                cutoff_val = 255;
            }
            else if (cutoff_val < 0)
            {
                cutoff_val = 0;
            }
            filter_.setCutoffFreqAndResonance(cutoff_val, resonance_);
        }
    }

    void noteOn(uint8_t note, uint8_t vel)
    {
        modulator.noteOn(note, vel);
        carrier.noteOn(note, vel);
        if (FILTER_ACTIVE)
        {
            filter_env_.noteOn();
        }
    }

    void noteOff()
    {
        carrier.noteOff();
        modulator.noteOff();
        if (FILTER_ACTIVE)
        {
            filter_env_.noteOff();
        }
    }

    int32_t next()
    {
        int32_t output_sample = 0;
        if (algorithm)
        {
            output_sample = carrier.next() + modulator.next();
        }
        else
        {
            output_sample = carrier.next(modulator.next());
        }

        if (FILTER_ACTIVE)
        {
            filter_.next(output_sample);
            output_sample = ((filter_.low() * filter_low_amt_) >> 8) + ((filter_.high() * filter_high_amt_) >> 8) + ((filter_.band() * filter_band_amt_) >> 8);
        }

        return output_sample;
    }

    int32_t last_output[2];
    mOperator<mNUM_CELLS, T> carrier;
    mOperator<mNUM_CELLS, T> modulator;

    MultiResonantFilter<uint8_t> filter_;
    ADSR<CONTROL_RATE, CONTROL_RATE> filter_env_;
    uint16_t cutoff_;
    uint16_t resonance_;
    uint16_t filter_env_amount_;
    uint16_t filter_sustain_level_;
    uint16_t filter_type_;
    uint16_t filter_low_amt_;
    uint16_t filter_high_amt_;
    uint16_t filter_band_amt_;

    int algorithm;

protected:
};

#endif // MFM2VOICE_H_