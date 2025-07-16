#ifndef MEAP_PLUCKED_H
#define MEAP_PLUCKED_H

#include "meap_delay_allpass.h"
#include "meap_one_zero.h"
#include "meap_one_pole.h"
#include "meap_noise.h"

/***************************************************/
/*! \class stk::Plucked
    \brief STK basic plucked string class.

    This class implements a simple plucked string
    physical model based on the Karplus-Strong
    algorithm.

    For a more advanced plucked string implementation,
    see the stk::Twang class.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.
    There exist at least two patents, assigned to
    Stanford, bearing the names of Karplus and/or
    Strong.

    by Perry R. Cook and Gary P. Scavone, 1995--2019.
*/
/***************************************************/

class MEAP_Plucked
{
public:
    //! Class constructor, taking the lowest desired playing frequency (greater than zero)
    MEAP_Plucked(float lowestFrequency = 10.0)
    {
        uint32_t delays = (uint32_t)(AUDIO_RATE / lowestFrequency);
        delayLine_.setMaximumDelay(delays + 1);

        this->setFrequency(220.0f);
    };

    //! Class destructor.
    ~MEAP_Plucked(void){};

    //! Reset and clear all internal state.
    void clear(void)
    {
        delayLine_.clear();
        loopFilter_.clear();
        pickFilter_.clear();
    };

    //! Set instrument parameters for a particular frequency (greater than zero)
    void setFrequency(float frequency)
    {
        // Delay = length - filter delay.
        float delay = (AUDIO_RATE / frequency) - loopFilter_.phaseDelay(frequency);
        delayLine_.setDelay(delay);

        loopGain_ = 0.995f + (frequency * 0.000005f);
        if (loopGain_ >= 1.0f)
            loopGain_ = 0.99999f;
    };

    //! Pluck the string with the given amplitude using the current frequency (amp between 0.0 and 1.0)
    void pluck(float amplitude)
    {
        pickFilter_.setPole(0.999f - (amplitude * 0.15f));
        pickFilter_.setGain(amplitude * 0.5f);
        for (uint32_t i = 0; i < delayLine_.getDelay(); i++)
            // Fill delay with noise additively with current contents.
            delayLine_.next(0.6f * delayLine_.lastOut() + pickFilter_.next(noise_.next()));
    };

    //! Start a note with the given frequency and amplitude.
    void noteOn(float frequency, float amplitude)
    {
        this->setFrequency(frequency);
        this->pluck(amplitude);
    };

    //! Stop a note with the given amplitude (speed of decay, between 0 and 1).
    void noteOff(float amplitude)
    {
        loopGain_ = 1.0f - amplitude;
    };

    //! Compute and return one output sample.
    float next()
    {
        // Here's the whole inner loop of the instrument!!
        return last_output_ = 3.0f * delayLine_.next(loopFilter_.next(delayLine_.lastOut() * loopGain_));
    };

protected:
    MEAP_Delay_Allpass<int16_t> delayLine_;
    MEAP_OneZero<int16_t> loopFilter_;
    MEAP_OnePole<int16_t> pickFilter_;
    MEAP_Noise noise_;

    float loopGain_;
    int32_t last_output_;
};

#endif // MEAP_PLUCKED_H
