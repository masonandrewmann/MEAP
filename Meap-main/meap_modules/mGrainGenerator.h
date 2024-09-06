#ifndef MGRAINGENERATOR_H_
#define MGRAINGENERATOR_H_

#include <tables/sin8192_int8.h> // table for Oscils to play
#include <tables/m_sin1024_uint8.h>

template <uint32_t mNUM_CELLS = SIN8192_NUM_CELLS, uint32_t mAUDIO_RATE = AUDIO_RATE, class T = int8_t>
class mGrainGenerator
{
public:
    mGrainGenerator(const T *TABLE_NAME)
    {
        init(TABLE_NAME);
    };

    mGrainGenerator()
    {
        init(SIN8192_DATA);
    };

    void init(const T *TABLE_NAME)
    {

        grain_amplitude_ = 255;
        grain_duration_ = 40;
        grain_frequency_ = 220;
        grain_pan_ = 127;

        osc_.setTable(TABLE_NAME);
        osc_.setFreq(grain_frequency_);

        env_.setADLevels(255, 0);
        env_.setTimes(grain_duration_ / 2, grain_duration_ / 2, 1, 1);
    }

    void setTable(const T *TABLE_NAME)
    {
        osc_.setTable(TABLE_NAME);
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

    // 0-255 (fully L to fully R)
    void setPan(int16_t pan)
    {
        grain_pan_ = pan;
    }

    void noteOn(float freq)
    {
        grain_frequency_ = freq;
        osc_.setFreq(grain_frequency_);
        env_.noteOn();
    }

    void noteOn()
    {
        osc_.setFreq(grain_frequency_);
        env_.noteOn();
    }

    // to be called in audio loop
    int32_t next()
    {
        env_.update();
        int64_t output_sample = osc_.next() * grain_amplitude_ * env_.next() >> 8; // 24 bits
        // int64_t output_sample = osc_.next() * env_.next(); // 24 bits

        // l_sample_ = (m_sin1024_uint_DATA[255 - grain_pan_] * output_sample) >> 16; // down to 16 bits
        // r_sample_ = (m_sin1024_uint_DATA[grain_pan_] * output_sample) >> 16;

        return output_sample;
    }

protected:
    ADSR<mAUDIO_RATE, mAUDIO_RATE> env_;
    Oscil<mNUM_CELLS, mAUDIO_RATE> osc_;

    int32_t l_sample_;
    int32_t r_sample_;

    int16_t grain_amplitude_;
    int32_t grain_duration_;
    float grain_frequency_;
    int16_t grain_pan_;
};

#endif // MGRAINGENERATOR_H_