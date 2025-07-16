#ifndef MEAP_SAMPLER_H_
#define MEAP_SAMPLER_H_

template <uint32_t mSAMPLE_LENGTH, class T = int16_t, uint8_t INTERP = mINTERP_NONE>
class mSampler
{
public:
    //! Class constructor.

    mSampler()
    {
        init(NULL);
    }

    mSampler(const T *TABLE_NAME)
    {
        init(TABLE_NAME);
    };

    void init(const T *TABLE_NAME)
    {
        // initialize adsr here
        adsr_.setTimes(1, 1, 4294967295, 1);
        adsr_.setADLevels(255, 255);
        freq_ = 1.0;
        sample_.setFreq(freq_);
        // initialize sample here
        sample_.setTable(TABLE_NAME);
        sample_.setEnd(mSAMPLE_LENGTH);
        velocity_ = 127;

        if (sizeof(T) == sizeof(int16_t))
        {
            shift_val_ = 15; // 8 bits down for sample birate + 7 bits for velocity
        }
        else
        {
            shift_val_ = 7; // just 7 bits down for velocity
        }
        default_freq_ = (float)AUDIO_RATE / (float)mSAMPLE_LENGTH;
    }

    void setTable(const T *TABLE_NAME, uint64_t TABLE_SIZE)
    {
        sample_.setTable(TABLE_NAME);
        sample_.setEnd(TABLE_SIZE);
    }

    void setTableAndEnd(const T *TABLE_NAME, uint64_t TABLE_SIZE)
    {
        sample_.setTable(TABLE_NAME);
        sample_.setEnd(TABLE_SIZE);
    }

    //! Start a note with the given frequency and amplitude.
    void noteOn(float frequency, uint8_t v_)
    {
        velocity_ = v_;
        sample_.setFreq(frequency);
        // sample_.setFreq(sample_frequencies[note_number]);
        sample_.start();
        adsr_.noteOn();
    }

    void setReverseOn()
    {
        sample_.setReverseOn();
    }

    void setReverseOff()
    {
        sample_.setReverseOff();
    }

    //! Stop a note with the given amplitude (speed of decay).
    void noteOff()
    {
        adsr_.noteOff();
    }

    //! Set instrument parameters for a particular frequency.
    void setFrequency(float frequency)
    {
        sample_.setFreq(frequency);
    }

    void setLoopingOn()
    {
        sample_.setLoopingOn();
    }

    void setLoopingOff()
    {
        sample_.setLoopingOff();
    }

    void setAttackTime(uint32_t a_)
    {
        adsr_.setAttackTime(a_);
    }

    void setDecayTime(uint32_t decay)
    {
        adsr_.setDecayTime(decay);
    }

    void setReleaseTime(uint32_t r)
    {
        adsr_.setReleaseTime(r);
    }

    void setTimes(uint32_t a_, uint32_t d_, uint32_t s_, uint32_t r_)
    {
        adsr_.setTimes(a_, d_, s_, r_);
    }

    void setADLevels(uint32_t a_l_, uint32_t d_l_)
    {
        adsr_.setADLevels(a_l_, d_l_);
    }

    void setStart(uint64_t start)
    {
        sample_.setStart(start);
    }

    void setEnd(uint64_t end)
    {
        sample_.setEnd(end);
    }

    void update()
    {
        adsr_.update();
    }

    int32_t next()
    {

        return (sample_.next() * adsr_.next() * velocity_) >> shift_val_; // 8 bit gain * 8 bit sample = 16bit result * velocity >>7 = ~16bit result
    }

    // CLASS VARIABLES
    uint16_t velocity_;
    float freq_;
    mSample<mSAMPLE_LENGTH, AUDIO_RATE, T, INTERP> sample_;

    uint8_t shift_val_;
    float default_freq_;

    ADSR<CONTROL_RATE, AUDIO_RATE> adsr_;
};

#endif // MEAP_SAMPLER_H_
