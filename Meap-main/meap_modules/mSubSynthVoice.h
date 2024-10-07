#ifndef MSUBSYNTH_H_
#define MSUBSYNTH_H_

#include <tables/saw2048_int8.h> // table for Oscils to play

template <uint32_t mNUM_CELLS = SAW2048_NUM_CELLS, uint32_t mNUM_OSC = 1, class T = int8_t>
class mSubSynthVoice
{
public:
    mSubSynthVoice(const T *TABLE_NAME)
    {
        init(TABLE_NAME);
    };

    mSubSynthVoice()
    {
        init(SAW2048_DATA);
    };

    void init(const T *TABLE_NAME)
    {
        cutoff_ = 255;
        resonance_ = 0;

        for (uint16_t i = 0; i < mNUM_OSC; i++)
        {
            osc_[i].setTable(TABLE_NAME);
            osc_[i].setFreq(220);
            osc_gain_[i] = 255;
            osc_semitones_[i] = 0;
        }

        noise_gain_ = 0;

        amp_env_.setADLevels(255, 255);
        amp_env_.setTimes(1, 1, 4294967295, 1);
        filter_env_amount_ = 0;
        filter_sustain_level_ = 255;
        filter_env_.setADLevels(filter_env_amount_, (filter_env_amount_ * filter_sustain_level_) >> 8);
        filter_env_.setTimes(1, 1, 4294967295, 1);
        filter_.setCutoffFreq(255);
        filter_.setResonance(0);
        sustain_level_ = 255;
        filter_key_offset = 0;

        if (sizeof(T) == sizeof(int16_t))
        {
            shift_val_ = 16; // 8 for env, 8 for gain
            noise_upper_ = 65535;
            noise_sub_ = 32768;
        }
        else
        {
            shift_val_ = 8;
            noise_upper_ = 255;
            noise_sub_ = 128;
        }

        phase_sync_ = true;
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

    // 0-255
    void setOscGain(uint16_t osc_num, int16_t gain)
    {
        osc_gain_[osc_num] = gain;
    }

    void setNoiseGain(uint16_t noise_gain)
    {
        noise_gain_ = noise_gain;
    }

    void setOscTable(uint16_t osc_num, const T *TABLE_NAME)
    {
        osc_[osc_num].setTable(TABLE_NAME);
    }

    void setOscSemitones(uint16_t osc_num, int16_t semitone_offset)
    {
        osc_semitones_[osc_num] = semitone_offset;
    }

    void setOscDetune(uint16_t osc_num, float detune_amount)
    {
        osc_detune_[osc_num] = detune_amount;
    }

    void setPhaseSyncOn()
    {
        phase_sync_ = true;
    }

    void setPhaseSyncOff()
    {
        phase_sync_ = false;
    }

    // in ms
    void setAttackTime(uint32_t a_)
    {
        amp_env_.setAttackTime(a_);
    }

    // in ms
    void setDecayTime(uint32_t d_)
    {
        amp_env_.setDecayTime(d_);
    }

    // in ms
    void setSustainTime(uint32_t s_)
    {
        amp_env_.setSustainTime(s_);
    }

    // in ms
    void setReleaseTime(uint32_t r_)
    {
        amp_env_.setReleaseTime(r_);
    }

    // in ms
    void setTimes(uint32_t a_, uint32_t d_, uint32_t s_, uint32_t r_)
    {
        amp_env_.setTimes(a_, d_, s_, r_);
    }

    // 0-255
    void setSustainLevel(uint32_t s_l)
    {

        sustain_level_ = s_l;
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

    // to be called in control loop
    void update()
    {
        filter_env_.update();
        int32_t cutoff_val = filter_env_.next() + cutoff_ + filter_key_offset;
        if (cutoff_val > 255)
        {
            cutoff_val = 255;
        }
        else if (cutoff_val < 0)
        {
            cutoff_val = 0;
        }
        filter_.setCutoffFreqAndResonance(cutoff_val, resonance_);
        amp_env_.update();
    }

    void noteOn(uint16_t note, uint16_t vel)
    {
        for (uint16_t i = 0; i < mNUM_OSC; i++)
        {
            osc_[i].setFreq(mtof((float)note + (float)osc_semitones_[i] + osc_detune_[i]));
            if (phase_sync_)
            {
                osc_[i].setPhase(0);
            }
        }
        amp_env_.setADLevels(vel << 1, (vel * sustain_level_) >> 7);
        amp_env_.noteOn();
        filter_env_.noteOn();
        if (note > 60)
        {
            filter_key_offset = (note - 60) * 2;
        }
    }

    void noteOff()
    {
        amp_env_.noteOff();
        filter_env_.noteOff();
    }

    // to be called in audio loop
    int32_t next()
    {
        int32_t output_sample = 0;

        for (uint16_t i = 0; i < mNUM_OSC; i++)
        {

            output_sample += (osc_[i].next() * osc_gain_[i]);
        }

        output_sample += ((xorshift96() % noise_upper_) - noise_sub_) * noise_gain_; // noise sample with same bitrate as osc

        filter_.next(output_sample);
        output_sample = filter_.low();

        output_sample = (output_sample * amp_env_.next()) >> shift_val_; // down to 16 bits

        return output_sample;
    }

protected:
    Oscil<mNUM_CELLS, AUDIO_RATE> osc_[mNUM_OSC];
    ADSR<CONTROL_RATE, AUDIO_RATE> amp_env_;
    MultiResonantFilter<uint8_t> filter_;
    ADSR<CONTROL_RATE, CONTROL_RATE> filter_env_;

    int8_t filter_key_offset;
    uint16_t noise_gain_;
    uint16_t osc_gain_[mNUM_OSC];
    int16_t osc_semitones_[mNUM_OSC];
    float osc_detune_[mNUM_OSC];
    uint16_t cutoff_;
    uint16_t resonance_;
    uint16_t filter_env_amount_;
    uint16_t filter_sustain_level_;
    uint16_t shift_val_;
    uint16_t sustain_level_;
    int32_t noise_upper_;
    int32_t noise_sub_;

    bool phase_sync_;
};

#endif // MSUBSYNTH_H_