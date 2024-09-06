#ifndef MSAMPLEGRAINGENERATOR_H_
#define MSAMPLEGRAINGENERATOR_H_

template <uint32_t mSAMPLE_LENGTH, uint32_t mAUDIO_RATE = AUDIO_RATE, class T = int8_t>
class mSampleGrainGenerator
{
public:
    mSampleGrainGenerator(const T *TABLE_NAME)
    {
        init(TABLE_NAME);
    };

    mSampleGrainGenerator() {
        // init(SIN8192_DATA);
    };

    void init(const T *TABLE_NAME)
    {

        if (sizeof(T) == sizeof(int16_t))
        {
            shift_val_ = 16; // 8 bits down for sample birate + 7 bits for velocity
        }
        else
        {
            shift_val_ = 8; // just 7 bits down for velocity
        }

        grain_amplitude_ = 255;
        grain_duration_ = 40;
        grain_frequency_ = (float)mAUDIO_RATE / (float)mSAMPLE_LENGTH;
        default_frequency_ = grain_frequency_;
        grain_pan_ = 127;
        l_pan_val_ = m_sin1024_uint_DATA[255 - grain_pan_];
        r_pan_val_ = m_sin1024_uint_DATA[grain_pan_];
        grain_pos_ = 0;

        sample_.setFreq(grain_frequency_);
        sample_.setEnd(mSAMPLE_LENGTH);
        sample_.setTable(TABLE_NAME);

        env_.setADLevels(255, 0);
        env_.setTimes(grain_duration_ / 2, grain_duration_ / 2, 1, 1);
    }

    void setTableAndEnd(const T *TABLE_NAME, uint64_t END)
    {
        sample_.setTable(TABLE_NAME);
        sample_.setEnd(END);
    }

    void setDuration(int16_t dur)
    {
        env_.setTimes(dur / 2, dur / 2, 0, 0);
    }

    // 0-255
    void setAmplitude(int16_t amp)
    {
        grain_amplitude_ = amp;
    }

    void setFrequency(float freq)
    {
        grain_frequency_ = freq;
    }

    // returns true if envelope is finished
    bool playing()
    {
        return env_.playing();
    }

    // 0-255 (fully L to fully R)
    void setPan(int16_t pan)
    {
        grain_pan_ = pan;
        l_pan_val_ = m_sin1024_uint_DATA[255 - grain_pan_];
        r_pan_val_ = m_sin1024_uint_DATA[grain_pan_];
    }

    void noteOn(float freq, uint32_t grain_pos)
    {
        grain_pos_ = grain_pos;
        grain_frequency_ = freq;
        sample_.setFreq(grain_frequency_);
        sample_.start(grain_pos_);
        env_.noteOn();
    }

    void noteOn()
    {
        sample_.setFreq(grain_frequency_);
        sample_.start(grain_pos_);
        env_.noteOn();
    }

    // to be called in audio loop
    int32_t next()
    {
        env_.update();
        int64_t output_sample = sample_.next() * grain_amplitude_ * env_.next() >> shift_val_; // 16 bits

        l_sample = (l_pan_val_ * output_sample) >> 8; // down to 16 bits
        r_sample = (r_pan_val_ * output_sample) >> 8;

        return l_sample;
    }

    int32_t l_sample;
    int32_t r_sample;

protected:
    ADSR<mAUDIO_RATE, mAUDIO_RATE> env_;
    mSample<mSAMPLE_LENGTH, mAUDIO_RATE, T> sample_;

    int16_t grain_amplitude_;
    int32_t grain_duration_;
    float grain_frequency_;
    int16_t grain_pan_;
    uint32_t grain_pos_;

    float default_frequency_;

    int16_t shift_val_;

    int16_t l_pan_val_;
    int16_t r_pan_val_;
};

#endif // MSAMPLEGRAINGENERATOR_H_