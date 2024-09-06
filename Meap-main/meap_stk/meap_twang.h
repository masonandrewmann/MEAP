#ifndef MEAP_TWANG_H
#define MEAP_TWANG_H

#include "meap_delay_allpass.h"
#include "meap_delay_linear.h"
#include "meap_fir.h"

/***************************************************/
/*! \class stk::Twang
    \brief STK enhanced plucked string class.

    This class implements an enhanced plucked-string
    physical model, a la Jaffe-Smith, Smith,
    Karjalainen and others.  It includes a comb
    filter to simulate pluck position.  The tick()
    function takes an input sample, which is added
    to the delayline input.  This can be used to
    implement commuted synthesis (if the input
    samples are derived from the impulse response of
    a body filter) and/or feedback (as in an electric
    guitar model).

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    by Perry R. Cook and Gary P. Scavone, 1995--2019.
*/
/***************************************************/
template <class T = int16_t>
class MEAP_Twang
{
public:
    //! Class constructor, taking the lowest desired playing frequency. Must be greater than zero!
    MEAP_Twang(float lowestFrequency = 50.0)
    {
        this->setLowestFrequency(lowestFrequency);
        float my_coefficients[2] = {0.5, 0.5};
        loopFilter_.setCoefficients(my_coefficients, 2);

        loopGain_ = 0.995;
        pluckPosition_ = 0.4;
        this->setFrequency(220.0);
    }
    //! Reset and clear all internal state.
    void clear(void)
    {
        delayLine_.clear();
        combDelay_.clear();
        loopFilter_.clear();
        lastOutput_ = 0.0;
    };

    //! Set the delayline parameters to allow frequencies as low as specified.
    void setLowestFrequency(float frequency)
    {
        uint32_t nDelays = (uint32_t)(AUDIO_RATE / frequency);
        delayLine_.setMaximumDelay(nDelays + 1);
        combDelay_.setMaximumDelay(nDelays + 1);
    };

    //! Set the delayline parameters for a particular frequency.
    void setFrequency(float frequency)
    {
        frequency_ = frequency;
        // Delay = length - filter delay.
        float delay = (AUDIO_RATE / frequency) - loopFilter_.phaseDelay(frequency);
        delayLine_.setDelay(delay);

        this->setLoopGain(loopGain_);

        // Set the pluck position, which puts zeroes at position * length.
        combDelay_.setDelay(0.5f * pluckPosition_ * delay);
    };

    //! Set the pluck or "excitation" position along the string (0.0 - 1.0).
    void setPluckPosition(float position)
    {
        pluckPosition_ = position;
    };

    //! Set the nominal loop gain. (0.0-1.0)
    /*!
      The actual loop gain is based on the value set with this
      function, but scaled slightly according to the frequency.  Higher
      frequency settings have greater loop gains because of
      high-frequency loop-filter roll-off.
    */
    void setLoopGain(float loopGain)
    {
        loopGain_ = loopGain;
        float gain = loopGain_ + (frequency_ * 0.000005f);
        if (gain >= 1.0f)
            gain = 0.99999;
        loopFilter_.setGain(gain);
    };

    //! Set the loop filter coefficients.
    /*!
      The loop filter can be any arbitrary FIR filter.  By default,
      the coefficients are set for a first-order lowpass filter with
      coefficients b = [0.5 0.5].
    */
    // void setLoopFilter(std::vector<float> coefficients);

    //! Return an StkFrames reference to the last output sample frame.
    // const StkFrames& lastFrame( void ) const { return lastFrame_; };

    //! Return the last computed output value.
    // float lastOut( void ) { return lastFrame_[0]; };
    T lastOut(void) { return lastOutput_; };

    //! Compute and return one output sample.
    T next(T input)
    {
        lastOutput_ = delayLine_.next(input + loopFilter_.next(delayLine_.lastOut()));
        lastOutput_ -= combDelay_.next(lastOutput_); // comb filtering on output
        lastOutput_ = lastOutput_ * 0.5;

        return lastOutput_;
    };

protected:
    // MEAP_Delay_Allpass<int16_t> delayLine_(2, 1); // dummy lengths
    // MEAP_Delay_Linear<int16_t> combDelay_(2, 1);  // dummy lengths
    MEAP_Delay_Allpass<int16_t> delayLine_; // dummy lengths
    MEAP_Delay_Linear<int16_t> combDelay_;  // dummy lengths
    MEAP_fir<int16_t> loopFilter_;

    T lastOutput_;
    float frequency_;
    float loopGain_;
    float pluckPosition_;
};

#endif // MEAP_TWANG_H
