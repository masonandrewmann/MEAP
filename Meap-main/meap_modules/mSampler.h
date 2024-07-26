#ifndef MEAP_SAMPLER_H_
#define MEAP_SAMPLER_H_

// #include "mInstrument.h"
// #include "meap_sample.h"
// #include "ADSR.h"

template <uint32_t mMAX_SAMPLE_LENGTH, uint32_t mAUDIO_RATE, class T = int16_t>
class mSampler
{
public:
    //! Class constructor.
    mSampler(void)
    {
        // initialize adsr here
        adsr_.setTimes(1, 1, 4294967295, 1);
        adsr_.setADLevels(255, 255);
        freq_ = 1.0;
        sample_.setFreq(freq_);
        gain_ = 0;
        // initialize sample here
        // sample_.setTable();
        sample_.setEnd(mMAX_SAMPLE_LENGTH);

        if(sizeof(T) == sizeof(int16_t)){
            shift_val_ = 8;
        } else {
            shift_val_ = 0;
        }
    };

    void setTable(const T *TABLE_NAME, uint64_t TABLE_SIZE)
    {
        sample_.setTable(TABLE_NAME);
        sample_.setEnd(TABLE_SIZE);
    }

    //! Start a note with the given frequency and amplitude.
    void noteOn(float frequency, uint8_t amplitude)
    {
        sample_.setFreq(frequency);
        sample_.start();
        adsr_.noteOn();
    }

    //! Stop a note with the given amplitude (speed of decay).
    void noteOff(uint8_t amplitude)
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

    void setDecay(uint32_t decay){
        adsr_.setDecayTime(decay);
    }

    //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
    void controlChange(uint8_t number, uint8_t value) {};

    //! Compute one sample frame and return the specified \c channel value.
    /*!
      For monophonic instruments, the \c channel argument is ignored.
    */
    int32_t next()
    {
        adsr_.update();
        gain_ = adsr_.next();
        return sample_.next() * gain_ >> shift_val_; // 8 bit gain * 8 bit sample = 16bit result
    }

protected:
    uint16_t gain_;
    float freq_;
    mSampleAmbi<mMAX_SAMPLE_LENGTH, mAUDIO_RATE, T> sample_;

    uint8_t shift_val_;

    ADSR<mAUDIO_RATE, mAUDIO_RATE> adsr_;
};

#endif // MEAP_SAMPLER_H_
