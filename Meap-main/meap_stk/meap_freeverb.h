// I NEED HELP! mostly start with those static const variables, maybe use initializer lists or macros or smth?

#ifndef MEAP_FREEVERB_H
#define MEAP_FREEVERB_H

#include "meap_effect.h"
#include "meap_delay.h"
#include "meap_one_pole.h"

/***********************************************************************/
/*! \class stk::FreeVerb
    \brief Jezar at Dreampoint's FreeVerb, implemented in STK.

    Freeverb is a free and open-source Schroeder reverberator
    originally implemented in C++. The parameters of the reverberation
    model are exceptionally well tuned. FreeVerb uses 8
    lowpass-feedback-comb-filters in parallel, followed by 4 Schroeder
    allpass filters in series.  The input signal can be either mono or
    stereo, and the output signal is stereo.  The delay lengths are
    optimized for a sample rate of 44100 Hz.

    Ported to STK by Gregory Burlet, 2012.
*/
/***********************************************************************/

template <class T = int16_t>
class MEAP_FreeVerb : public MEAP_Effect
{
public:
    //! FreeVerb Constructor
    /*!
      Initializes the effect with default parameters. Note that these defaults
      are slightly different than those in the original implementation of
      FreeVerb [Effect Mix: 0.75; Room Size: 0.75; Damping: 0.25; Width: 1.0;
      Mode: freeze mode off].
    */
    MEAP_FreeVerb()
    {
        // Resize lastFrame_ for stereo output
        last_output_ = (T *)calloc(2, sizeof(T));

        // Initialize parameters
        MEAP_Effect::setEffectMix(0.75);                 // set initially to 3/4 wet 1/4 dry signal (different than original freeverb)
        roomSizeMem_ = (0.75f * scaleRoom) + offsetRoom; // feedback attenuation in LBFC
        dampMem_ = 0.25f * scaleDamp;                    // pole of lowpass filters in the LBFC
        width_ = 1.0;
        frozenMode_ = false;
        update();

        gain_ = fixedGain; // input gain before sending to filters
        g_ = 0.5;          // allpass coefficient, immutable in FreeVerb

        // Scale delay line lengths according to the current sampling rate
        float fsScale = (float)AUDIO_RATE / 44100.0f;
        if (fsScale != 1.0f)
        {
            // scale comb filter delay lines
            for (int16_t i = 0; i < nCombs; i++)
            {
                cDelayLengths[i] = (int16_t)floor(fsScale * cDelayLengths[i]);
            }

            // Scale allpass filter delay lines
            for (int16_t i = 0; i < nAllpasses; i++)
            {
                aDelayLengths[i] = (int16_t)floor(fsScale * aDelayLengths[i]);
            }
        }

        // Initialize delay lines for the LBFC filters
        for (int16_t i = 0; i < nCombs; i++)
        {
            combDelayL_[i].setMaximumDelay(cDelayLengths[i]);
            combDelayL_[i].setDelay(cDelayLengths[i]);
            combDelayR_[i].setMaximumDelay(cDelayLengths[i] + stereoSpread);
            combDelayR_[i].setDelay(cDelayLengths[i] + stereoSpread);
        }

        // initialize delay lines for the allpass filters
        for (int16_t i = 0; i < nAllpasses; i++)
        {
            allPassDelayL_[i].setMaximumDelay(aDelayLengths[i]);
            allPassDelayL_[i].setDelay(aDelayLengths[i]);
            allPassDelayR_[i].setMaximumDelay(aDelayLengths[i] + stereoSpread);
            allPassDelayR_[i].setDelay(aDelayLengths[i] + stereoSpread);
        }
    };

    //! Destructor
    ~MEAP_FreeVerb()
    {
        free(last_output_);
    };

    //! Set the effect mix [0 = mostly dry, 1 = mostly wet].
    void setEffectMix(float mix)
    {
        MEAP_Effect::setEffectMix(mix);
        update();
    };

    //! Set the room size (comb filter feedback gain) parameter [0,1].
    void setRoomSize(float roomSize)
    {
        roomSizeMem_ = (roomSize * scaleRoom) + offsetRoom;
        update();
    };

    //! Get the room size (comb filter feedback gain) parameter.
    float getRoomSize(void)
    {
        return (roomSizeMem_ - offsetRoom) / scaleRoom;
    };

    //! Set the damping parameter [0=low damping, 1=higher damping].
    void setDamping(float damping)
    {
        dampMem_ = damping * scaleDamp;
        update();
    };

    //! Get the damping parameter.
    float getDamping(void)
    {
        return dampMem_ / scaleDamp;
    };

    //! Set the width (left-right mixing) parameter [0,1].
    void setWidth(float width)
    {
        width_ = width;
        update();
    };

    //! Get the width (left-right mixing) parameter.
    float getWidth(void)
    {
        return width_;
    };

    //! Set the mode [frozen = 1, unfrozen = 0].
    void setMode(bool isFrozen)
    {
        frozenMode_ = isFrozen;
        update();
    };

    //! Get the current freeze mode [frozen = 1, unfrozen = 0].
    float getMode(void)
    {
        return frozenMode_;
    };

    //! Clears delay lines, etc.
    void clear(void)
    {
        // Clear LBFC delay lines
        for (int i = 0; i < nCombs; i++)
        {
            combDelayL_[i].clear();
            combDelayR_[i].clear();
        }

        // Clear allpass delay lines
        for (int i = 0; i < nAllpasses; i++)
        {
            allPassDelayL_[i].clear();
            allPassDelayR_[i].clear();
        }

        last_output_[0] = 0.0;
        last_output_[1] = 0.0;
    };

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

    //! Input one or two samples to the effect and return the specified \c channel value of the computed stereo frame.
    /*!
      Use the lastFrame() function to get both values of the computed
      stereo output frame. The \c channel argument must be 0 or 1 (the
      first channel is specified by 0).  However, range checking is only
      performed if _STK_DEBUG_ is defined during compilation, in which
      case an out-of-range value will trigger an StkError exception.
    */
    T next(T inputL, T inputR = 0.0, uint16_t channel = 0)
    {
        T fInput = (inputL + inputR) * gain_;
        T outL = 0.0;
        T outR = 0.0;

        // Parallel LBCF filters
        for (int16_t i = 0; i < nCombs; i++)
        {
            // Left channel
            // float yn = fInput + (roomSize_ * FreeVerb::undenormalize(combLPL_[i].next(FreeVerb::undenormalize(combDelayL_[i].nextOut()))));
            T yn = fInput + (roomSize_ * combLPL_[i].next(combDelayL_[i].nextOut()));
            combDelayL_[i].next(yn);
            outL += yn;

            // Right channel
            // yn = fInput + (roomSize_ * FreeVerb::undenormalize(combLPR_[i].next(FreeVerb::undenormalize(combDelayR_[i].nextOut()))));
            yn = fInput + (roomSize_ * combLPR_[i].next(combDelayR_[i].nextOut()));
            combDelayR_[i].next(yn);
            outR += yn;
        }

        // Series allpass filters
        for (int16_t i = 0; i < nAllpasses; i++)
        {
            // Left channel
            // float vn_m = FreeVerb::undenormalize(allPassDelayL_[i].nextOut());
            T vn_m = allPassDelayL_[i].nextOut();
            T vn = outL + (g_ * vn_m);
            allPassDelayL_[i].next(vn);

            // calculate output
            outL = -vn + (1.0f + g_) * vn_m;

            // Right channel
            // vn_m = FreeVerb::undenormalize(allPassDelayR_[i].nextOut());
            vn_m = allPassDelayR_[i].nextOut();
            vn = outR + (g_ * vn_m);
            allPassDelayR_[i].next(vn);

            // calculate output
            outR = -vn + (1.0f + g_) * vn_m;
        }

        // Mix output
        last_output_[0] = outL * wet1_ + outR * wet2_ + inputL * dry_;
        last_output_[1] = outR * wet1_ + outL * wet2_ + inputR * dry_;

        /*
        // Hard limiter ... there's not much else we can do at this point
        if ( lastFrame_[0] >= 1.0 ) {
          lastFrame_[0] = 0.9999;
        }
        if ( lastFrame_[0] <= -1.0 ) {
          lastFrame_[0] = -0.9999;
        }
        if ( lastFrame_[1] >= 1.0 ) {
          lastFrame_[1] = 0.9999;
        }
        if ( lastFrame_[1] <= -1.0 ) {
          lastFrame_[1] = -0.9999;
        }
        */

        return last_output_[channel];
    };

protected:
    //! Update interdependent parameters.
    void update(void)
    {
        float wet = scaleWet * effectMix_;
        dry_ = scaleDry * (1.0f - effectMix_);

        // Use the L1 norm so the output gain will sum to one while still
        // preserving the ratio of scalings in original FreeVerb
        wet /= (wet + dry_);
        dry_ /= (wet + dry_);

        wet1_ = wet * (width_ / 2.0f + 0.5f);
        wet2_ = wet * (1.0f - width_) / 2.0f;

        if (frozenMode_)
        {
            // put into freeze mode
            roomSize_ = 1.0;
            damp_ = 0.0;
            gain_ = 0.0;
        }
        else
        {
            roomSize_ = roomSizeMem_;
            damp_ = dampMem_;
            gain_ = fixedGain;
        }

        for (int16_t i = 0; i < nCombs; i++)
        {
            // set low pass filter for delay output
            combLPL_[i].setCoefficients(1.0f - damp_, -damp_);
            combLPR_[i].setCoefficients(1.0f - damp_, -damp_);
        }
    };

    // Clamp very small floats to zero, version from
    // http://music.columbia.edu/pipermail/linux-audio-user/2004-July/013489.html .
    // However, this is for 32-bit floats only.
    // static inline float undenormalize( volatile float s ) {
    //  s += 9.8607615E-32f;
    //  return s - 9.8607615E-32f;
    //}

    const static int16_t nCombs = 8;
    const static int16_t nAllpasses = 4;
    int16_t stereoSpread = 23;
    float fixedGain = 0.015;
    float scaleWet = 3;
    float scaleDry = 2;
    float scaleDamp = 0.4;
    float scaleRoom = 0.28;
    float offsetRoom = 0.7;

    // Delay line lengths for 44100Hz sampling rate.
    // int16_t cDelayLengths[nCombs];
    // int16_t aDelayLengths[nAllpasses];

    float g_; // allpass coefficient
    float gain_;
    float roomSizeMem_, roomSize_;
    float dampMem_, damp_;
    float wet1_, wet2_;
    float dry_;
    float width_;
    bool frozenMode_;

    // LBFC: Lowpass Feedback Comb Filters
    MEAP_Delay<T> combDelayL_[nCombs];
    MEAP_Delay<T> combDelayR_[nCombs];
    MEAP_OnePole<T> combLPL_[nCombs];
    MEAP_OnePole<T> combLPR_[nCombs];

    // AP: Allpass Filters
    MEAP_Delay<T> allPassDelayL_[nAllpasses];
    MEAP_Delay<T> allPassDelayR_[nAllpasses];

    T *last_output_;

    int16_t cDelayLengths[nCombs] = {1617, 1557, 1491, 1422, 1356, 1277, 1188, 1116};
    int16_t aDelayLengths[nAllpasses] = {225, 556, 441, 341};
};

#endif // MEAP_FREEVERB_H
