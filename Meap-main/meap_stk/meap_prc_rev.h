#ifndef MEAP_PRCREV_H
#define MEAP_PRCREV_H

// #include "Effect.h"

#include "meap_delay.h"
#include "meap_effect.h"

/***************************************************/
/*! \class stk::PRCRev
    \brief Perry's simple reverberator class.

    This class takes a monophonic input signal and produces a stereo
    output signal.  It is based on some of the famous Stanford/CCRMA
    reverbs (NRev, KipRev), which were based on the
    Chowning/Moorer/Schroeder reverberators using networks of simple
    allpass and comb delay filters.  This class implements two series
    allpass units and two parallel comb filters.

    by Perry R. Cook and Gary P. Scavone, 1995--2019.
*/
/***************************************************/

template <class T = int16_t>
class MEAP_PRCRev : public MEAP_Effect
{
public:
    //! Class constructor taking a T60 decay time (T60 > 0) argument (one second default value).
    MEAP_PRCRev()
    {
    }

    void init(float T60 = 1.0)
    {
        allpassDelays_ = new MEAP_Delay<T>[2];
        combDelays_ = new MEAP_Delay<T>[2];

        // Delay lengths for 44100 Hz sample rate.
        int lengths[4] = {341, 613, 1557, 2137};
        float scaler = ((float)AUDIO_RATE) / 44100.0f;

        // Scale the delay lengths if necessary.
        int delay, i;
        if (scaler != 1.0f)
        {
            for (i = 0; i < 4; i++)
            {
                delay = (int)floor(scaler * lengths[i]);
                if ((delay & 1) == 0)
                    delay++;
                while (!this->isPrime(delay))
                    delay += 2;
                lengths[i] = delay;
                // Serial.println(lengths[i]);
            }
        }

        for (i = 0; i < 2; i++)
        {
            allpassDelays_[i].setMaximumDelay(lengths[i]);
            allpassDelays_[i].setDelay(lengths[i] - 1);

            combDelays_[i].setMaximumDelay(lengths[i + 2]);
            combDelays_[i].setDelay(lengths[i + 2] - 1);
        }

        this->setT60(T60);
        allpassCoefficient_ = 0.7;
        effectMix_ = 0.5;
        this->clear();
    }

    //! Reset and clear all internal state.
    void clear(void)
    {
        allpassDelays_[0].clear();
        allpassDelays_[1].clear();
        combDelays_[0].clear();
        combDelays_[1].clear();
        last_output_[0] = 0.0;
        last_output_[1] = 0.0;
    };

    //! Set the reverberation T60 decay time.
    void setT60(float T60)
    {
        combCoefficient_[0] = pow(10.0f, (-3.0f * combDelays_[0].getDelay() / (T60 * AUDIO_RATE)));
        combCoefficient_[1] = pow(10.0f, (-3.0f * combDelays_[1].getDelay() / (T60 * AUDIO_RATE)));

        // Serial.println(combCoefficient_[0]);
        // Serial.println(combCoefficient_[0]);
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

        T temp, temp0, temp1, temp2, temp3;

        temp = allpassDelays_[0].lastOut();
        temp0 = allpassCoefficient_ * temp;
        temp0 += input;
        allpassDelays_[0].next(temp0);
        temp0 = -(allpassCoefficient_ * temp0) + temp;

        temp = allpassDelays_[1].lastOut();
        temp1 = allpassCoefficient_ * temp;
        temp1 += temp0;
        allpassDelays_[1].next(temp1);
        temp1 = -(allpassCoefficient_ * temp1) + temp;

        temp2 = temp1 + (combCoefficient_[0] * combDelays_[0].lastOut());
        temp3 = temp1 + (combCoefficient_[1] * combDelays_[1].lastOut());

        last_output_[0] = effectMix_ * (combDelays_[0].next(temp2));
        last_output_[1] = effectMix_ * (combDelays_[1].next(temp3));
        temp = (1.0f - effectMix_) * input;
        last_output_[0] += temp;
        last_output_[1] += temp;

        return last_output_[channel];
    }

protected:
    MEAP_Delay<T> *allpassDelays_;
    MEAP_Delay<T> *combDelays_;

    float allpassCoefficient_;
    float combCoefficient_[2];

    T last_output_[2];
};

#endif // MEAP_PRCREV_H
