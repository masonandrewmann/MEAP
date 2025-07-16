#ifndef MEAP_TWOZERO_H
#define MEAP_TWOZERO_H

#include "meap_filter.h"

/***************************************************/
/*! \class stk::TwoZero
    \brief STK two-zero filter class.

    This class implements a two-zero digital filter.  A method is
    provided for creating a "notch" in the frequency response while
    maintaining a constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995--2019.
*/
/***************************************************/

template <class T = int16_t>
class MEAP_TwoZero : public MEAP_Filter
{
public:
    //! Default constructor creates a second-order pass-through filter.
    MEAP_TwoZero()
    {
        gain_ = 1.0;
        b_ = (float *)calloc(3, sizeof(float));
        b_[0] = 1.0;

        inputs_ = (T *)calloc(3, sizeof(T));
    };

    //! Class destructor.
    ~MEAP_TwoZero(){};

    //! Set the b[0] coefficient value.
    void setB0(float b0) { b_[0] = b0; };

    //! Set the b[1] coefficient value.
    void setB1(float b1) { b_[1] = b1; };

    //! Set the b[2] coefficient value.
    void setB2(float b2) { b_[2] = b2; };

    //! Set all filter coefficients.
    void setCoefficients(float b0, float b1, float b2, bool clearState = false)
    {
        b_[0] = b0;
        b_[1] = b1;
        b_[2] = b2;

        if (clearState)
            this->clear();
    };

    //! Sets the filter coefficients for a "notch" at \e frequency (in Hz).
    /*!
      This method determines the filter coefficients corresponding to
      two complex-conjugate zeros with the given \e frequency (in Hz)
      and \e radius from the z-plane origin.  The coefficients are then
      normalized to produce a maximum filter gain of one (independent of
      the filter \e gain parameter).  The resulting filter frequency
      response has a "notch" or anti-resonance at the given \e
      frequency.  The closer the zeros are to the unit-circle (\e radius
      close to or equal to one), the narrower the resulting notch width.
      The \e frequency value should be between zero and half the sample
      rate.  The \e radius value should be positive.
    */
    void setNotch(float frequency, float radius)
    {
        // #if defined(_STK_DEBUG_)
        //         if (frequency < 0.0f || frequency > 0.5 * Stk::sampleRate())
        //         {
        //             oStream_ << "TwoZero::setNotch: frequency argument (" << frequency << ") is out of range!";
        //             handleError(StkError::WARNING);
        //             return;
        //         }
        //         if (radius < 0.0f)
        //         {
        //             oStream_ << "TwoZero::setNotch: radius argument (" << radius << ") is negative!";
        //             handleError(StkError::WARNING);
        //             return;
        //         }
        // #endif

        b_[2] = radius * radius;
        b_[1] = -2.0f * radius * cos(6.2831853072 * frequency / (float)AUDIO_RATE);

        // Normalize the filter gain.
        if (b_[1] > 0.0f) // Maximum at z = 0.
            b_[0] = 1.0f / (1.0f + b_[1] + b_[2]);
        else // Maximum at z = -1.
            b_[0] = 1.0f / (1.0f - b_[1] + b_[2]);
        b_[1] *= b_[0];
        b_[2] *= b_[0];
    };

    //! Return the last computed output value.
    float lastOut(void) const { return last_output_; };

    //! Input one sample to the filter and return one output.
    float next(float input)
    {
        inputs_[0] = gain_ * input;
        last_output_ = b_[2] * inputs_[2] + b_[1] * inputs_[1] + b_[0] * inputs_[0];
        inputs_[2] = inputs_[1];
        inputs_[1] = inputs_[0];

        return last_output_;
    };

protected:
    float *b_;

    T *inputs_;

    T last_output_;
};

#endif // MEAP_TWOZERO_H
