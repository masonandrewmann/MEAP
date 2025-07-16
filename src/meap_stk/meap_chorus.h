#ifndef MEAP_CHORUS_H
#define MEAP_CHORUS_H

#include "meap_effect.h"
#include "meap_delay_linear.h"
#include "meap_sinewave.h"

/***************************************************/
/*! \class stk::Chorus
    \brief STK chorus effect class.

    This class implements a chorus effect.  It takes a monophonic
    input signal and produces a stereo output signal.

    by Perry R. Cook and Gary P. Scavone, 1995--2019.
*/
/***************************************************/

template <class T = int16_t>
class MEAP_Chorus : public MEAP_Effect
{
public:
    //! Class constructor, taking the median desired delay length.
    /*!
      An StkError can be thrown if the rawwave path is incorrect.
    */
    MEAP_Chorus(float baseDelay = 300)
    {
        last_output_ = (T *)calloc(2, sizeof(T));

        delayLine_[0].setMaximumDelay((uint32_t)(baseDelay * 1.414f) + 2);
        delayLine_[0].setDelay(baseDelay);
        delayLine_[1].setMaximumDelay((uint32_t)(baseDelay * 1.414f) + 2);
        delayLine_[1].setDelay(baseDelay);
        // baseLength_ = baseDelay;
        baseLength_[0] = baseDelay * 0.707f;
        baseLength_[1] = baseDelay * 0.5f;

        mods_[0].setFrequency(0.2);
        mods_[1].setFrequency(0.222222);
        modDepth_ = 0.05;

        effectMix_ = 0.5;
        this->clear();
    }

    ~MEAP_Chorus()
    {
        free(last_output_);
    }

    //! Reset and clear all internal state.
    void clear(void)
    {
        delayLine_[0].clear();
        delayLine_[1].clear();
        last_output_[0] = 0;
        last_output_[1] = 0;
    };

    //! Set modulation depth in range 0.0 - 1.0.
    void setModDepth(float depth)
    {
        modDepth_ = depth;
    };

    //! Set modulation frequency.
    void setModFrequency(float frequency)
    {
        mods_[0].setFrequency(frequency);
        mods_[1].setFrequency(frequency * 1.1111f);
    }

    //! Return the specified channel value of the last computed stereo frame.
    /*!
      Use the lastFrame() function to get both values of the last
      computed stereo frame.  The \c channel argument must be 0 or 1
      (the first channel is specified by 0).  However, range checking is
      only performed if _STK_DEBUG_ is defined during compilation, in
      which case an out-of-range value will trigger an StkError
      exception.
    */
    T lastOut(uint16_t channel = 0)
    {
        return last_output_[channel];
    };

    //! Input one sample to the effect and return the specified \c channel value of the computed stereo frame.
    /*!
      Use the lastFrame() function to get both values of the computed
      stereo output frame. The \c channel argument must be 0 or 1 (the
      first channel is specified by 0).  However, range checking is only
      performed if _STK_DEBUG_ is defined during compilation, in which
      case an out-of-range value will trigger an StkError exception.
    */
    T next(T input, uint16_t channel = 0)
    {
        delayLine_[0].setDelay(baseLength_[0] * (1.0f + modDepth_ * mods_[0].next()));
        delayLine_[1].setDelay(baseLength_[1] * (1.0f - modDepth_ * mods_[1].next()));
        // last_output_[0] = effectMix_ * (delayLine_[0].next(input) - input) + input;
        // last_output_[1] = effectMix_ * (delayLine_[1].next(input) - input) + input;
        last_output_[0] = delayLine_[0].next(input); // effect mix hardcoded to fully wet for now
        last_output_[1] = delayLine_[1].next(input);
        return last_output_[channel];
    };

protected:
    MEAP_Delay_Linear<T> delayLine_[2];
    MEAP_SineWave mods_[2];

    float baseLength_[2];
    float modDepth_;

    T *last_output_;
};

#endif // MEAP_CHORUS_H
