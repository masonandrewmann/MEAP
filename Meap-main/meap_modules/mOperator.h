#ifndef MOPERATOR_H_
#define MOPERATOR_H_

template <uint32_t NUM_CELLS, class T = int8_t>
class mOperator
{
public:
    // mOperator(const T *TABLE_NAME)
    // {
    //     init(TABLE_NAME);
    // };

    mOperator() {
    };

    void init(const T *TABLE_NAME)
    {
        osc_.setTable(TABLE_NAME);
        osc_.setFreq(220);
        sustain_time_ = 4294967295;
        env_.setTimes(0, 0, sustain_time_, 0);
        sustain_level_ = 255;
        env_.setADLevels(255, sustain_level_);
        env_val_ = 0;
        osc_freq_ratio_ = 1.0;
        gain_ = 255;

        if (sizeof(T) == sizeof(int16_t))
        {
            shift_val_ = 16; // 8 bits down for osc birate
        }
        else
        {
            shift_val_ = 8;
        }

        looping_on_ = false;
        phase_sync_ = true;
        drone_ = false;
    }

    void noteOn(uint8_t note, uint8_t velocity)
    {
        osc_.setFreq(mtof(note) * osc_freq_ratio_);
        if (phase_sync_)
        {
            osc_.setPhase(0);
        }
        env_.setADLevels(velocity << 1, (velocity * sustain_level_ >> 7));
        env_.noteOn();
        note_active_ = true;
    }

    void noteOff()
    {
        env_.noteOff();
        note_active_ = false;
    }

    void setTable(const T *TABLE_NAME)
    {
        osc_.setTable(TABLE_NAME);
    }

    void setFreq(float f_)
    {
        osc_.setFreq(f_ * osc_freq_ratio_);
    }

    void setFreqRatio(float r_)
    {
        osc_freq_ratio_ = r_;
    }

    float getFreqRatio()
    {
        return osc_freq_ratio_;
    }

    void setLoopingOn()
    {
        looping_on_ = true;
        env_.setSustainTime(0);
    }

    void setLoopingOff()
    {
        looping_on_ = false;
        env_.setSustainTime(sustain_time_);
    }

    void setPhaseSyncOn()
    {
        phase_sync_ = true;
    }

    void setPhaseSyncOff()
    {
        phase_sync_ = false;
    }

    void setDroneOn()
    {
        drone_ = true;
    }

    void setDroneOff()
    {
        drone_ = false;
    }

    // 0-255
    void setGain(uint16_t g_)
    {
        gain_ = g_;
    }

    void setAttackTime(uint32_t a_)
    {
        env_.setAttackTime(a_);
    }

    void setDecayTime(uint32_t d_)
    {
        env_.setDecayTime(d_);
    }

    void setSustainTime(uint32_t s_)
    {
        sustain_time_ = s_;
        env_.setSustainTime(s_);
    }

    void setSustainLevel(uint16_t s_l)
    {
        sustain_level_ = s_l;
    }

    void setReleaseTime(uint32_t r_)
    {
        env_.setReleaseTime(r_);
    }

    void setTimes(uint32_t a_, uint32_t d_, uint32_t s_, uint32_t r_)
    {
        env_.setTimes(a_, d_, s_, r_);
    }

    void setADLevels(uint32_t a_l_, uint32_t d_l_)
    {
        sustain_level_ = d_l_;
        env_.setADLevels(a_l_, d_l_);
    }

    void update()
    {
        env_.update();
        if (looping_on_ && note_active_ && (env_.playing() == false))
        {
            env_.noteOn();
        }
    }

    int32_t next()
    {
        // env_.update();
        if (drone_)
        {
            env_val_ = 255;
        }
        else
        {
            env_val_ = env_.next();
        }
        last_out_ = osc_.next() * env_val_ * gain_ >> shift_val_;
        return last_out_; // returns a 16 bit sample
    }

    // mod input is 8bit
    int32_t next(int16_t mod_input)
    {
        // env_.update();
        if (drone_)
        {
            env_val_ = 255;
        }
        else
        {
            env_val_ = env_.next();
        }
        UFix<16, 16> deviation = 1.0;
        auto modulation = deviation * toSFraction(mod_input);
        last_out_ = osc_.phMod(modulation);
        return last_out_ * env_val_ * gain_ >> shift_val_; // returns a 16 bit sample
    }

protected:
    Oscil<NUM_CELLS, AUDIO_RATE> osc_;
    float osc_freq_ratio_;

    ADSR<CONTROL_RATE, AUDIO_RATE> env_;
    uint16_t env_val_;
    uint16_t shift_val_;
    uint32_t sustain_time_;
    uint16_t sustain_level_;
    uint16_t gain_;
    uint8_t feedback_amount_;
    int32_t last_out_;

    bool note_active_;
    bool phase_sync_;
    bool looping_on_;
    bool drone_;

    UFix<0, 16> mod_index_;
    UFix<16, 16> deviation_;
};

#endif // MOPERATOR_H_
